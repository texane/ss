//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Tue Oct  5 22:33:27 2010 texane
// Last update Wed Oct  6 21:12:34 2010 texane
//


#include <list>
#include <iterator>
#include <math.h>
#include "physics.hh"
#include "conf.hh"
#include "bot.hh"


using std::list;


// internal bot implementation
class bot
{
  // asserv _asserv;
  cpBody* _body;
  cpPolyShape* _shape;

public:
  bot() : _body(NULL), _shape(NULL) {}

  void set_physics(cpBody* body, cpPolyShape* shape)
  { 
    _body = body;
    _shape = shape;
  }

  bool has_physics(const cpBody* body) const
  {
    return body == _body;
  }

  void update_velocity(cpBody* body, cpFloat dt)
  {
    const cpFloat asserv_x = 1500.f;
    const cpFloat asserv_y = 1050.f;
    const cpFloat asserv_v = 400.f;

    // simulate asservissement
    const cpFloat dx = asserv_x - body->p.x;
    const cpFloat dy = asserv_y - body->p.y;
    const cpFloat d = ::sqrt(dx * dx + dy * dy);

    body->v.x = 0.f;
    if (::fabs(dx) > 20.f)
      body->v.x = (dx * d) / asserv_v;

    body->v.y = 0.f;
    if (::fabs(dy) > 20.f)
      body->v.y = (dy * d) / asserv_v;
  }

  void next()
  {
#if 0
    cpFloat mul = 1.f;
    if (_shape->shape.body->v.x > 0.f)
    {
      if (_shape->tc.x == 1000.f)
	mul = -1.f;
    }
    else
    {
      if (_shape->tc.x == 1000.f)
	mul = -1.f;
    }

    _shape->shape.body->v.x *= mul;
#endif
  }
};


// global bot list
static bot blue_bot;
static bot red_bot;


// exported

static inline bool is_bot_type(enum conf::object::object_type t)
{
  return
    (t == conf::object::OBJECT_TYPE_BLUE_BOT) ||
    (t == conf::object::OBJECT_TYPE_RED_BOT);
}

void create_bots(const conf& conf)
{
  list<conf::object_t>::const_iterator pos = conf._objects.begin();
  list<conf::object_t>::const_iterator end = conf._objects.end();

  for (; pos != end; ++pos)
  {
    if (is_bot_type(pos->_type) == false)
      continue ;
  }
}


void delete_bots()
{
}


void set_bot_physics(bool is_red, cpBody* body, cpPolyShape* shape)
{
  // set the bot physics information
  // shape needed when dealing with chipmunk

  bot& b = (is_red == true ? red_bot : blue_bot);
  b.set_physics(body, shape);
}


void schedule_bots()
{
  red_bot.next();
  blue_bot.next();
}


void update_bot_velocity(cpBody* body, double dt)
{
  // todo: use private user defined pointer
  bot& b = (red_bot.has_physics(body) == true ? red_bot : blue_bot);
  b.update_velocity(body, dt);
}
