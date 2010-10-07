//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Tue Oct  5 22:18:42 2010 texane
// Last update Thu Oct  7 22:02:15 2010 texane
//


// chipmunk physics engine routines


#include <list>
#include <stdlib.h>
#include <chipmunk/chipmunk.h>
#include "bot.hh"
#include "x.hh"
#include "conf.hh"
#include "dtor.hh"


using std::list;


// initialize

static cpFloat wscale;
static cpFloat hscale;

static const struct x_color* red_color = NULL;
static const struct x_color* blue_color = NULL;
static const struct x_color* yellow_color = NULL;
static const struct x_color* lred_color = NULL;
static const struct x_color* lblue_color = NULL;
static const struct x_color* lgreen_color = NULL;
static const struct x_color* black_color = NULL;

static void init_stuff(const conf& conf)
{
  // scaling
  wscale = (cpFloat)(conf::_space_width + 1.f) / (cpFloat)x_get_width();
  hscale = (cpFloat)(conf::_space_height + 1.f) / (cpFloat)x_get_height();

  // colors
  static const unsigned char red_rgb[3] = {0xff, 0x00, 0x00};
  static const unsigned char yellow_rgb[3] = {0x80, 0x80, 0x00};
  static const unsigned char blue_rgb[3] = {0x00, 0x00, 0xff};
  static const unsigned char lred_rgb[3] = {0xff, 0x40, 0x40};
  static const unsigned char lblue_rgb[3] = {0x40, 0x40, 0xff};
  static const unsigned char lgreen_rgb[3] = {0x40, 0xff, 0x40};
  static const unsigned char black_rgb[3] = {0x00, 0x00, 0x00};

  x_alloc_color(red_rgb, &red_color);
  x_alloc_color(yellow_rgb, &yellow_color);
  x_alloc_color(blue_rgb, &blue_color);
  x_alloc_color(lred_rgb, &lred_color);
  x_alloc_color(lgreen_rgb, &lgreen_color);
  x_alloc_color(lblue_rgb, &lblue_color);
  x_alloc_color(black_rgb, &black_color);
}


// world to view translation

static inline void space_to_view
(cpSegmentShape* shape, cpVect& a, cpVect& b)
{
  a.x = shape->ta.x / wscale;
  a.y = shape->ta.y / hscale;

  b.x = shape->tb.x / wscale;
  b.y = shape->tb.y / hscale;
}

static inline void space_to_view
(cpCircleShape* shape, cpVect& pos, cpFloat& radius)
{
  const cpFloat scale = (wscale + hscale) / 2.f;

  pos.x = shape->tc.x / wscale;
  pos.y = shape->tc.y / hscale;

  radius = shape->r / scale;
}

static inline void space_to_view
(cpPolyShape* shape, cpVect verts[4])
{
  for (size_t i = 0; i < 4; ++i)
  {
    verts[i].x = shape->tVerts[i].x / wscale;
    verts[i].y = shape->tVerts[i].y / hscale;
  }
}


// shape drawing routines

static void __attribute__((unused)) draw_shape
(cpBody* body, cpSegmentShape* shape, cpSpace* space)
{
  // translate from space to view 
  cpVect a, b;
  space_to_view(shape, a, b);

  // draw the line
  x_draw_line((int)a.x, (int)a.y, (int)b.x, (int)b.y, red_color);
}


static void draw_shape
(cpBody* body, cpCircleShape* shape, cpSpace* space)
{
  // translate from space to view 
  cpVect pos;
  double radius;
  space_to_view(shape, pos, radius);

  // draw the circle
  x_draw_circle((int)pos.x, (int)pos.y, (int)radius, yellow_color);
}


static void draw_shape
(cpBody* body, cpPolyShape* shape, cpSpace* space)
{
  // translate from space to view 
  cpVect verts[4];
  space_to_view(shape, verts);

  int x0 = (int)verts[3].x;
  int y0 = (int)verts[3].y;

  // draw the circle
  for (size_t i = 0; i < 4; ++i)
  {
    const int x1 = (int)verts[i].x;
    const int y1 = (int)verts[i].y;

    x_draw_line(x0, y0, x1, y1, blue_color);

    x0 = x1;
    y0 = y1;
  }
}


static void draw_object(cpShape* shape, cpSpace* space)
{
  cpBody* const body = shape->body;

  switch (shape->klass->type)
  {
  case CP_CIRCLE_SHAPE:
    draw_shape(body, (cpCircleShape*)shape, space);
    break;

  case CP_POLY_SHAPE:
    draw_shape(body, (cpPolyShape*)shape, space);
    break;

#if 0
  case CP_SEGMENT_SHAPE:
    draw_shape(body, (cpSegmentShape*)shape, space);
    break;
#endif

  default:
    break;
  }
}


