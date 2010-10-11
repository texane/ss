//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Fri Oct  8 12:11:44 2010 texane
// Last update Mon Oct 11 03:39:04 2010 fabien le mentec
//


#include <stdio.h>
#include "bot.hh"


// read sharps

static void read_sharps
(sensor* sharps, unsigned int* values, size_t count)
{
  for (size_t i = 0; i < count; ++i)
    values[i] = sharps[i].read();
}

static unsigned int get_min_sharp
(sensor* sharps, size_t count = 3)
{
  // assume count is 3
  unsigned int values[3];

  // return the min distance
  read_sharps(sharps, values, count);
  return std::min
    (values[0], std::min(values[1], values[2]));
}


// strategy entrypoints

void bot::debug_strategy()
{
  _ticker.reset();

  _asserv.set_velocity(400);

#if 1 // grabber

  const unsigned int min_dist = _clamp.grabbing_distance() - 40;
  unsigned int d = min_dist + 1;

  printf("moving\n");

  // move until pawn detected
  _asserv.move_forward(1000);
  while (_asserv.is_done() == false)
    if ((d = get_min_sharp(_sharps)) <= min_dist)
    {
      _asserv.stop();
      break ;
    }

  if (d > min_dist)
  {
    printf("notPlacing\n");
    return ;
  }

  // wait for pending stop command
  _asserv.wait_done();

  printf("placing\n");

  // pawn reached, place
  unsigned int a = 30;
  unsigned int iter = 20;
  for (; iter; --iter)
  {
    unsigned int ds[3];
    read_sharps(_sharps, ds, 3);

    if (ds[1] < min_dist)
      break ;

    if (ds[0] < ds[1])
      _asserv.turn_left(a);
    else
      _asserv.turn_right(a);
    _asserv.wait_done();

    a /= 2;
  }

  if (iter == 0)
  {
    printf("notGrabbing\n");
    return ;
  }

  // placed, grabit
  if (_clamp.grab() == false)
  {
    printf("notGrabbed\n");
    return ;
  }

  printf("grabbed\n");

  _asserv.move_to(1400, 500);
  _asserv.wait_done();

  _clamp.drop();

  int x, y;
  _asserv.get_position(x, y);
  printf("dropped(%u, %u)\n", x, y);

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
  const unsigned int dist = _sharps[2].read();
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
  printf("d == %u\n", _sharps[1].read());
  return ;

  _asserv.move_to(1500, 200);
  _asserv.wait_done();

  printf("d == %u\n", _sharps[1].read());

  _asserv.turn_to(90);
  _asserv.wait_done();

  printf("d == %u\n", _sharps[1].read());

  _asserv.move_forward(400);
  _asserv.wait_done();

  printf("d == %u\n", _sharps[1].read());

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
	const unsigned int min = get_min_sharp(_sharps);
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
      if (get_min_sharp(_sharps) <= MIN_DIST)
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
