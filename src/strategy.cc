//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Fri Oct  8 12:11:44 2010 texane
// Last update Fri Oct  8 23:45:50 2010 texane
//


#include "bot.hh"


void bot::wandering_strategy()
{
}


void bot::debug_strategy()
{
  _asserv.set_velocity(400);

#if 1 // turn_to

  _asserv.turn_to(33);
  _asserv.wait_done();
  ::usleep(1000000);

  _asserv.turn_to(0);
  _asserv.wait_done();
  ::usleep(1000000);

  _asserv.turn_to(190);
  _asserv.wait_done();
  ::usleep(1000000);

#elif 0 // move_to

  _asserv.move_to(1500, 500);
  _asserv.wait_done();

#elif 0 // square

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

#endif
}
