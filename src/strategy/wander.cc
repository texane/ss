//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Oct 11 19:43:48 2010 texane
// Last update Tue Oct 12 12:12:00 2010 texane
//


#include <stdio.h>
#include <sys/types.h>
#include "bot.hh"
#include "strategy/strategy.hh"


// strategy entrypoint

void wander::main(bot& b)
{
  printf("wander strategy\n");

  b._ticker.reset();

  b._asserv.set_velocity(400);

  bool is_moving = false;

  while (1)
  {
    if (is_moving == true)
    {
      if (b._asserv.is_done() == false)
      {
#define MIN_DIST 350U
	if (util::min_front_low_sharp(b) <= MIN_DIST)
	{
	  b._asserv.stop();
	  b._asserv.wait_done();
	  is_moving = false;

	  b._asserv.turn(10);
	  b._asserv.wait_done();
	}
      }
      else
      {
	is_moving = false;
      }
    }
    else // is_moving == false
    {
      if (util::min_front_low_sharp(b) <= MIN_DIST)
      {
	b._asserv.turn(10);
	b._asserv.wait_done();
      }
      else
      {
	b._asserv.move_forward(300);
	is_moving = true;
      }
    }
  }
}
