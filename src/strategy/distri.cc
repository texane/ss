//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Thu Oct 14 20:12:05 2010 texane
// Last update Thu Oct 14 20:56:35 2010 texane
//


#include <stdio.h>
#include <string.h>
#include "bot.hh"
#include "strategy/strategy.hh"


void distri::main(bot& b)
{
  printf("distri strategy\n");

  b._ticker.reset();
  b._asserv.set_velocity(400);

  // minimum grabbing distance
  const unsigned int grab_dist = b._clamp.grabbing_distance();

  // index of the side sharps
  const size_t side_lo =
    b.is_red() ? bot::RIGHT_LOW_MIDDLE : bot::LEFT_LOW_MIDDLE;
  const size_t side_hi =
    b.is_red() ? bot::RIGHT_HIGH_MIDDLE : bot::LEFT_HIGH_MIDDLE;

  b._asserv.move_forward(270);
  b._asserv.wait_done();

  b._asserv.turn_to(90);
  b._asserv.wait_done();

  b._asserv.move_forward(3000);
  while (b._asserv.is_done() == false)
  {
    if (b._sharps[side_lo].read() > 200)
      continue ;
    b._asserv.stop();
  }
}
