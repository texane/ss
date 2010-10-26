//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Thu Oct 14 20:12:05 2010 texane
// Last update Thu Oct 14 22:46:51 2010 texane
//


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include "bot.hh"
#include "misc/dtor.hh"
#include "strategy/strategy.hh"
#include "strategy/tile.hh"


#define CONFIG_MIN_DIST 150


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
  STATE_FIND_FREE_TILE,
  STATE_INVALID
};


static void place_to_take_pawn(bot& b, unsigned int sharps[bot::SHARP_COUNT])
{
  /* code taken from distri fsm */

  /* return 0 if pawn taken */

  /* front sharp distance */
  unsigned int ldist;
  unsigned int rdist;

  /* index of the sharp to read */
  size_t sharp_index;

  /* angle to turn */
  unsigned int a = 0;

  /* angular velocity */
  int w = 0;

  const unsigned int grab_dist = b._clamp.grabbing_distance();

  /* rotate until object seen by the low_{left,right} */
  if (sharps[bot::FRONT_LOW_LCORNER] <= CONFIG_MIN_DIST)
  {
    /* corner sees but left misses the item, turn a bit */
    if (sharps[bot::FRONT_LOW_LEFT] > CONFIG_MIN_DIST)
    {
      sharp_index = bot::FRONT_LOW_LEFT;
      w = -300;
      a = 15;
    }
  }
  else if (sharps[bot::FRONT_LOW_RCORNER] <= CONFIG_MIN_DIST)
  {
    if (sharps[bot::FRONT_LOW_RIGHT] > CONFIG_MIN_DIST)
    {
      sharp_index = bot::FRONT_LOW_RIGHT;
      w = 300;
      a = 15;
    }
  }

  /* above resulted in something to do */
  if (a != 0)
  {
    ldist = sharps[sharp_index];
    while (a && (ldist > 120))
    {
      b._asserv.turn(a, w);
      b._asserv.wait_done();

      a -= 5;

      ldist = b._sharps[sharp_index].read();
    }
  }

  /* place to grab */
  ldist = b._sharps[bot::FRONT_LOW_LEFT].read();
  rdist = b._sharps[bot::FRONT_LOW_RIGHT].read();

  a = 8;
  while (abs(ldist - rdist) > 50)
  {
    if (ldist > rdist)
      b._asserv.turn_right(a);
    else
      b._asserv.turn_left(a);
    b._asserv.wait_done();

    if (a == 1)
      break ;

    a -= 1;

    ldist = b._sharps[bot::FRONT_LOW_LEFT].read();
    rdist = b._sharps[bot::FRONT_LOW_RIGHT].read();
  }

  /* make ldist contain the max dist */
  if (ldist < rdist)
    ldist = rdist;

  /* move near enough to grab */
  if (ldist > grab_dist)
  {
    b._asserv.move_forward(ldist - grab_dist + 20);
    b._asserv.wait_done();
  }
}


static void get_pawn_position
(bot& b, unsigned int* pawn_x, unsigned int* pawn_y)
{
  /* assuming the bot is placed to take a pawn,
     get the pawn center coordinates. assume
     the distance from the pawn center is 200
  */

  static const unsigned int dist_pawn_center = 200;

  int pos_x, pos_y, pos_a;

  b._asserv.get_position((int&)pos_x, (int&)pos_y);
  pos_a = b._asserv.get_angle();

  *pawn_x = pos_x + cos(dtor((double)pos_a)) * dist_pawn_center;
  *pawn_y = pos_y + sin(dtor((double)pos_a)) * dist_pawn_center;
}


static unsigned int get_dist_from_tile_center
(unsigned int pawn_x, unsigned int pawn_y)
{
  unsigned int tile_x = clamp_tile_x(pawn_x);
  unsigned int tile_y = pawn_y;
  unsigned int dx, dy;

  /* translating back and forth to get the center */
  /* todo: avoid the double translation */
  world_to_tile(tile_x, tile_y);
  tile_to_world(tile_x, tile_y);

  dx = abs(tile_x - pawn_x);
  dy = abs(tile_y - pawn_y);

  return (unsigned int)sqrt(dx * dx + dy * dy);
}


