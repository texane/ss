//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Fri Oct  8 12:11:44 2010 texane
// Last update Fri Oct  8 17:45:08 2010 texane
//


#include "bot.hh"


void bot::wandering_strategy()
{
}


void bot::square_strategy()
{
  _asserv.set_velocity(400);

  for (size_t i = 0; i < 4; ++i)
  {
    _asserv.move_forward(200);
    _asserv.wait_done();

    if (is_red())
      _asserv.turn_right(90);
    else
      _asserv.turn_left(90);

    _asserv.wait_done();
  }
}
