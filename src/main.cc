//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Oct  4 19:53:39 2010 texane
// Last update Mon Oct  4 22:49:16 2010 texane
//


#include <stdio.h>
#include <stdlib.h>
#include "x.hh"
#include "conf.hh"


// redraw map

#include <list>
#include <chipmunk/chipmunk.h>


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


static void draw_object(cpShape* shape, cpSpace* space)
{
  cpBody* const body = shape->body;

  switch (shape->klass->type)
  {
  case CP_CIRCLE_SHAPE:
    // draw_circle_shape(body, (cpCircleShape*)shape, space);
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


static void draw_space(cpSpace* space)
{
  // iterate over static and active shapes
  cpSpaceHashEach(space->activeShapes, (cpSpaceHashIterator)draw_object, space);
  cpSpaceHashEach(space->staticShapes, (cpSpaceHashIterator)draw_object, space);
}


static cpSpace* create_space(conf& conf)
{
  cpInitChipmunk();

  cpResetShapeIdCounter();

  cpSpace* const space = cpSpaceNew();
  if (space == NULL)
    return NULL;

  cpSpaceResizeStaticHash(space, 40.f, conf._static_count);
  cpSpaceResizeActiveHash(space, 40.f, conf._active_count);

  // add walls
  list<conf::object_t>::const_iterator pos = conf._objects.begin();
  list<conf::object_t>::const_iterator end = conf._objects.end();
  for (; pos != end; ++pos)
  {
    if (pos->_type != conf::object::OBJECT_TYPE_WALL)
      continue ;

    // body

    cpBody* const body = cpBodyNew(INFINITY, INFINITY);
    body->p = cpv(pos->_x, pos->_y); // position
    cpSpaceAddBody(space, body);

    // shape

    cpVect a = cpv(-pos->_w / 2, -pos->_h / 2);
    cpVect b = cpv(+pos->_w / 2, +pos->_h / 2);

    cpShape* const shape = cpSegmentShapeNew(body, a, b, 10.f);
    shape->e = 1.f; // elasticity
    shape->u = 1.f; // friction

    cpSpaceAddShape(space, shape);
  }

  return space;
}


static void destroy_space(cpSpace* space)
{
  cpSpaceFreeChildren(space);
  cpSpaceFree(space);
}


// event handlers

static int on_event(const struct x_event* ev, void* arg)
{
  switch (x_event_get_type(ev))
  {
  case X_EVENT_TICK:
    // sim::next();
    // view::update();
    draw_space((cpSpace*)arg);
    break;

  case X_EVENT_QUIT:
    x_cleanup();
    ::exit(-1);
    break;

  default:
    break;
  }

  return 0;
}


// main

int main(int ac, char** av)
{
  conf conf;

  if (conf.load(av[1]) == -1)
  {
    printf("invalid conf\n");
    return -1;
  }

  if (x_initialize(40) == -1)
    return -1;

  cpSpace* const space = create_space(conf);
  x_loop(on_event, (void*)space);

#if CONFIG_DEBUG
  conf.print();
#endif

  destroy_space(space);

  return 0;
}
