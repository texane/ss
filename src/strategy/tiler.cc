//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Wed Oct 13 20:37:06 2010 texane
// Last update Thu Oct 14 04:09:40 2010 fabien le mentec
//


#include <stdio.h>
#include <string.h>
#include "bot.hh"
#include "strategy/strategy.hh"
#include "strategy/tile.hh"


// tile routines

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

	  // do not take those into account for avoidance
	  if (n == bot::FRONT_HIGH_MIDDLE)
	    continue ;
	  else if (n == bot::BACK_LOW_MIDDLE)
	    continue ;
	  else if (n == bot::BACK_HIGH_MIDDLE)
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

	printf("something in the air\n");

	// left or right wall
	if (sharps[bot::LEFT_LOW_MIDDLE] <= AVOID_DIST)
	{
	  printf("left wall\n");
	  b._asserv.turn_right(10);
	  b._asserv.wait_done();
	  continue ;
	}
	else if (sharps[bot::RIGHT_HIGH_MIDDLE] <= AVOID_DIST)
	{
	  printf("right wall\n");
	  b._asserv.turn_left(10);
	  b._asserv.wait_done();
	  continue ;
	}

	// this is a pawn
	if (sharps[bot::FRONT_HIGH_MIDDLE] >= AVOID_DIST)
	{
	  printf("pawn, grabbing (%u)\n", sharps[bot::FRONT_HIGH_MIDDLE]);

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
	if (sharps[bot::FRONT_LOW_LCORNER] <= AVOID_DIST)
	  b._asserv.turn_left(10);
	else if (sharps[bot::FRONT_LOW_RCORNER] <= AVOID_DIST)
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
