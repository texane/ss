//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Thu Oct 14 20:12:05 2010 texane
// Last update Thu Oct 14 22:46:51 2010 texane
//


#include <stdio.h>
#include <string.h>
#include <math.h>
#include "bot.hh"
#include "strategy/strategy.hh"


// tile routines

#define TILE_FLAG_USED (1 << 0)
#define TILE_FLAG_RED (1 << 1)

static const size_t tiles_per_row = 6;
static const size_t tiles_per_col = 6;
static const size_t tile_count = tiles_per_col * tiles_per_row;

static inline void init_tiles(unsigned int* tiles)
{
  memset(tiles, 0, tile_count * sizeof(unsigned int));
}

static inline void tile_to_world
(unsigned int& x, unsigned int& y)
{
  x = 450 + x * 350 + 350 / 2;
  y = y * 350 + 350 / 2;
}

static inline void world_to_tile
(unsigned int& x, unsigned int& y)
{
  // assume x >= 450
  x = (x - 450) / 350;
  y = y / 350;
}

static inline unsigned int& get_tile_at
(unsigned int* tiles, unsigned int x, unsigned int y)
{
  // all the above function are in tile coords
  return tiles[y * tiles_per_row + x];
}

static inline bool is_tile_used
(const unsigned int& tile)
{
  return tile & TILE_FLAG_USED;
}

static inline bool is_tile_used
(unsigned int* tiles, unsigned int x, unsigned int y)
{
  return is_tile_used(get_tile_at(tiles, x, y));
}

static inline void set_tile_used
(unsigned int& tile, bool is_red)
{
  tile |= TILE_FLAG_USED;
  if (is_red == true)
    tile |= TILE_FLAG_RED;
}

static inline void set_tile_used
(unsigned int* tiles, unsigned int x, unsigned int y, bool is_red)
{
  set_tile_used(get_tile_at(tiles, x, y), is_red);
}

static inline void clear_tile_used(unsigned int& tile)
{
  tile &= ~(TILE_FLAG_USED | TILE_FLAG_RED);
}

static inline void clear_tile_used
(unsigned int* tiles, unsigned int x, unsigned int y)
{
  clear_tile_used(get_tile_at(tiles, x, y));
}

static inline bool is_tile_red
(unsigned int x, unsigned int y)
{
  return (y & 1) ^ (x & 1); 
}

static inline void get_tile_xy
(unsigned int* tiles, unsigned int* tile, unsigned int& x, unsigned int& y)
{
  const size_t pos = tile - tiles;
  x = pos / tiles_per_row;
  y = pos % tiles_per_col;
}

static bool find_free_neighbor_tile
(unsigned int* tiles, bool is_red, unsigned int& x, unsigned int& y)
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
    else if (is_tile_used(tiles, nx, ny))
      continue ;

    // found a free is_red tile
    x = (unsigned int)nx;
    y = (unsigned int)ny;

    return true;
  }

  return false;
}


// automaton state

enum state
{
  STATE_INIT = 0,
  STATE_LEAVE_START_AREA,
  STATE_FIND_PAWN,
  STATE_PLACE_TO_GRAB,
  STATE_TAKE_PAWN,
  STATE_DISTRI_DONE,
  STATE_INVALID
};


// map related


