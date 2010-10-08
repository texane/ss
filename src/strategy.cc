//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Fri Oct  8 12:11:44 2010 texane
// Last update Fri Oct  8 13:02:35 2010 texane
//


#include "bot.hh"


void bot::wandering_strategy()
{
}


void bot::square_strategy()
{
  const int dir = (is_red() ? 1 : -1);

  _asserv.set_velocity(400);

  _asserv.move_forward(800);
  _asserv.wait_done();

  for (size_t i = 0; i < 4; ++i)
  {
    _asserv.move_forward(400);
    _asserv.wait_done();

    _asserv.turn(dir * 90);
    _asserv.wait_done();
  }
}
