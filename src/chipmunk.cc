//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Tue Oct  5 22:18:42 2010 texane
// Last update Tue Oct  5 23:19:34 2010 texane
//


// chipmunk physics engine routines


#include <list>
#include <chipmunk/chipmunk.h>
#include "bot.hh"
#include "x.hh"
#include "conf.hh"


using std::list;


static inline void space_to_view
(cpSegmentShape* shape, cpVect& a, cpVect& b)
{
  const cpFloat wscale = (cpFloat)conf::_space_width / (cpFloat)x_get_width();
  const cpFloat hscale = (cpFloat)conf::_space_height / (cpFloat)x_get_height();

  a.x = shape->ta.x / wscale;
  a.y = shape->ta.y / hscale;

  b.x = shape->tb.x / wscale;
  b.y = shape->tb.y / hscale;
}


static inline void space_to_view
(cpCircleShape* shape, cpVect& pos, cpFloat& radius)
{
  const cpFloat wscale = (cpFloat)conf::_space_width / (cpFloat)x_get_width();
  const cpFloat hscale = (cpFloat)conf::_space_height / (cpFloat)x_get_height();
  const cpFloat scale = (wscale + hscale) / 2.f;

  pos.x = shape->tc.x / wscale;
  pos.y = shape->tc.y / hscale;

  radius = shape->r / scale;
}


static void draw_segment_shape
(cpBody* body, cpSegmentShape* shape, cpSpace* space)
{
  // allocate segment color
  static const struct x_color* red_color = NULL;
  static const unsigned char red_rgb[3] = {0xff, 0, 0};
  if (red_color == NULL)
    x_alloc_color(red_rgb, &red_color);

  // translate from space to view 
  cpVect a, b;
  space_to_view(shape, a, b);

  // draw the line
  x_draw_line((int)a.x, (int)a.y, (int)b.x, (int)b.y, red_color);
}


static void draw_circle_shape
(cpBody* body, cpCircleShape* shape, cpSpace* space)
{
  // allocate circle color
  static const struct x_color* blue_color = NULL;
  static const unsigned char blue_rgb[3] = {0, 0, 0xff};
  if (blue_color == NULL)
    x_alloc_color(blue_rgb, &blue_color);

  // translate from space to view 
  cpVect pos;
  double radius;
  space_to_view(shape, pos, radius);

  // draw the circle
  x_draw_circle((int)pos.x, (int)pos.y, (int)radius, blue_color);
}


static void draw_object(cpShape* shape, cpSpace* space)
{
  cpBody* const body = shape->body;

  switch (shape->klass->type)
  {
  case CP_CIRCLE_SHAPE:
    draw_circle_shape(body, (cpCircleShape*)shape, space);
    break;

  case CP_SEGMENT_SHAPE:
    draw_segment_shape(body, (cpSegmentShape*)shape, space);
    break;

  case CP_POLY_SHAPE:
    // draw_poly_shape(body, (cpPolyShape*)shape, space);
    break;

  default:
    break;
  }
}


// exported

void draw_space(cpSpace* space)
{
  // iterate over static and active shapes
  cpSpaceHashEach(space->activeShapes, (cpSpaceHashIterator)draw_object, space);
  cpSpaceHashEach(space->staticShapes, (cpSpaceHashIterator)draw_object, space);
}


cpSpace* create_space(conf& conf)
{
  cpInitChipmunk();

  cpResetShapeIdCounter();

  // instanciate a space, basic unit of physical interaction
  cpSpace* const space = cpSpaceNew();
  if (space == NULL)
    return NULL;

#if 0
  space->iterations = 10; // iterative solver iteration count
  space->elasticIterations = 0.f; // idem but for elastic collisions
#endif
  space->gravity = cpv(0.f, 0.f);

  // static (optimized non movable), active counts
#define AVERAGE_SIZE 40.f
  cpSpaceResizeStaticHash(space, AVERAGE_SIZE, conf._static_count);
  cpSpaceResizeActiveHash(space, AVERAGE_SIZE, conf._active_count);

  // add objects
  list<conf::object_t>::const_iterator pos = conf._objects.begin();
  list<conf::object_t>::const_iterator end = conf._objects.end();
  for (; pos != end; ++pos)
  {
    switch (pos->_type)
    {
    case conf::object::OBJECT_TYPE_WALL:
      {
	// body
	cpBody* const body = cpBodyNew(INFINITY, INFINITY);
	body->p = cpv(pos->_x, pos->_y); // position

	// shape
	cpVect a = cpv(-pos->_w / 2, -pos->_h / 2);
	cpVect b = cpv(+pos->_w / 2, +pos->_h / 2);
	cpShape* const shape = cpSegmentShapeNew(body, a, b, 10.f);
	shape->e = 1.f; // elasticity
	shape->u = 1.f; // friction
	cpSpaceAddStaticShape(space, shape);

	break;
      }

    case conf::object::OBJECT_TYPE_BLUE_BOT:
    case conf::object::OBJECT_TYPE_RED_BOT:
      {
	// body

	cpFloat mass = 6000.f; // mass in grams
	if (pos->_type == conf::object::OBJECT_TYPE_RED_BOT)
	  mass = 1000.f;

	const cpFloat moment = cpMomentForCircle(mass, 0.0f, pos->_w, cpvzero);
	cpBody* const body = cpBodyNew(mass, moment);
	body->p = cpv(pos->_x, pos->_y); // position
	body->v = cpv(0.f, 100.f);
	cpSpaceAddBody(space, body);

	// shape
	cpVect offset = cpv(0.f, 0.f);
	cpShape* const shape = cpCircleShapeNew(body, pos->_w, offset);
	shape->e = 1.f; // elasticity
	shape->u = 1.f; // friction
	cpSpaceAddShape(space, shape);

	// set the bot physics
	bool is_red = true;
	if (pos->_type == conf::object::OBJECT_TYPE_BLUE_BOT)
	  is_red = false;
	set_bot_physics(is_red, shape);

	break;
      }

    default:
      break;
    }

  }

  return space;
}


void destroy_space(cpSpace* space)
{
  cpSpaceFreeChildren(space);
  cpSpaceFree(space);
}


void next_space(cpSpace* space)
{
  // this routine is called every 40ms

  static const cpFloat dt = 0.01;
  for (size_t i = 0; i < 4; ++i)
    cpSpaceStep(space, dt);
}
