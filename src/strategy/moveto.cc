//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Thu Oct 14 20:12:05 2010 texane
// Last update Thu Oct 14 22:46:51 2010 texane
//


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "bot.hh"
#include "strategy/strategy.hh"


/* tiles */
static const size_t tiles_per_row = 6;
static const size_t tiles_per_col = 6;
static const size_t tile_count = tiles_per_col * tiles_per_row;

static inline void tile_to_world(unsigned int& x, unsigned int& y)
{
  x = 450 + x * 350 + 350 / 2;
  y = y * 350 + 350 / 2;
}


/* fsm states */
enum state
{
  STATE_INIT = 0,
  STATE_LEAVE_START_AREA,
  STATE_CHOOSE_TILE,
  STATE_MOVE_TO,
  STATE_AVOID_COMMON,
  STATE_AVOID_HIGH,
  STATE_AVOID_PAWN,
  STATE_INVALID
};


void moveto::main(bot& b)
{
  printf("moveto fsm\n");

/*   unsigned int tiles[tiles_per_row * tiles_per_col]; */
/*   init_tiles(tiles); */

  b._ticker.reset();
  b._asserv.set_velocity(400);

  /* bot color */
  const bool is_red = b.is_red();

  /* angular velocity */
  const int w = is_red ? 300 : -300;

  /* automaton state */
  enum state state = STATE_LEAVE_START_AREA;
  
  /* where to move */
  unsigned int moveto_x = 0;
  unsigned int moveto_y = 0;

  uint8_t has_stopped;

  /* sharp buffer */
  uint32_t sharps[bot::SHARP_COUNT];

  /* schedule automaton */
  while (1)
  {
    switch (state)
    {
    case STATE_INIT:
    case STATE_LEAVE_START_AREA:
      /* leave the start area */
      b._asserv.move_forward(350);
      b._asserv.wait_done();
      b._asserv.turn(95, w);
      b._asserv.wait_done();
      b._asserv.move_forward(300);
      b._asserv.wait_done();
      state = STATE_CHOOSE_TILE;
      break ;

    case STATE_CHOOSE_TILE:
      /* peek a random destination */
      moveto_x = rand() % tiles_per_row;
      moveto_y = rand() % tiles_per_col;
      tile_to_world(moveto_x, moveto_y);
      state = STATE_MOVE_TO;
      break ;

    case STATE_MOVE_TO:
      /* move to the destination unless stopped by something */
      has_stopped = 0;
      b._asserv.move_to((int)moveto_x, (int)moveto_y);
      while (b._asserv.is_done() == false)
      {
	/* get minimum value. stop if < 200 */
	const size_t min_index = util::read_front_sharps_get_min(b, sharps);
	if (sharps[min_index] < 200)
	{
	  has_stopped = 1;
	  b._asserv.stop();
	  b._asserv.wait_done();
	  break ;
	}
      }

      if (has_stopped == 1)
	state = STATE_AVOID_COMMON;
      else
	state = STATE_CHOOSE_TILE;

      break ; /* STATE_MOVE_TO */

    case STATE_AVOID_COMMON:
      /* bot, wall, tower */
      if (sharps[bot::FRONT_HIGH_MIDDLE])
	state = STATE_AVOID_HIGH;
      else
	state = STATE_AVOID_PAWN;

      break ; /* STATE_AVOID_COMMON */

    case STATE_AVOID_HIGH:
      /* turn a bit and restart moveto */
      printf("state_avoid_high\n");
      b._asserv.turn(30);
      b._asserv.wait_done();
      state = STATE_CHOOSE_TILE;

      break ; /* STATE_AVOID_HIGH */

    case STATE_AVOID_PAWN:
      printf("state_avoid_pawn\n");
      b._asserv.turn(30);
      b._asserv.wait_done();
      state = STATE_CHOOSE_TILE;

#if 0 /* todo */
      if (place_and_take() == false)
      {
	state = STATE_CHOOSE_TILE;
	break ;
      }

      goto_tile_center();
      const int angle = b._asserv.get_angle() % 360;
      angle_to_dir();
      orient_toward(dir);

      b._asserv.get_position(cur_x, cur_y);

      if (is_red != is_tile_red(cur_x, cur_y))
      {
	// not the same color, move the pawn on a neighboring 
	// tile. use side sharps to detect if a tile is empty
      redo:
	ldist = b._sharps[bot::LEFT_LOW_MIDDLE].read();
	if (ldist > 300)
	{
	  b._asserv.turn_left(90);
	}
	else
	{
	  rdist = b._sharps[bot::RIGHT_LOW_MIDDLE].read();
	  if (rdist > 300)
	    b._asserv.turn_left(90);
	  else
	    turn_(90);
	}
	b._asserv.wait_done();
	if (is_free == false)
	  goto redo;
      }
      else // is_red == b.is_red()
      {
	// same color, drop on the tile we are on
      }

      // continue moving to
      b._asserv.move_to(moveto_x, moveto_y);

#endif /* todo */

      break;

    default:
      break ;
    }

  }
}
