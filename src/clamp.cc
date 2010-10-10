//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sun Oct 10 13:15:37 2010 texane
// Last update Sun Oct 10 18:01:15 2010 texane
//


#include <math.h>
#include <pthread.h>
#include "physics.hh"
#include "clamp.hh"


#define CONFIG_DEBUG_CLAMP 0
#if CONFIG_DEBUG_CLAMP
# include <stdio.h>
#endif


clamp::clamp() :
  _x(0), _y(0), _w(0), _h(0), _a(0), _is_grabbing(false)
{}


void clamp::set_info
(double x, double y, double w, double h, double a)
{
  _x = x;
  _y = y;
  _w = w;
  _h = h;
  _a = a;
}


unsigned int clamp::grabbing_distance() const
{
  return (unsigned int)_h;
}


// clamp::update()

typedef struct grab_functor
{
  // physics space
  cpSpace* _space;

  // reference body (ie. the bot)
  const cpBody* _body;

  // clamp reference
  const clamp* _clamp;

  // cached values
  double _cosa;
  double _sina;

  // clamp (center) translated to world
  double _tcx;
  double _tcy;

  // has something been grabbed
  bool _has_grabbed;

  grab_functor(const clamp* klamp, cpSpace* space, const cpBody* body) :
    _space(space), _body(body), _clamp(klamp), _has_grabbed(false) {}

  void operator()(cpShape* shape)
  {
    if (_has_grabbed == true)
      return ;

    if (shape->klass->type != CP_CIRCLE_SHAPE)
      return ;

    // translate to clamp
    const double tx = shape->body->p.x - _tcx;
    const double ty = shape->body->p.y - _tcy;

    // rotate to clamp
    const double rx = tx * _cosa + ty * _sina;
    const double ry = ty * _cosa - tx * _sina;

#if CONFIG_DEBUG_CLAMP
    const double dx = tx - _tcx;
    const double dy = ty - _tcy;
    if (sqrt(dx * dx + dy * dy) < 250)
    {
      printf("shapeInWorld: %lf, %lf\n", shape->body->p.x, shape->body->p.y);
      printf("shapeInClamp: %lf, %lf\n", tx, ty);
      printf("shapeInClampRotated: %lf, %lf\n", rx, ry);
    }
#endif // CONFIG_DEBUG_CLAMP

    // test if shape can be grabbed
    const cpCircleShape* const cs = (const cpCircleShape*)shape;
    if ((ry - cs->r) < (-_clamp->_w / 2))
      return ;
    else if ((ry + cs->r) > (_clamp->_w / 2))
      return ;
    else if ((rx - cs->r < 0) || (rx - cs->r > _clamp->_h))
      return ;

    // remove the body from space
    remove_shape(_space, shape);

    _has_grabbed = true;
  }

} grab_functor_t;


static void on_shape(void* shape, void* functor)
{
  (*(grab_functor_t*)functor)((cpShape*)shape);
}

void clamp::update(cpSpace* space, cpBody* body)
{
  if (_is_grabbing == false)
    return ;

  // already updated
  if (_has_updated == true)
    return ;

  grab_functor_t f(this, space, body);

  // translate clamp to world
  const double cosa = ::cos(body->a);
  const double sina = ::sin(body->a);

  f._tcx = body->p.x + (_x * cosa - _y * sina);
  f._tcy = body->p.y + (_x * sina + _y * cosa);

#if CONFIG_DEBUG_CLAMP
  printf("clampInBot: %lf, %lf\n", _x, _y);
  printf("clampInWorld: %lf, %lf\n", f._tcx, f._tcy);
#endif

  // cache clamp cosa, sina
  f._cosa = ::cos(body->a + _a);
  f._sina = ::sin(body->a + _a);

  // apply functor over each bodies
  cpSpaceHashEach(space->activeShapes, on_shape, &f);
  _has_grabbed = f._has_grabbed;

  // commit
  __sync_synchronize();
  _has_updated = true;
}


bool clamp::grab()
{
  // return true if something grabbed

  _has_updated = false;
  _is_grabbing = true;
  __sync_synchronize();

  while (_has_updated == false)
    pthread_yield();

  _is_grabbing = false;

  __sync_synchronize();
  return _has_grabbed;
}
