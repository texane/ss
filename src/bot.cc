//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Tue Oct  5 22:33:27 2010 texane
// Last update Sat Oct  9 22:38:23 2010 texane
//


#include <list>
#include <iterator>
#include <stdlib.h>
#include <pthread.h>
#include "asserv.hh"
#include "physics.hh"
#include "conf.hh"
#include "bot.hh"
#include "rtod.hh"


using std::list;


// class instances
bot bot::_red_bot;
bot bot::_blue_bot;


// bot methods implementation

inline bot::bot() : _space(NULL), _body(NULL), _shape(NULL)
{}


inline bool bot::is_valid() const
{
  return _body != NULL;
}


bool bot::is_red() const
{
  return this == (void*)&_red_bot;
}


void bot::set_physics(cpSpace* space, cpBody* body, cpPolyShape* shape)
{ 
  _space = space;
  _body = body;
  _shape = shape;

  _asserv.set_position((int)body->p.x, (int)body->p.y);
  _asserv.set_angle((int)rtod(body->a));
}


void bot::update_physics()
{
  _asserv.update(_body);

  _lsharp.update(_space, _body);
  _hsharp.update(_space, _body);

  // _grabber.next(_body);
}


bot* bot::get_bot_by_type(enum conf::object::object_type type)
{
  if (type == conf::object::OBJECT_TYPE_RED_BOT)
    return &bot::_red_bot;
  return &bot::_blue_bot;
}


static inline bool is_bot(enum conf::object::object_type t)
{
  return (t == conf::object::OBJECT_TYPE_BLUE_BOT) ||
    (t == conf::object::OBJECT_TYPE_RED_BOT);
}


int bot::create_bots(const conf& conf)
{
  // instanciate bots from conf

  list<conf::object_t>::const_iterator pos = conf._objects.begin();
  list<conf::object_t>::const_iterator end = conf._objects.end();

  for (; pos != end; ++pos)
  {
    if (is_bot(pos->_type) == false)
      continue ;

    bot* const b = get_bot_by_type(pos->_type);

    // configure asserv
    b->_asserv.set_angle(pos->_a);

    // configure sharps
    b->_lsharp.set_info(pos->_w / 2, 0, 0, 25);
    b->_lsharp.set_info(pos->_w / 2, 0, 0, 10);

    // create bot thread
    b->_status = THREAD_STATUS_WAIT;
    pthread_create(&b->_thread, NULL, bot::static_entry, (void*)b);
  }

  return 0;
}


void bot::destroy_bots()
{
  if (_red_bot.is_valid())
  {
    _red_bot._status = THREAD_STATUS_DONE;
    pthread_join(_red_bot._thread, NULL);
  }

  if (_blue_bot.is_valid())
  {
    _blue_bot._status = THREAD_STATUS_DONE;
    pthread_join(_blue_bot._thread, NULL);
  }
}


void bot::start_bots()
{
  // rely on physics info to know if valid

  if (_red_bot.is_valid())
    _red_bot._status = THREAD_STATUS_RUN;

  if (_blue_bot.is_valid())
    _blue_bot._status = THREAD_STATUS_RUN;
}


void* bot::static_entry(void* arg)
{
  bot* const b = static_cast<bot*>(arg);

  while (b->_status == THREAD_STATUS_WAIT) 
    ;

  if (b->_status == THREAD_STATUS_RUN)
    b->wandering_strategy();
  b->_status = THREAD_STATUS_DONE;

  return NULL;
}
