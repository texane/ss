//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Oct 11 19:43:48 2010 texane
// Last update Thu Oct 14 21:01:38 2010 texane
//


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "bot.hh"
#include "strategy/tile.hh"
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
  // tile array
  unsigned int tiles[tiles_per_row * tiles_per_col];
  init_tiles(tiles);

  const char* const id = b.is_red() ? "red" : "blu"; 

  const unsigned int avoid_dist = 250;
  const unsigned int grab_dist = b._clamp.grabbing_distance() - 100;

  printf("[%s] wander strategy\n", id);

  b._ticker.reset();

  b._asserv.set_velocity(400);

#define NEXT_STATE(__state)		\
  do {					\
    state = STATE_ ## __state;		\
    goto STATE_ ## __state ## _case;	\
  } while (0)

#define STATE_CASE(__state)		\
  case STATE_ ## __state :		\
  STATE_ ## __state ## _case:

  // goto init state
  enum state state;
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
	// there is something nearby, stop
	if (util::min_front_low_sharp(b) <= avoid_dist)
	{
	  b._asserv.stop();
	  b._asserv.wait_done();

	  // get the front absolute position
	  unsigned int tilex, tiley;
	  util::get_front_position(b, tilex, tiley);
	  world_to_tile(tilex, tiley);

	  // scan if this is not a self tile
	  if (is_tile_red(tilex, tiley) != b.is_red())
	    NEXT_STATE(SCAN);

	  // otherwise, avoid
	  b._asserv.turn(90);
	  b._asserv.wait_done();
	  NEXT_STATE(WANDER);
	}
	else if (b._asserv.is_done() == true)
	{
	  b._asserv.move_forward(1000);
	  NEXT_STATE(WANDER);
	}

	NEXT_STATE(WANDER);

	break;
      }

      STATE_CASE(SCAN)
      {
	// scan to know if this is a pawn
	if (util::front_high_middle_sharp(b) <= avoid_dist)
	{
	  NEXT_STATE(AVOID);
	}

	NEXT_STATE(PLACE);

	break;
      }

      STATE_CASE(AVOID)
      {
	while (1)
	{
	  if (util::min_front_low_sharp(b) > avoid_dist)
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

	  // balanced engouh orientation
	  if (delta < 100)
	  {
	    printf("[%s] delta %u\n", id, delta);

	    const unsigned int m = b._sharps[bot::FRONT_HIGH_MIDDLE].read();
	    if (m < avoid_dist)
	    {
	      // this was not a pawn
	      printf("[%s] going to wander\n", id);
	      NEXT_STATE(WANDER);
	    }

	    // position near enough to grab
	    const unsigned int dist = l < r ? l : r;
	    if (dist > grab_dist)
	    {
	      printf("[%s] move_forward(%u)\n", id, dist);
	      b._asserv.move_forward(dist - grab_dist);

	      while (b._asserv.is_done() == false)
	      {
		if (util::min_front_low_sharp(b) <= grab_dist)
		{
		  // asserv::is_done will
		  b._asserv.stop();
		  break ;
		}
	      }
	    }

	    printf("[%s] going to grab\n", id);

	    NEXT_STATE(GRAB);
	  }
	  else if (l < r)
	    b._asserv.turn_left(a);
	  else
	    b._asserv.turn_right(a);
	  b._asserv.wait_done();

	  if ((a /= 2) == 0)
	  {
	    // didnot place
	    b._asserv.turn(90);
	    b._asserv.wait_done();
	    NEXT_STATE(WANDER);
	  }
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
	b._asserv.turn(90);
	b._asserv.wait_done();

	NEXT_STATE(WANDER);

	break;
      }

      STATE_CASE(TILE)
      {
	// find a free self tile to drop on
	unsigned int tilex, tiley;
	util::get_front_position(b, tilex, tiley);

	printf("[%s] pos(%u, %u)\n", id, tilex, tiley);

	if ((tilex < 450) || (tilex > 2550))
	{
	  // doesnot handle distribution area yet
	  goto drop_and_wander;
	}

	world_to_tile(tilex, tiley);

	if (find_free_neighbor_tile(tiles, b.is_red(), tilex, tiley) == false)
	{
	  printf("[%s] no file free found(%u, %u)\n", id, tilex, tiley);

	drop_and_wander:
	  b._asserv.turn(90);
	  b._asserv.wait_done();
	  b._clamp.drop();
	  b._asserv.turn(90);
	  b._asserv.wait_done();
	  NEXT_STATE(WANDER);
	}

	printf("[%s] found free tile (%u, %u)\n", id, tilex, tiley);

	// work in world coords
	tile_to_world(tilex, tiley);

	// goto the tile, avoid if necessary
	b._asserv.move_to(tilex, tiley);
	while (b._asserv.is_done() == false)
	{
	  if (util::min_front_low_sharp(b) > avoid_dist)
	    continue ;

	  b._asserv.stop();
	  b._asserv.wait_done();

	  b._asserv.turn(20);
	  b._asserv.wait_done();

	  b._asserv.move_to(tilex, tiley);
	}

	// we are on the tile, dropit
	b._asserv.move_forward(-150);
	b._asserv.wait_done();

	b._clamp.drop();

	b._asserv.move_forward(-100);
	b._asserv.wait_done();

	// mark the tile as used
	world_to_tile(tilex, tiley);
	set_tile_used(tiles, tilex, tiley, b.is_red());

	b._asserv.turn(90);
	b._asserv.wait_done();

	NEXT_STATE(WANDER);

	break ;
      }

      default: break;
    }
  }
}
