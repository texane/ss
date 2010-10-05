//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Tue Oct  5 22:33:27 2010 texane
// Last update Tue Oct  5 23:17:37 2010 texane
//


#include <list>
#include <iterator>
#include <stdlib.h>
#include "conf.hh"
#include "bot.hh"


using std::list;


// fwd decls
struct cpShape;


// bot interface
class bot
{
public:
  virtual void next() = 0;
  virtual void set_physics(cpShape*) = 0;

  ~bot() {}
};


// default bot implementation
class default_bot : public bot
{
public:
  virtual void next() {}
  virtual void set_physics(cpShape*) {}
};


// global bot list
static bot* blue_bot = NULL;
static bot* red_bot = NULL;


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

    // assume bot of the given type not already instanciated
    bot* const b = new default_bot();
    if (pos->_type == conf::object::OBJECT_TYPE_BLUE_BOT)
      blue_bot = b;
    else
      red_bot = b;
  }
}


void delete_bots()
{
  if (red_bot != NULL)
    delete red_bot;

  if (blue_bot != NULL)
    delete blue_bot;

  red_bot = NULL;
  blue_bot = NULL;
}


void set_bot_physics(bool is_red, cpShape* shape)
{
  // set the bot physics information
  // shape needed when dealing with chipmunk

  // assume the bot is valid
  if (is_red == true)
    red_bot->set_physics(shape);
  else
    blue_bot->set_physics(shape);
}


void schedule_bots()
{
  if (red_bot != NULL)
    red_bot->next();

  if (blue_bot != NULL)
    blue_bot->next();
}
