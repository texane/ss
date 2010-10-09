//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sat Oct  9 08:26:05 2010 texane
// Last update Sat Oct  9 15:26:54 2010 texane
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


typedef struct ray_info
{
  // y = mx + p
  cpFloat _m;
  cpFloat _p;

  void compute_line_eq
  (cpFloat x0, cpFloat y0, cpFloat x1, cpFloat y1)
  {
    // compute m and p line coeffs given 2 points

    // special case, vertical line
    if (::fabs(x0 - x1) < 0.0001f)
    {
      _m = std::numeric_limits<cpFloat>::max();
      _p = 0.f;
      return ;
    }

    const cpFloat dx = x1 - x0;
    const cpFloat dy = y1 - y0;

    _m = dy / dx;

    if (::fabs(_m) > 0.0001f)
    {
      if (::fabs(x0) > 0.0001f)
	_p = y0 / (_m * x0);
      else
	_p = y1 / (_m * x1);
    }
    else
    {
      _p = y0;
    }

    // printf("-- y = %lfx + %lf\n", _m, _p);
  }

} ray_info_t;


typedef struct solver_info
{
  ray_info_t _ri;
  cpFloat _res;

  solver_info()
  {
    // min distance reached
    _res = std::numeric_limits<cpFloat>::max();
  }

} solver_info_t;


static cpFloat solve_ray_segment
(const ray_info_t& r0, const ray_info_t& r1, cpFloat x0, cpFloat x1)
{
  // x0 x1 the segment limits, unordered

  // test for parallel lines
  if (::fabs(r0._m - r1._m) < 0.0001f)
    return std::numeric_limits<cpFloat>::max();

  // vertical line
  if (r0._m == std::numeric_limits<cpFloat>::max())
    return std::numeric_limits<cpFloat>::max();
  else if (r1._m == std::numeric_limits<cpFloat>::max())
    return std::numeric_limits<cpFloat>::max();

  const cpFloat x = (r1._p - r0._p) / (r0._m - r1._m);

  // handle out of segment
  if ((x < std::min(x0, x1)) || (x > std::max(x0, x1)))
    return std::numeric_limits<cpFloat>::max();

  return r0._m * x + r0._p;
}


static void solve_ray_shape(cpShape* shape, solver_info_t* si)
{
  switch (shape->klass->type)
  {
  case CP_CIRCLE_SHAPE:
    {
      // circle line intersection
      break;
    }

  case CP_POLY_SHAPE:
    {
      cpPolyShape* const ps = (cpPolyShape*)shape;

      cpFloat x0 = ps->tVerts[ps->numVerts - 1].x;
      cpFloat y0 = ps->tVerts[ps->numVerts - 1].y;

      for (int i = 0; i < ps->numVerts; ++i)
      {
	const cpFloat x1 = ps->tVerts[i].x;
	const cpFloat y1 = ps->tVerts[i].y;

	ray_info_t ri;
	ri.compute_line_eq(x0, y0, x1, y1);

	const cpFloat res = solve_ray_segment(si->_ri, ri, x0, x1);
	si->_res = std::min(si->_res, res);

	x0 = x1;
	y0 = y1;
      }

      break;
    }

  case CP_SEGMENT_SHAPE:
    {
      cpSegmentShape* const ss = (cpSegmentShape*)shape;

      ray_info_t ri;
      ri.compute_line_eq(ss->ta.x, ss->ta.y, ss->tb.x, ss->tb.y);

#if 0
      if ((ss->ta.x == 0) && (ss->ta.y != ss->tb.y))
      {
	::printf("ri=(%lf, %lf) :: si._ri(%lf, %lf)\n",
		 ri._m, ri._p, si->_ri._m, si->_ri._p);
      }
#endif

      const cpFloat res = solve_ray_segment(si->_ri, ri, ss->ta.x, ss->tb.x);
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

  // instanciate intersection solver
  solver_info_t si;

  // initialize sensor ray
  const cpFloat ta = body->a + _a;
  const cpFloat tx = body->p.x + _x;
  const cpFloat ty = body->p.y + _y;
  si._ri.compute_line_eq(tx, ty, tx + ::cos(ta), ty + ::sin(ta));

  // foreach object, test ray intersection
  cpSpaceHashEach(space->activeShapes, (cpSpaceHashIterator)solve_ray_shape, &si);
  cpSpaceHashEach(space->staticShapes, (cpSpaceHashIterator)solve_ray_shape, &si);

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
