//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Oct 11 20:38:16 2010 texane
// Last update Mon Oct 11 20:52:48 2010 texane
//


#include <stdio.h>
#include "bot.hh"
#include "strategy/utility.hh"


// strategy entrypoint

void test::main(bot& b)
{
  b._ticker.reset();
  b._asserv.set_velocity(400);

#if 1 // grabber

  printf("moving\n");

  const unsigned int min_dist =
    b._clamp.grabbing_distance() - 40;
  unsigned int d = min_dist + 1;

  // move until pawn detected
  b._asserv.move_forward(1000);
  while (b._asserv.is_done() == false)
    if ((d = get_min_sharp(b._sharps)) <= min_dist)
    {
      b._asserv.stop();
      break ;
    }

  if (d > min_dist)
  {
    printf("notPlacing\n");
    return ;
  }

  // wait for pending stop command
  b._asserv.wait_done();

  printf("placing\n");

  // pawn reached, place
  unsigned int a = 30;
  unsigned int iter = 20;
  for (; iter; --iter)
  {
    unsigned int ds[3];
    read_sharps(b._sharps, ds, 3);

    if (ds[1] < min_dist)
      break ;

    if (ds[0] < ds[1])
      b._asserv.turn_left(a);
    else
      b._asserv.turn_right(a);
    b._asserv.wait_done();

    a /= 2;
  }

  if (iter == 0)
  {
    printf("notGrabbing\n");
    return ;
  }

  // placed, grabit
  if (b._clamp.grab() == false)
  {
    printf("notGrabbed\n");
    return ;
  }

  printf("grabbed\n");

  const unsigned int start = b._ticker.get_msecs();
  b._asserv.move_to(1400, 500);
  b._asserv.wait_done();
  const unsigned int stop = b._ticker.get_msecs();
  printf("msecs: %u\n", stop - start);

  b._clamp.drop();

  int x, y;
  b._asserv.get_position(x, y);
  printf("dropped(%u, %u)\n", x, y);

#elif 0 // turn_to

  b._asserv.turn_to(313);
  b._asserv.wait_done();
  ::usleep(1000000);

  b._asserv.turn_to(0);
  b._asserv.wait_done();
  ::usleep(1000000);

  b._asserv.turn_to(190);
  b._asserv.wait_done();
  ::usleep(1000000);

#elif 0 // sensor

  if (is_red() == false) return ;

//   b._asserv.move_forward(500);
//   while (b._asserv.is_done() == false)
//   {
  const unsigned int dist = b._sharps[2].read();
  printf("d == %u\n", dist);
//     if (dist > MIN_DIST)
//       continue ;

//     b._asserv.stop();
//   }

  return ;

  b._asserv.move_to(1500, 200);
  b._asserv.wait_done();

  b._asserv.turn_to(90);
  b._asserv.wait_done();
  printf("d == %u\n", b._sharps[1].read());
  return ;

  b._asserv.move_to(1500, 200);
  b._asserv.wait_done();

  printf("d == %u\n", b._sharps[1].read());

  b._asserv.turn_to(90);
  b._asserv.wait_done();

  printf("d == %u\n", b._sharps[1].read());

  b._asserv.move_forward(400);
  b._asserv.wait_done();

  printf("d == %u\n", b._sharps[1].read());

#elif 0 // move_to

  int x, y;
  b._asserv.get_position(x, y);

  b._asserv.move_to(500, 500);
  b._asserv.wait_done();

//   b._asserv.move_to(400, 400);
//   b._asserv.wait_done();

  b._asserv.move_to(x, y);
  b._asserv.wait_done();

//   b._asserv.move_to(200, 200);
//   b._asserv.wait_done();

#elif 0 // square

  for (size_t i = 0; i < 4; ++i)
  {
    b._asserv.move_forward(200);
    b._asserv.wait_done();

    if (is_red())
      b._asserv.turn_right(90);
    else
      b._asserv.turn_left(90);

    b._asserv.wait_done();
  }

#endif
}