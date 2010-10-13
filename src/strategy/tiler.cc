//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Wed Oct 13 20:37:06 2010 texane
// Last update Wed Oct 13 21:19:07 2010 texane
//


#include <stdio.h>
#include "bot.hh"
#include "strategy/strategy.hh"


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
