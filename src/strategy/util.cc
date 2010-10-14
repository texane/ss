//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Oct 11 20:42:39 2010 texane
// Last update Thu Oct 14 20:06:54 2010 texane
//


#include <sys/types.h>
#include "bot.hh"
#include "device/sensor.hh"
#include "strategy/strategy.hh"


// read sharps

void util::front_low_sharps(bot& b, unsigned int values[4])
{
  values[0] = b._sharps[bot::FRONT_LOW_LCORNER].read();
  values[1] = b._sharps[bot::FRONT_LOW_LEFT].read();
  values[2] = b._sharps[bot::FRONT_LOW_RIGHT].read();
  values[3] = b._sharps[bot::FRONT_LOW_RCORNER].read();
}

static inline unsigned int min(unsigned int a, unsigned int b)
{
  return a < b ? a : b;
}

unsigned int util::min_front_low_sharp(bot& b)
{
  unsigned int mv = -1U;

  mv = min(mv, b._sharps[bot::FRONT_LOW_LCORNER].read());
  mv = min(mv, b._sharps[bot::FRONT_LOW_LEFT].read());
  mv = min(mv, b._sharps[bot::FRONT_LOW_RIGHT].read());
  mv = min(mv, b._sharps[bot::FRONT_LOW_RCORNER].read());

  return mv;
}

unsigned int util::front_high_middle_sharp(bot& b)
{
  return b._sharps[bot::FRONT_HIGH_MIDDLE].read();
}


// get the robot front position

#include <math.h>
#include "misc/dtor.hh"

void util::get_front_position
(bot& b, unsigned int& x, unsigned int& y)
{
  // todo: optimize

  const double a = dtor((double)b._asserv.get_angle());

  // robot rotate to world. fronty is 0, thus dont mul.
  const double frontx = (200 / 2) * ::cos(a) ; // - fronty * sina
  const double fronty = (200 / 2) * ::sin(a) ; // + fronty * cosa;

  // translate to world
  int posx, posy;
  b._asserv.get_position(posx, posy);
  x = (unsigned int)((double)posx + frontx);
  y = (unsigned int)((double)posy + fronty);
}
