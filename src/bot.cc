//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Tue Oct  5 22:33:27 2010 texane
// Last update Sun Oct 10 17:20:37 2010 texane
//


#include <list>
#include <iterator>
#include <algorithm>
#include <stdlib.h>
#include <pthread.h>
#include "asserv.hh"
#include "physics.hh"
#include "ticker.hh"
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
  _ticker.update();

  _asserv.update(_body);

  const size_t count = sizeof(_sharps) / sizeof(_sharps[0]);
  for (size_t i = 0; i < count; ++i)
    _sharps[i].update(_space, _body);

  _clamp.update(_space, _body);
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

    // configure ticker
    b->_ticker.set_info(10);

    // configure sharps
    b->_sharps[0].set_info(pos->_w / 2, -pos->_w / 2, 0, 10);
    b->_sharps[1].set_info(pos->_w / 2, 0, 0, 10);
    b->_sharps[2].set_info(pos->_w / 2, pos->_w / 2, 0, 10);

    // configure clamp
    b->_clamp.set_info(pos->_w / 2, 0, 300, 150, 0);

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
    b->debug_strategy();
  b->_status = THREAD_STATUS_DONE;

  return NULL;
}


// device wrappers

unsigned int bot::do_sharps()
{
  // return the min distance

  const size_t count = sizeof(_sharps) / sizeof(_sharps[0]);

  unsigned int dists[count];

  for (size_t i = 0; i < count; ++i)
    dists[i] = _sharps[i].sense();

  return std::min(dists[0], std::min(dists[1], dists[2]));
}
