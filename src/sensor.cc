//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sat Oct  9 08:26:05 2010 texane
// Last update Sat Oct  9 22:37:45 2010 texane
//


#include <limits>
#include <algorithm>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include "physics.hh"
#include "sensor.hh"
#include "dtor.hh"
#include "rtod.hh"
#include "intersect.hh"


sensor::sensor()
{
}


void sensor::set_info(int x, int y, int a, int h)
{
  _x = x;
  _y = y;

  _a = dtor((double)a);

  _h = h;
}


typedef struct solver_info
{
  cpBody* _body;

  cpFloat _x0;
  cpFloat _y0;
  cpFloat _x1;
  cpFloat _y1;

  cpFloat _res;

  solver_info(cpBody* body, cpFloat x0, cpFloat y0, cpFloat x1, cpFloat y1)
    : _body(body), _x0(x0), _y0(y0), _x1(x1), _y1(y1)
  {
    // min distance reached
    _res = std::numeric_limits<cpFloat>::max();
  }

} solver_info_t;


static void solve_line_shape(cpShape* shape, solver_info_t* si)
{
  if (si->_body == shape->body)
    return ;

  switch (shape->klass->type)
  {
  case CP_CIRCLE_SHAPE:
    {
      cpCircleShape* const cs = (cpCircleShape*)shape;

      const cpFloat res = (cpFloat)intersect_circle
	(si->_x0, si->_y0, si->_x1, si->_y1, cs->tc.x, cs->tc.y, cs->r);
      si->_res = std::min(si->_res, res);

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
	  (si->_x0, si->_y0, si->_x1, si->_y1, x2, y2, x3, y3);
	si->_res = std::min(si->_res, res);

	x2 = x3;
	y2 = y3;
      }

      break;
    }

  case CP_SEGMENT_SHAPE:
    {
      cpSegmentShape* const ss = (cpSegmentShape*)shape;

      const cpFloat res = (cpFloat)intersect_segment
	(si->_x0, si->_y0, si->_x1, si->_y1,
	 ss->ta.x, ss->ta.y, ss->tb.x, ss->tb.y);

      si->_res = std::min(si->_res, res);

      break;
    }

  default: break;
  }
}


void sensor::update(cpSpace* space, cpBody* body)
{
  if (_is_sensing == 0)
    return ;

  // initialize sensor line
  const cpFloat a = body->a + _a;
  const cpFloat x0 = body->p.x + _x * ::cos(body->a);
  const cpFloat y0 = body->p.y + _y * ::sin(body->a);
  const cpFloat x1 = x0 + ::cos(a) * 10000.f;
  const cpFloat y1 = y0 + ::sin(a) * 10000.f;

  // instanciate intersection solver
  solver_info_t si(body, x0, y0, x1, y1);

  // foreach object, test ray intersection
  cpSpaceHashEach(space->activeShapes, (cpSpaceHashIterator)solve_line_shape, &si);
  cpSpaceHashEach(space->staticShapes, (cpSpaceHashIterator)solve_line_shape, &si);

  // commit so the other thread see the result
  if (si._res != std::numeric_limits<cpFloat>::max())
    _dist = (unsigned int)si._res;
  else
    _dist = std::numeric_limits<unsigned int>::max();
  __sync_synchronize();
  _has_updated = 1;
}


unsigned int sensor::sense()
{
  _has_updated = 0;
  __sync_synchronize();
  _is_sensing = 1;

  while (_has_updated == 0)
    pthread_yield();

  __sync_synchronize();
  _is_sensing = 0;

  return _dist;
}
