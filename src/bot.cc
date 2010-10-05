//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Tue Oct  5 22:33:27 2010 texane
// Last update Tue Oct  5 22:46:08 2010 texane
//


#include <list>
#include <iterator>
#include "conf.hh"
#include "bot.hh"


using std::list;


// bot type
class bot
{
public:
  void next() {}
};


// global bot list
static list<bot*> bots;


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

    bots.push_back(new bot());
  }
}


void delete_bots()
{
  list<bot*>::iterator pos = bots.begin();
  list<bot*>::iterator end = bots.end();
  for (; pos != end; ++pos)
    delete *pos;
}


void schedule_bots()
{
  list<bot*>::iterator pos = bots.begin();
  list<bot*>::iterator end = bots.end();
  for (; pos != end; ++pos)
    (*pos)->next();
}
