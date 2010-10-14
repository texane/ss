//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Wed Oct 13 20:37:06 2010 texane
// Last update Thu Oct 14 05:45:24 2010 texane
//


#include <stdio.h>
#include <string.h>
#include "bot.hh"
#include "strategy/strategy.hh"


// tile routines

#define TILE_FLAG_SEEN (1 << 0)
#define TILE_FLAG_USED (1 << 1)
#define TILE_FLAG_RED (1 << 2)

static const size_t tiles_per_row = 6;
static const size_t tiles_per_col = 6;

static unsigned int tiles[tiles_per_row * tiles_per_col];

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

static void init_tiles(void)
{
  memset(tiles, 0, sizeof(tiles));
}

static inline unsigned int get_tile_at
(unsigned int x, unsigned int y)
{
  // all the above function are in tile coords
  return tiles[y * tiles_per_row + x];
}

static char tile_to_char(unsigned int tile)
{
  if ((tile & TILE_FLAG_SEEN) == 0)
  {
    return '?';
  }
  if (tile & TILE_FLAG_USED)
  {
    if (tile & TILE_FLAG_RED)
      return 'r';
    return 'b';
  }
  return ' ';
}

static void print_tiles(void)
{
  for (size_t i = 0; i < tiles_per_row; ++i)
  {
    for (size_t j = 0; j < tiles_per_col; ++j)
      printf("%c", tile_to_char(get_tile_at(i, j)));
    printf("\n");
  }
  printf("\n");
}

void tiler::main(bot& b)
{
  printf("tiler strategy\n");

  b._ticker.reset();
  b._asserv.set_velocity(400);

  for (unsigned int i = 0; i < 6; ++i)
  {
    for (unsigned int j = 0; j < 6; ++j)
    {
      unsigned int x = j;
      unsigned int y = i;

      tile_to_world(x, y);
      printf("move_to(%u, %u)\n", x, y);

      b._asserv.move_to(x, y);
      while (b._asserv.is_done() == false)
      {
	// read the sharps, save the min low
	unsigned int lowmin = (unsigned int)-1;
	unsigned int sharps[bot::SHARP_COUNT];
	for (size_t n = 0; n < bot::SHARP_COUNT; ++n)
	{
	  sharps[n] = b._sharps[n].read();

	  if (n == bot::FRONT_HIGH_MIDDLE)
	    continue ;

	  if (sharps[n] < lowmin)
	    lowmin = sharps[n];
	}

	// no nearby object
#define AVOID_DIST 150
	if (lowmin >= AVOID_DIST)
	  continue ;

	// stop
	b._asserv.stop();
	b._asserv.wait_done();

	if (sharps[bot::FRONT_HIGH_MIDDLE] >= AVOID_DIST)
	{
	  printf("placing\n");

	  printf("grabbing\n");

	  if (b._clamp.grab() == true)
	  {
	    printf("grabbed\n");

	    b._asserv.turn(180);
	    b._asserv.wait_done();

	    b._clamp.drop();

	    b._asserv.move_forward(-100);
	    b._asserv.wait_done();

	    b._asserv.turn(180);
	    b._asserv.wait_done();

	    b._asserv.move_to(x, y);

	    continue ;
	  }

	  printf("missed\n");
	}

	// avoid
	if (sharps[bot::FRONT_LEFT_CORNER] <= AVOID_DIST)
	  b._asserv.turn_left(10);
	else if (sharps[bot::FRONT_RIGHT_CORNER] <= AVOID_DIST)
	  b._asserv.turn_right(10);
	b._asserv.wait_done();

	b._asserv.move_to(x, y);
      }
    }
  }

  printf("done\n");
  b._asserv.stop();
  b._asserv.wait_done();
}
