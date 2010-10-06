//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Tue Oct  5 22:33:27 2010 texane
// Last update Wed Oct  6 17:38:25 2010 texane
//


#include <list>
#include <iterator>
#include <stdlib.h>
#include "physics.hh"
#include "conf.hh"
#include "bot.hh"


using std::list;


// internal bot implementation
class bot
{
  // asserv _asserv;
  cpPolyShape* _shape;

public:
  bot() : _shape(NULL) {}

  void set_physics(cpPolyShape* shape)
  { _shape = shape; }

  void on_collision()
  {}

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


void set_bot_physics(bool is_red, cpPolyShape* shape)
{
  // set the bot physics information
  // shape needed when dealing with chipmunk

  if (is_red == true)
    red_bot.set_physics(shape);
  else
    blue_bot.set_physics(shape);
}


void schedule_bots()
{
  red_bot.next();
  blue_bot.next();
}
