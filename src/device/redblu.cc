#include <math.h>
#include <pthread.h>
#include "redblu.hh"
#include "physics/physics.hh"
#include "strategy/tile.hh"
#include "misc/dtor.hh"
#include "misc/rtod.hh"


// exported

redblu::redblu()
{
  _is_sensing = false;
}


void redblu::set_info(double x, double y, double h)
{
  _x = x;
  _y = y;
  _h = h;
}


void redblu::update(cpSpace* space, cpBody* body)
{
  if (_is_sensing == false)
    return ;

  // already updated
  if (_has_updated == true)
    return ;

  const double cosa = ::cos(body->a);
  const double sina = ::sin(body->a);

  unsigned int x = (unsigned int)(body->p.x + (_x * cosa - _y * sina));
  unsigned int y = (unsigned int)(body->p.y + (_x * sina + _y * cosa));

  x = clamp_tile_x(x);
  world_to_tile(x, y);
  _is_red = is_tile_red(x, y);

  __sync_synchronize();
  _has_updated = true;
}


unsigned int redblu::read()
{
  // todo: could be computed without posting a message
  // result undefined if used outside the tiled area

  _has_updated = false;
  _is_sensing = true;
  __sync_synchronize();

  while (_has_updated == false)
    pthread_yield();

  _is_sensing = false;

  __sync_synchronize();

  return _is_red;
}