// pawn velocity function

static void pawn_velocity_func
(cpBody* body, cpVect gravity, cpFloat damping, cpFloat dt)
{
  cpBodyUpdateVelocity(body, gravity, damping, dt);

  // ground friction
  body->v.x *= 0.9;
  body->v.y *= 0.9;
}


// bot velocity function

static void bot_velocity_func
(cpBody* body, cpVect gravity, cpFloat damping, cpFloat dt)
{
  cpBodyUpdateVelocity(body, gravity, damping, dt);
  update_bot_velocity(body);
}


// background drawing

static void fill_rectangle
(int x, int y, int w, int h, const struct x_color* c)
{
  // scale values
  x /= wscale;
  y /= hscale;
  w /= wscale;
  h /= hscale;

  // draw rectangle
  for (int xx = x; xx < x + w; ++xx)
    for (int yy = y; yy < y + h; ++yy)
      x_draw_pixel(xx, yy, c);
}

static void draw_background()
{
  // player areas
  fill_rectangle(0, 0, 400, 400, lred_color);
  fill_rectangle(2600, 0, 400, 400, lblue_color);

  // distribution areas
  fill_rectangle(0, 400, 400, 2700, lgreen_color);
  fill_rectangle(2600, 400, 400, 2700, lgreen_color);

  // bands
  fill_rectangle(400, 0, 50, 2100, black_color);
  fill_rectangle(2550, 0, 50, 2100, black_color);

  // tiles
  const struct x_color* tile_color = lblue_color;
  for (int i = 0; i < 6; ++i)
  {
    const int y = i * 350;
    for (int j = 0; j < 6; ++j)
    {
      const int x = 450 + j * 350;
      fill_rectangle(x, y, 350, 350, tile_color);
      tile_color = (tile_color == lblue_color ? lred_color : lblue_color);
    }
    tile_color = (tile_color == lblue_color ? lred_color : lblue_color);
  }

  // reserved areas
  fill_rectangle(450, 1980, 700, 120, black_color);
  fill_rectangle(1850, 1980, 700, 120, black_color);
}


// exported

void draw_space(cpSpace* space)
{
  draw_background();

  // iterate over static and active shapes
  cpSpaceHashEach(space->activeShapes, (cpSpaceHashIterator)draw_object, space);
  cpSpaceHashEach(space->staticShapes, (cpSpaceHashIterator)draw_object, space);
}


cpSpace* create_space(conf& conf)
{
  init_stuff(conf);

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
	cpShape* const shape = cpSegmentShapeNew(body, a, b, 1.f);
	shape->e = 1.f; // elasticity
	shape->u = 1.f; // friction
	cpSpaceAddStaticShape(space, shape);

	break;
      }

    case conf::object::OBJECT_TYPE_BLUE_BOT:
    case conf::object::OBJECT_TYPE_RED_BOT:
      {
	const cpFloat mass = 6000.f; // mass in grams

	const cpFloat moment = cpMomentForBox(pos->_w, pos->_w, mass);
	cpBody* const body = cpBodyNew(mass, moment);
	body->velocity_func = bot_velocity_func;
	body->p = cpv(pos->_x, pos->_y); // position
	body->v = cpvzero;
	body->a = dtor(pos->_a);
	cpSpaceAddBody(space, body);

	// shape
	cpShape* const shape = cpBoxShapeNew(body, pos->_w, pos->_w);
	shape->e = 1.f; // elasticity
	shape->u = 1.f; // friction
	cpSpaceAddShape(space, shape);

	// set the bot physics
	bool is_red = true;
	if (pos->_type == conf::object::OBJECT_TYPE_BLUE_BOT)
	  is_red = false;
	set_bot_physics(is_red, body, (cpPolyShape*)shape);

	break;
      }

    case conf::object::OBJECT_TYPE_PAWN:
    case conf::object::OBJECT_TYPE_KING:
    case conf::object::OBJECT_TYPE_QUEEN:
      {
	cpFloat mass = 700.f; // mass in grams
	if (pos->_type == conf::object::OBJECT_TYPE_PAWN)
	  mass = 500.f;

	// solid circles have inner diameters of 0
	const cpFloat moment = cpMomentForCircle(mass, 0.0f, pos->_w * 2.f, cpvzero);
	cpBody* const body = cpBodyNew(mass, moment);
	body->velocity_func = pawn_velocity_func;
	body->p = cpv(pos->_x, pos->_y); // position
	cpSpaceAddBody(space, body);

	// shape
	cpVect offset = cpv(0.f, 0.f);
	cpShape* const shape = cpCircleShapeNew(body, pos->_w, offset);
	shape->e = 1.f; // elasticity
	shape->u = 1.f; // friction
	cpSpaceAddShape(space, shape);

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
