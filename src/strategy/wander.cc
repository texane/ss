//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Oct 11 19:43:48 2010 texane
// Last update Tue Oct 12 20:12:51 2010 texane
//


#include <stdio.h>
#include <sys/types.h>
#include "bot.hh"
#include "strategy/strategy.hh"


// implemented as a state machine
enum state
{
  STATE_INIT = 0,
  // wander to find an object
  STATE_WANDER,
  // scan the object
  STATE_SCAN,
  STATE_AVOID,
  STATE_PLACE,
  STATE_GRAB,
  STATE_TILE,
  STATE_INVALID
};


// strategy entrypoint

void wander::main(bot& b)
{
  const char* const id = b.is_red() ? "red" : "blu"; 

  printf("[%s] wander strategy\n", id);

  b._ticker.reset();

  b._asserv.set_velocity(400);

  enum state state;

#define NEXT_STATE(__state)		\
  do {					\
    state = STATE_ ## __state;		\
    goto STATE_ ## __state ## _case;	\
  } while (0)

#define STATE_CASE(__state)		\
  case STATE_ ## __state :		\
  STATE_ ## __state ## _case:

  // goto init state
  NEXT_STATE(INIT);

  while (1)
  {
    switch (state)
    {
      STATE_CASE(INIT)
      {
	NEXT_STATE(WANDER);
	break;
      }

      STATE_CASE(WANDER)
      {
	if (util::min_front_low_sharp(b) <= 200)
	{
	  b._asserv.stop();
	  b._asserv.wait_done();
	  NEXT_STATE(SCAN);
	}
	else if (b._asserv.is_done() == true)
	{
	  b._asserv.move_forward(1000);
	  NEXT_STATE(WANDER);
	}

	break;
      }

      STATE_CASE(SCAN)
      {
	// scan to know if this is a pawn
	if (util::front_high_middle_sharp(b) <= 200)
	{
	  NEXT_STATE(AVOID);
	}
	else
	{
	  NEXT_STATE(PLACE);
	}
	break;
      }

      STATE_CASE(AVOID)
      {
	while (1)
	{
	  if (util::min_front_low_sharp(b) > 200)
	    break ;
	  b._asserv.turn(10);
	  b._asserv.wait_done();
	}

	NEXT_STATE(WANDER);
	break;
      }

      STATE_CASE(PLACE)
      {
	printf("[%s] placing\n", id);

	unsigned int a = 16;

	// place
	while (1)
	{
	  const unsigned int l = b._sharps[bot::FRONT_LOW_LEFT].read();
	  const unsigned int r = b._sharps[bot::FRONT_LOW_RIGHT].read();
	  const unsigned int delta = l > r ? l - r : r - l;
	  if (delta < 5)
	    NEXT_STATE(GRAB);
	  else if (l < r)
	    b._asserv.turn_left(a);
	  else
	    b._asserv.turn_right(a);
	  b._asserv.wait_done();

	  a /= 2; if (a == 0)
	    NEXT_STATE(WANDER);
	}
	break;
      }

      STATE_CASE(GRAB)
      {
	printf("[%s] grabbing\n", id);

	// grab
	if (b._clamp.grab() == true)
	{
	  printf("[%s] has_grabbed\n", id);
	  NEXT_STATE(TILE);
	}

	// didnot grab, unplace
	b._asserv.turn(180);
	b._asserv.wait_done();
	NEXT_STATE(WANDER);
	break;
      }

      STATE_CASE(TILE)
      {
	// find a tile to drop on
	NEXT_STATE(WANDER);
	break;
      }

      default: break;
    }
  }
}