void moveto::main(bot& b)
{
  printf("moveto fsm\n");

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

  /* move is in progress */
  uint8_t in_progress;

  /* sharp scanning */
  size_t min_index;

  /* sharp buffered values */
  uint32_t sharps[bot::SHARP_COUNT];

  /* position */
  unsigned int pos_x;
  unsigned int pos_y;
  unsigned int pos_a;

  /* distance */
  unsigned int dist;

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
      /* peek a random destination. the choice could be
	 parametrized given remaining time, bonus area,
	 proximity, item density, opponent distance...
       */
      moveto_x = rand() % tiles_per_row;
      moveto_y = rand() % tiles_per_col;
      tile_to_world(moveto_x, moveto_y);
      state = STATE_MOVE_TO;
      break ;

    case STATE_MOVE_TO:
      in_progress = 0;

    continue_move:
      /* move to the destination unless something in front */
      min_index = util::read_front_sharps_get_min(b, sharps);
      if (sharps[min_index] < CONFIG_MIN_DIST)
      {
	if (in_progress)
	{
	  b._asserv.stop();
	  b._asserv.wait_done();
	}
	state = STATE_AVOID_COMMON;
	break ;
      }

      /* do this way to allow the above check prior moving */
      if (in_progress == 0)
      {
	in_progress = 1;
	b._asserv.move_to((int)moveto_x, (int)moveto_y);
      }
      else if (b._asserv.is_done())
      {
	/* move is done */
	state = STATE_CHOOSE_TILE;
	break ;
      }
      goto continue_move;

      break ; /* STATE_MOVE_TO */

    case STATE_AVOID_COMMON:
      printf("avoid_common\n");

      /* bot, wall, tower */
      if (sharps[bot::FRONT_HIGH_MIDDLE] < CONFIG_MIN_DIST)
	state = STATE_AVOID_HIGH;
      else
	state = STATE_AVOID_PAWN;

      break ; /* STATE_AVOID_COMMON */

    case STATE_AVOID_HIGH:
      /* move back, turn a bit and restart moveto */
      printf("state_avoid_high\n");

      b._asserv.move_forward(-150);
      b._asserv.wait_done();

#if 0 /* todo */
      if (b._sharps[bot::RIGHT_LOW_FCORNER].read() < 50)
      {
	b._asserv.turn_right(90);
	b._asserv.wait_done();
      }
      else if (b._sharps[bot::LEFT_LOW_FCORNER].read() < 50)
      {
	b._asserv.turn_left(90);
	b._asserv.wait_done();
      }
#else
      b._asserv.turn(90);
      b._asserv.wait_done();
#endif

      state = STATE_CHOOSE_TILE;

      break ; /* STATE_AVOID_HIGH */

    case STATE_AVOID_PAWN:
      printf("state_avoid_pawn\n");

      /* place in front of the pawn */
      place_to_take_pawn(b, sharps);

      /* if the pawn is not well placed, take and move it */
      get_pawn_position(b, &pos_x, &pos_y);

#if 0
      {
	unsigned int posx, posy, posa;
	b._asserv.get_position((int&)posx, (int&)posy);
	posa = b._asserv.get_angle();
	printf("position: %u %u %u %u %u\n", posa, posx, posy, pos_x, pos_y);
      }
#endif

      dist = get_dist_from_tile_center(pos_x, pos_y);

      /* translate to tile for is_tile_red */
      pos_x = clamp_tile_x(pos_x);
      world_to_tile(pos_x, pos_y);

      /* theoritical distance is 75, but 80 works best */
      if (!((dist > 80) || (is_tile_red(pos_x, pos_y) != is_red)))
      {
	/* todo: mark the tile */
	state = STATE_AVOID_HIGH;
	break ;
      }

      if (dist > 75)
	printf("pawn is not well placed: %u\n", dist);
      if (is_tile_red(pos_x, pos_y) != is_red)
	printf("pawn not on a same tile: %u, %u\n", pos_x, pos_y);

      if (b._clamp.grab() == false)
      {
	printf("grab failure\n");
	state = STATE_AVOID_HIGH;
	break ;
      }

      state = STATE_FIND_FREE_TILE;
      break ;

    case STATE_FIND_FREE_TILE:
      /* find a cell to drop the pawn to */
      printf("state_find_free_tile\n");

      /* get center of the current tile */
      /* todo: already computed before */
      b._asserv.get_position((int&)pos_x, (int&)pos_y);
      pos_x = clamp_tile_x(pos_x);
      world_to_tile(pos_x, pos_y);
      tile_to_world(pos_x, pos_y);

      /* move to the tile center */
      b._asserv.move_to(pos_x, pos_y);
      b._asserv.wait_done();

      /* todo: use previous computations */
      world_to_tile(pos_x, pos_y);
      if (is_red != is_tile_red(pos_x, pos_y))
      {
	/* not the same color, move the pawn on a neighboring
	   tile. use side sharps to detect if a tile is empty
	 */

	/* points towards the nearest direction */
	pos_a = b._asserv.get_angle() % 360;
	if ((pos_a < 45) || (pos_a > 315)) pos_a = 0;
	else if (pos_a < 135) pos_a = 90;
	else if (pos_a < 225) pos_a = 180;
	else pos_a = 270;

	b._asserv.turn_to(pos_a);
	b._asserv.wait_done();

      redo:
	dist = b._sharps[bot::LEFT_LOW_MIDDLE].read();
	if (dist < 300)
	{
	  /* ok to put on left tile */
	  b._asserv.turn_left(90);
	}
	else
	{
	  /* something on the left, test right */
	  dist = b._sharps[bot::RIGHT_LOW_MIDDLE].read();
	  b._asserv.turn_right(90);
	  if (dist < 300)
	  {
	    /* something on the right too */
	    b._asserv.wait_done();
	    goto redo;
	  }
	}

	/* wait turn is done */
	b._asserv.wait_done();
	b._asserv.move_forward(200);
	b._asserv.wait_done();
	b._clamp.drop();
      }
      else /* is_red == b.is_red() */
      {
	/* todo: is moving backward safe? */
	printf("tile ok, dropping here\n");
	b._asserv.move_forward(-175);
	b._asserv.wait_done();
	b._clamp.drop();
      }

      state = STATE_AVOID_HIGH;

      break ; /* STATE_AVOID_PAWN */

    default:
      break ;
    }

  }
}
