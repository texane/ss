//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sat Oct  9 08:26:05 2010 texane
// Last update Tue Oct 12 09:56:12 2010 texane
//


#include <limits>
#include <algorithm>
#include <math.h>
#include <pthread.h>
#include "physics/physics.hh"
#include "misc/dtor.hh"
#include "misc/rtod.hh"
#include "device/sensor.hh"
#include "device/intersect.hh"


sensor::sensor()
{
  _is_sensing = false;
}


void sensor::set_info(double x, double y, double a, double h)
{
  _x = x;
  _y = y;

  _a = dtor(a);

  _h = h;
}


typedef struct ray_functor
{
  cpBody* _body;

  // ray segment coords
  cpFloat _x0;
  cpFloat _y0;
  cpFloat _x1;
  cpFloat _y1;

  // ray height
  cpFloat _h;

  cpFloat _res;

  ray_functor
  (cpBody* body, cpFloat x0, cpFloat y0, cpFloat x1, cpFloat y1, cpFloat h)
    : _body(body), _x0(x0), _y0(y0), _x1(x1), _y1(y1), _h(h)
  {
    // min distance reached
    _res = std::numeric_limits<cpFloat>::max();
  }

  // fixme: should be shape->data->_h
  static double get_shape_height(const cpShape* shape)
  {
    double h;

    switch (shape->klass->type)
    {
      // pawn or figure
    case CP_CIRCLE_SHAPE:
      h = (shape->body->m == 500.f ? 50.f : 210.f);
      break;

      // wall
    case CP_SEGMENT_SHAPE:
      h = 100.f;
      break;

      // bot
    case CP_POLY_SHAPE:
      h = 200.f;
      break;

    default:
      h = 0.f;
      break;
    }

    return h;
  }

  void operator()(cpShape* shape)
  {
    // dont shed ray on ourselves
    if (_body == shape->body)
      return ;

    // ray is above the shape
    if (_h > get_shape_height(shape))
      return ;

    switch (shape->klass->type)
    {
    case CP_CIRCLE_SHAPE:
      {
	cpCircleShape* const cs = (cpCircleShape*)shape;

	const cpFloat res = (cpFloat)intersect_circle
	  (_x0, _y0, _x1, _y1, cs->tc.x, cs->tc.y, cs->r);

	_res = std::min(_res, res);

	break;
      }

    case CP_POLY_SHAPE:
      {
	cpPolyShape* const ps = (cpPolyShape*)shape;

	cpFloat x2 = ps->tVerts[ps->numVerts - 1].x;
	cpFloat y2 = ps->tVerts[ps->numVerts - 1].y;

	for (int i = 0; i < ps->numVerts; ++i)
	{
	  const cpFloat x3 = ps->tVerts[i].x;
	  const cpFloat y3 = ps->tVerts[i].y;

	  const cpFloat res = (cpFloat)intersect_segment
	    (_x0, _y0, _x1, _y1, x2, y2, x3, y3);

	  _res = std::min(_res, res);

	  x2 = x3;
	  y2 = y3;
	}

	break;
      }

    case CP_SEGMENT_SHAPE:
      {
	cpSegmentShape* const ss = (cpSegmentShape*)shape;

	const cpFloat res = (cpFloat)intersect_segment
	  (_x0, _y0, _x1, _y1, ss->ta.x, ss->ta.y, ss->tb.x, ss->tb.y);

	_res = std::min(_res, res);

	break;
      }

    default: break;
    }
  }

  static void static_func(void* shape, void* functor)
  {
    (*(struct ray_functor*)functor)((cpShape*)shape);
  }

} ray_functor_t;


void sensor::update(cpSpace* space, cpBody* body)
{
  if (_is_sensing == false)
    return ;

  // already updated
  if (_has_updated == true)
    return ;

  // translate sensor in bot repere

  const cpFloat cosa = ::cos(body->a);
  const cpFloat sina = ::sin(body->a);
  const cpFloat x0 = body->p.x + (_x * cosa - _y * sina);
  const cpFloat y0 = body->p.y + (_x * sina + _y * cosa);

  const cpFloat ba = body->a + _a;
  const cpFloat x1 = x0 + ::cos(ba) * 10000.f;
  const cpFloat y1 = y0 + ::sin(ba) * 10000.f;

  // instanciate ray functor
  ray_functor_t rf(body, x0, y0, x1, y1, _h);

  // foreach object, test ray intersection
  cpSpaceHashEach(space->activeShapes, ray_functor::static_func, &rf);
  cpSpaceHashEach(space->staticShapes, ray_functor::static_func, &rf);

  // commit so the other thread see the result
  if (rf._res != std::numeric_limits<cpFloat>::max())
    _dist = (unsigned int)rf._res;
  else
    _dist = std::numeric_limits<unsigned int>::max();

  __sync_synchronize();
  _has_updated = true;
}


unsigned int sensor::read()
{
  _has_updated = false;
  _is_sensing = true;
  __sync_synchronize();

  while (_has_updated == false)
    pthread_yield();

  _is_sensing = false;

  __sync_synchronize();

  // post processing, refer to sharp documentation
  const unsigned int dist = _dist;
  return dist < 30 ? (unsigned int)-1 : (dist > 800 ? 800 : dist);
}