void distri::main(bot& b)
{
  printf("distri strategy\n");

  unsigned int tiles[tiles_per_row * tiles_per_col];
  init_tiles(tiles);

  b._ticker.reset();
  b._asserv.set_velocity(400);

  // angle to turn to at initialisation
  const unsigned int init_a = b.is_red() ? 95 : 85;

  // angle to turn to at grabbing
  unsigned int grab_a;

  // tile row, coloumn to walk
  const unsigned int next_col = b.is_red() ? 5 : 0;
  unsigned int next_row = 1;

  // minimum grabbing distance
  const unsigned int grab_dist = b._clamp.grabbing_distance();

  // index of the side sharps
  const size_t side_lo =
    b.is_red() ? bot::RIGHT_LOW_MIDDLE : bot::LEFT_LOW_MIDDLE;
  const size_t side_hi =
    b.is_red() ? bot::RIGHT_HIGH_MIDDLE : bot::LEFT_HIGH_MIDDLE;
  const size_t side_co =
    b.is_red() ? bot::RIGHT_LOW_FCORNER : bot::LEFT_LOW_FCORNER;
  const int w = b.is_red() ? 300 : -300;

  // automaton state
  enum state state = STATE_LEAVE_START_AREA;

  // side sharps distance
  unsigned int dist_co;
  unsigned int dist_lo;

  // front sharps distance
  unsigned int ldist;
  unsigned int rdist;

  // schedule automaton
  while (1)
  {
    switch (state)
    {
    case STATE_INIT:
    case STATE_LEAVE_START_AREA:
      printf("state_leave_start\n");
      b._asserv.move_forward(350);
      b._asserv.wait_done();
      b._asserv.turn(95, w);
      b._asserv.wait_done();
      b._asserv.move_forward(300);
      b._asserv.wait_done();
      state = STATE_FIND_PAWN;
      break;

    case STATE_FIND_PAWN:
      printf("state_find_pawn\n");

      dist_co = (unsigned int)-1;
      dist_lo = (unsigned int)-1;

      b._asserv.turn_to(init_a);
      b._asserv.wait_done();

      b._asserv.move_forward(3000);

      while (b._asserv.is_done() == false)
      {
	// assume a wall is reached
	ldist = b._sharps[bot::FRONT_LOW_LCORNER].read();
	rdist = b._sharps[bot::FRONT_LOW_RCORNER].read();
	if ((ldist <= 150) && (rdist <= 150))
	{
	  printf("wall reached\n");

	  b._asserv.stop();
	  b._asserv.wait_done();
	  state = STATE_DISTRI_DONE;
	  break ;
	}

	// stop on a pawn until 2 sharps see it < 200
	if (dist_co == (unsigned int)-1)
	{
	  dist_co = b._sharps[side_co].read();
	  if (dist_co > 200)
	    dist_co = (unsigned int)-1;
	  continue ;
	}
	else if (dist_lo == (unsigned int)-1)
	{
	  dist_lo = b._sharps[side_lo].read();
	  if (dist_lo > 200)
	    dist_lo = (unsigned int)-1;
	  continue ;
	}

	// the 2 sharps saw it
	b._asserv.stop();
	b._asserv.wait_done();

	state = STATE_PLACE_TO_GRAB;
	break;
      }
      break;

    case STATE_PLACE_TO_GRAB:
      printf("place_to_grab\n");

      b._asserv.turn(90, w);
      b._asserv.wait_done();

      ldist = b._sharps[bot::FRONT_LOW_LEFT].read();
      rdist = b._sharps[bot::FRONT_LOW_RIGHT].read();

      printf("d: %u %u\n", ldist, rdist);

      // adjust orientation
      grab_a = 8;
      while (fabs(ldist - rdist) > 50)
      {
	if (ldist > rdist)
	  b._asserv.turn_right(grab_a);
	else
	  b._asserv.turn_left(grab_a);
	b._asserv.wait_done();

	grab_a -= 1;

	ldist = b._sharps[bot::FRONT_LOW_LEFT].read();
	rdist = b._sharps[bot::FRONT_LOW_RIGHT].read();
      }

      // make ldist contain the max dist
      if (ldist < rdist)
	ldist = rdist;

      // move near enough to grab
      if (ldist > grab_dist)
      {
	b._asserv.move_forward(ldist - grab_dist + 20);
	b._asserv.wait_done();
      }

      state = STATE_TAKE_PAWN;
      break ;

    case STATE_TAKE_PAWN:
      printf("take_pawn\n");

      // grabbing failure
      if (b._clamp.grab() == false)
      {
	printf("grab() == false\n");
	state = STATE_FIND_PAWN;
	break ;
      }

      b._asserv.move_forward(-100);
      b._asserv.wait_done();

      b._asserv.turn(90, w);
      b._asserv.wait_done();

      b._clamp.drop();

#warning TODO
#if 0
      next_row += 2;
      // last row reached
      if (next_row >= 6)
	state = STATE_DISTRI_DONE;
      else
#endif
	state = STATE_FIND_PAWN;

      break;

    case STATE_DISTRI_DONE:
      printf("distri_done\n");
      b._asserv.stop();
      b._asserv.wait_done();
      return ;
      break;

    default: break;
    }
  }
}
