//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Oct 11 19:43:48 2010 texane
// Last update Wed Oct 13 05:42:54 2010 texane
//


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "bot.hh"
#include "strategy/strategy.hh"



// tile routines

#define TILE_FLAG_USED (1 << 0)
#define TILE_FLAG_RED (1 << 1)

typedef unsigned int tile_t;

static const size_t tiles_per_row = 6;
static const size_t tiles_per_col = 6;
static tile_t tiles[tiles_per_row * tiles_per_col];

static inline void init_tiles(void)
{
  memset(tiles, 0, sizeof(tiles));
}

static inline void tile_to_world
(unsigned int& x, unsigned int& y)
{
  x = 4500 + x * 3500;
  y = y * 30000;
}

static inline void world_to_tile
(unsigned int& x, unsigned int& y)
{
  x = x - 4500 / 3500;
  y = y / 30000;
}

static inline tile_t* get_tile_at
(unsigned int x, unsigned int y)
{
  // all the above function are in tile coords
  return tiles + x * tiles_per_row + y;
}

static inline bool is_tile_used(const tile_t* tile)
{
  return *tile & TILE_FLAG_USED;
}

static inline void set_tile_used(tile_t* tile, bool is_red)
{
  *tile |= TILE_FLAG_USED;
  if (is_red == true)
    *tile |= TILE_FLAG_RED;
}

static inline void clear_tile_used(tile_t* tile)
{
  *tile &= ~(TILE_FLAG_USED | TILE_FLAG_RED);
}

static inline bool is_tile_red(unsigned int x, unsigned int y)
{
  return (y & 1) ^ (x & 1); 
}

static inline void get_tile_xy
(const tile_t* tile, unsigned int& x, unsigned int& y)
{
  const size_t pos = tile - tiles;
  x = pos / tiles_per_row;
  y = pos % tiles_per_col;
}

static tile_t* find_free_neighbor_tile
(bool is_red, unsigned int x, unsigned int y)
{
  // find a free neighbor tile of the same color

  // north, clockwise
  static const int dirs[8][2] =
  {
    {  0, -1 },
    {  1, -1 },
    {  1,  0 },
    {  1,  1 },
    {  0,  1 },
    { -1,  1 },
    { -1,  0 },
    { -1, -1 }
  };

  // translate to tile repere

  static const size_t ndirs = sizeof(dirs) / sizeof(dirs[0]);
  for (size_t i = 0; i < ndirs; ++i)
  {
    const int nx = (int)x + dirs[i][0];
    const int ny = (int)y + dirs[i][1];

    if ((nx == -1) || (nx >= (int)tiles_per_row))
      continue ;
    else if ((ny == -1) || (ny >= (int)tiles_per_col))
      continue ;
    else if (is_tile_red(nx, ny) != is_red)
      continue ;

    tile_t* const tile = get_tile_at(x, y);
    if (is_tile_used(tile))
      continue ;

    return tile;
  }

  return NULL;
}

// implemented as a state machine
enum state
{
  STATE_INIT = 0,
  // wander to find an object
  STATE_WANDER,
  // scan the object
  STATE_SCAN,
  STATE_AVOID,
  STATE_PLACE,
  STATE_GRAB,
  STATE_TILE,
  STATE_INVALID
};


// strategy entrypoint

void wander::main(bot& b)
{
  const char* const id = b.is_red() ? "red" : "blu"; 

  const unsigned int min_dist = b._clamp.grabbing_distance() - 10;

  printf("[%s] wander strategy\n", id);

  b._ticker.reset();

  b._asserv.set_velocity(400);

#define NEXT_STATE(__state)		\
  do {					\
    state = STATE_ ## __state;		\
    goto STATE_ ## __state ## _case;	\
  } while (0)

#define STATE_CASE(__state)		\
  case STATE_ ## __state :		\
  STATE_ ## __state ## _case:

  // goto init state
  enum state state;
  NEXT_STATE(INIT);

  while (1)
  {
    switch (state)
    {
      STATE_CASE(INIT)
      {
	NEXT_STATE(WANDER);
	break;
      }

      STATE_CASE(WANDER)
      {
	// there is something nearby, stop
	if (util::min_front_low_sharp(b) <= min_dist)
	{
	  b._asserv.stop();
	  b._asserv.wait_done();

	  // get the front absolute position
	  unsigned int tilex, tiley;
	  util::get_front_position(b, tilex, tiley);
	  world_to_tile(tilex, tiley);

	  // scan if this is not a self tile
	  if (is_tile_red(tilex, tiley) == false)
	    NEXT_STATE(SCAN);

	  // otherwise, avoid
	  b._asserv.turn(90);
	  b._asserv.wait_done();
	  NEXT_STATE(WANDER);
	}
	else if (b._asserv.is_done() == true)
	{
	  b._asserv.move_forward(1000);
	  NEXT_STATE(WANDER);
	}

	break;
      }

      STATE_CASE(SCAN)
      {
	// scan to know if this is a pawn
	if (util::front_high_middle_sharp(b) <= min_dist)
	{
	  NEXT_STATE(AVOID);
	}

	NEXT_STATE(PLACE);

	break;
      }

      STATE_CASE(AVOID)
      {
	while (1)
	{
	  if (util::min_front_low_sharp(b) > min_dist)
	    break ;
	  b._asserv.turn(10);
	  b._asserv.wait_done();
	}

	NEXT_STATE(WANDER);
	break;
      }

      STATE_CASE(PLACE)
      {
	printf("[%s] placing\n", id);

	unsigned int a = 16;

	// place
	while (1)
	{
	  const unsigned int l = b._sharps[bot::FRONT_LOW_LEFT].read();
	  const unsigned int r = b._sharps[bot::FRONT_LOW_RIGHT].read();
	  const unsigned int delta = l > r ? l - r : r - l;

	  if (delta < 10)
	  {
	    const unsigned int m = b._sharps[bot::FRONT_HIGH_MIDDLE].read();
	    if (m < 200)
	    {
	      // this was not a pawn
	      NEXT_STATE(WANDER);
	    }
	    NEXT_STATE(GRAB);
	  }
	  else if (l < r)
	    b._asserv.turn_left(a);
	  else
	    b._asserv.turn_right(a);
	  b._asserv.wait_done();

	  if ((a /= 2) == 0)
	  {
	    // didnot place
	    b._asserv.turn(90);
	    b._asserv.wait_done();
	    NEXT_STATE(WANDER);
	  }
	}
	break;
      }

      STATE_CASE(GRAB)
      {
	printf("[%s] grabbing\n", id);

	// grab
	if (b._clamp.grab() == true)
	{
	  printf("[%s] has_grabbed\n", id);
	  NEXT_STATE(TILE);
	}

	// didnot grab, unplace
	b._asserv.turn(90);
	b._asserv.wait_done();

	NEXT_STATE(WANDER);

	break;
      }

      STATE_CASE(TILE)
      {
	// find a free self tile to drop on
	unsigned int tilex, tiley;
	b._asserv.get_position((int&)tilex, (int&)tiley);
	world_to_tile(tilex, tiley);

	tile_t* const tile =
	  find_free_neighbor_tile(b.is_red(), tilex, tiley);

	// work in world coords
	tile_to_world(tilex, tiley);

	b._asserv.move_to(tilex, tiley);
	b._asserv.wait_done();

	b._asserv.move_forward(-100);
	b._asserv.wait_done();

	b._clamp.drop();

	// mark the tile as used
	set_tile_used(tile, b.is_red());

	b._asserv.turn(180);
	b._asserv.wait_done();

	NEXT_STATE(WANDER);

	break ;
      }

      default: break;
    }
  }
}
