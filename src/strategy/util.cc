//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Oct 11 20:42:39 2010 texane
// Last update Wed Oct 13 03:41:55 2010 fabien le mentec
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


// get the robot front position

#include <math.h>

void util::get_front_position
(bot& b, unsigned int& x, unsigned int& y)
{
  // todo: optimize

  const double a = (double)b._asserv.get_angle();
  const double cosa = ::cos(a);
  const double sina = ::sin(a);

  // local to robot
  unsigned int frontx, fronty;
  frontx = 200 / 2;
  fronty = 0;

  // rotate to world
  frontx = (frontx * cosa - fronty * sina);
  fronty = (frontx * sina + fronty * cosa);

  // translate to world
  int posx, posy;
  b._asserv.get_position((int&)posx, (int&)posy);
  x = posx + frontx;
  y = posy + fronty;
}
