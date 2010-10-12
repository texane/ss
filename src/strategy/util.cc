//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Oct 11 20:42:39 2010 texane
// Last update Tue Oct 12 23:25:31 2010 texane
//


#include <sys/types.h>
#include "bot.hh"
#include "device/sensor.hh"
#include "strategy/strategy.hh"


// read sharps

void util::front_low_sharps(bot& b, unsigned int values[4])
{
  values[0] = b._sharps[bot::FRONT_LEFT_CORNER].read();
  values[1] = b._sharps[bot::FRONT_LOW_LEFT].read();
  values[2] = b._sharps[bot::FRONT_LOW_RIGHT].read();
  values[3] = b._sharps[bot::FRONT_RIGHT_CORNER].read();
}

static inline unsigned int min(unsigned int a, unsigned int b)
{
  return a < b ? a : b;
}

unsigned int util::min_front_low_sharp(bot& b)
{
  unsigned int mv = -1U;

  mv = min(mv, b._sharps[bot::FRONT_LEFT_CORNER].read());
  mv = min(mv, b._sharps[bot::FRONT_LOW_LEFT].read());
  mv = min(mv, b._sharps[bot::FRONT_LOW_RIGHT].read());
  mv = min(mv, b._sharps[bot::FRONT_RIGHT_CORNER].read());

  return mv;
}

unsigned int util::front_high_middle_sharp(bot& b)
{
  return b._sharps[bot::FRONT_HIGH_MIDDLE].read();
}


// map related

void util::get_front_position(bot&, unsigned int, unsigned int)
{
}
