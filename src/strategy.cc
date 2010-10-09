//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Fri Oct  8 12:11:44 2010 texane
// Last update Sat Oct  9 15:35:27 2010 texane
//


#include "bot.hh"


void bot::debug_strategy()
{
  _asserv.set_velocity(400);

#if 0 // turn_to

  _asserv.turn_to(313);
  _asserv.wait_done();
  ::usleep(1000000);

  _asserv.turn_to(0);
  _asserv.wait_done();
  ::usleep(1000000);

  _asserv.turn_to(190);
  _asserv.wait_done();
  ::usleep(1000000);

#elif 1 // sensor

  if (is_red() == false) return ;

//   _asserv.turn_to(90);
//   _asserv.wait_done();

  printf("d == %u\n", _lsharp.sense());

  _asserv.move_forward(-50);
  _asserv.wait_done();

  printf("d == %u\n", _lsharp.sense());

#elif 0 // move_to

  int x, y;
  _asserv.get_position(x, y);

  _asserv.move_to(500, 500);
  _asserv.wait_done();

//   _asserv.move_to(400, 400);
//   _asserv.wait_done();

  _asserv.move_to(x, y);
  _asserv.wait_done();

//   _asserv.move_to(200, 200);
//   _asserv.wait_done();

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


void bot::wandering_strategy()
{
#define MIN_DIST 300 // 30 cms

  _asserv.set_velocity(400);

  bool is_moving = false;

  while (1)
  {
    if (is_moving == true)
    {
      if (_asserv.is_done() == false)
      {
	const unsigned int hdist = _hsharp.sense();
	const unsigned int ldist = _lsharp.sense();
	if ((hdist < MIN_DIST) || (ldist < MIN_DIST))
	{
	  _asserv.stop();
	  _asserv.wait_done();
	}
      }
      else
      {
	is_moving = false;
      }
    }
    else
    {
      const unsigned int hdist = _hsharp.sense();
      const unsigned int ldist = _lsharp.sense();
      if ((hdist < MIN_DIST) || (ldist < MIN_DIST))
      {
     	_asserv.turn(15);
	_asserv.stop();
      }
      else
      {
	_asserv.move_forward(300);
	is_moving = true;
      }
    }
  }
}
