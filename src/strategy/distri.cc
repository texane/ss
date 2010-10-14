//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Thu Oct 14 20:12:05 2010 texane
// Last update Thu Oct 14 21:58:26 2010 texane
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
  const int w = b.is_red() ? 300 : -300;

  b._asserv.move_forward(290);
  b._asserv.wait_done();

  b._asserv.turn_to(90);
  b._asserv.wait_done();

  bool is_placing = false;
  unsigned int prev_dist = 0;

  b._asserv.move_forward(3000);

  while (b._asserv.is_done() == false)
  {
    if (util::min_front_low_sharp(b) <= 150)
    {
      b._asserv.stop();
      b._asserv.wait_done();
      break ;
    }

    const unsigned int dist = b._sharps[side_lo].read();
    if (dist > 200)
    {
      is_placing = false;
      continue ;
    }

    if (is_placing == false)
    {
      printf("is_placing = true, prev_dist = %u\n", dist);
      is_placing = true;
      prev_dist = dist;
    }
    else if ((prev_dist < dist) || (dist <= grab_dist))
    {
      printf("%u < %u\n", prev_dist, dist);
      is_placing = false;

      b._asserv.stop();
      b._asserv.wait_done();

      b._asserv.turn(90, w);
      b._asserv.wait_done();

      b._clamp.grab();

      b._asserv.turn(90, w);
      b._asserv.wait_done();

      b._asserv.move_forward(-100);
      b._clamp.drop();
      b._asserv.wait_done();

      b._asserv.turn(180, w);
      b._asserv.wait_done();

      b._asserv.move_forward(3000);
    }
    else
    {
      prev_dist = dist;
    }
  }

  b._asserv.stop();
  b._asserv.wait_done();
}
