//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Wed Oct 13 18:38:16 2010 texane
// Last update Fri Oct 15 17:29:56 2010 texane
//


#include <stdio.h>
#include "bot.hh"
#include "strategy/strategy.hh"


// strategy entrypoint

void test::main(bot& b)
{
  printf("test strat\n");

  b._ticker.reset();
  b._asserv.set_velocity(350);

#if 0 // grabber

  printf("moving\n");

  const unsigned int min_dist =
    b._clamp.grabbing_distance() - 40;
  unsigned int d = min_dist + 1;

  // move until pawn detected
  b._asserv.move_forward(1000);
  while (b._asserv.is_done() == false)
  {
    if ((d = util::min_front_low_sharp(b)) <= min_dist)
    {
      b._asserv.stop();
      break ;
    }
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
    unsigned int ds[4];

    util::front_low_sharps(b, ds);

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

    if (b.is_red())
      b._asserv.turn_right(90);
    else
      b._asserv.turn_left(90);

    b._asserv.wait_done();
  }

#elif 0 // wall follower

  // assume the red pawn

  bool is_moving = false;

  while (1)
  {
    if (is_moving == true)
    {
      if (b._asserv.is_done() == true)
      {
	is_moving = false;
	continue ;
      }

      const unsigned int dr = b._sharps[bot::RIGHT_LOW_FCORNER].read();
      const unsigned int df = b._sharps[bot::FRONT_LOW_RCORNER].read();

      if ((dr > 20) && (dr < 100) && (df > 20))
	continue ;

      is_moving = false;
      b._asserv.stop();
      b._asserv.wait_done();

      if (df <= 20)
	b._asserv.turn_left(45);
      else if (dr > 200)
	b._asserv.turn_right(5);
      else if (dr <= 10)
	b._asserv.turn_left(5);

      b._asserv.wait_done();
    }
    else
    {
      b._asserv.move_forward(1000);
      is_moving = true;
    }
  }

#elif 0 // test sharps

  b._asserv.move_forward(500);
  b._asserv.wait_done();

  b._asserv.turn(110);
  b._asserv.wait_done();

  for (size_t i = 0; i < bot::FRONT_COUNT; ++i)
    printf("%u\n", b._sharps[i].read());

#elif 0 // test redblu

  printf("fubar\n");

  static const char* s[] = { "blu", "red" };

  b._asserv.move_forward(3000);
  while (!b._asserv.is_done())
  {
    printf("%s\n", s[b._redblu.read()]);
    usleep(300000);
  }

  b._asserv.stop();
  b._asserv.wait_done();

#else

#endif
}
