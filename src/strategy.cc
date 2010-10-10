//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Fri Oct  8 12:11:44 2010 texane
// Last update Sun Oct 10 16:20:38 2010 texane
//


#include "bot.hh"


void bot::debug_strategy()
{
  _ticker.reset();

  _asserv.set_velocity(400);

#if 1 // grabber

  const unsigned int min_dist = _clamp.grabbing_distance() - 40;

  unsigned int d = do_sharps();
  if (d < min_dist) goto do_grab;

  _asserv.move_forward(1000);
  while (_asserv.is_done() == false)
  {
    if ((d = do_sharps()) <= min_dist)
      _asserv.stop();
  }

  if (d > min_dist)
  {
    printf("notgrabbing\n");
    return ;
  }

  // pawn reached, grab it
 do_grab:
  int x, y;
  _asserv.get_position(x, y);
  printf("grabbing at (%d, %d) %u\n", x, y, d);

  if (_clamp.grab() == true)
    printf("grabbed\n");
  else
    printf("missed\n");

#elif 0 // turn_to

  _asserv.turn_to(313);
  _asserv.wait_done();
  ::usleep(1000000);

  _asserv.turn_to(0);
  _asserv.wait_done();
  ::usleep(1000000);

  _asserv.turn_to(190);
  _asserv.wait_done();
  ::usleep(1000000);

#elif 0 // sensor

  if (is_red() == false) return ;

//   _asserv.move_forward(500);
//   while (_asserv.is_done() == false)
//   {
  const unsigned int dist = _sharps[2].sense();
  printf("d == %u\n", dist);
//     if (dist > MIN_DIST)
//       continue ;

//     _asserv.stop();
//   }

  return ;

  _asserv.move_to(1500, 200);
  _asserv.wait_done();

  _asserv.turn_to(90);
  _asserv.wait_done();
  printf("d == %u\n", _sharps[1].sense());
  return ;

  _asserv.move_to(1500, 200);
  _asserv.wait_done();

  printf("d == %u\n", _sharps[1].sense());

  _asserv.turn_to(90);
  _asserv.wait_done();

  printf("d == %u\n", _sharps[1].sense());

  _asserv.move_forward(400);
  _asserv.wait_done();

  printf("d == %u\n", _sharps[1].sense());

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
  _ticker.reset();

  _asserv.set_velocity(400);

  bool is_moving = false;

  while (1)
  {
    if (is_moving == true)
    {
      if (_asserv.is_done() == false)
      {
	const unsigned int min = do_sharps();
#define MIN_DIST 400U
	if (min <= MIN_DIST)
	{
	  _asserv.stop();
	  _asserv.wait_done();
	  is_moving = false;

	  _asserv.turn(10);
	  _asserv.wait_done();
	}
      }
      else
      {
	is_moving = false;
      }
    }
    else // is_moving == false
    {
      if (do_sharps() <= MIN_DIST)
      {
	_asserv.turn(10);
	_asserv.wait_done();
      }
      else
      {
	_asserv.move_forward(300);
	is_moving = true;
      }
    }
  }
}
