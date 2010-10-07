//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Tue Oct  5 22:33:27 2010 texane
// Last update Thu Oct  7 21:12:40 2010 texane
//


#include <list>
#include <iterator>
#include <pthread.h>
#include "asserv.hh"
#include "physics.hh"
#include "conf.hh"
#include "bot.hh"
#include "rtod.hh"


using std::list;


// internal bot implementation
class bot
{
public:

  // physics
  cpBody* _body;
  cpPolyShape* _shape;

  // threading
  pthread_t _thread;

  // bot info
  bool _is_valid;
  bool _is_red;
  asserv _asserv;

#define BOT_STATUS_WAIT 0
#define BOT_STATUS_RUN 1
#define BOT_STATUS_DONE 2
  volatile long _status  __attribute__((aligned));

  bot() : _body(NULL), _shape(NULL), _is_valid(false) {}

  void set_physics(cpBody* body, cpPolyShape* shape)
  { 
    _body = body;
    _shape = shape;

    _asserv.set_position((int)body->p.x, (int)body->p.y);
    _asserv.set_angle((int)rtod(body->a));
  }

  bool has_physics(const cpBody* body) const
  {
    return body == _body;
  }

  void update_velocity(cpBody* body)
  {
    _asserv.next(body);
    // _grabber.next(body);
  }

  void entry()
  {
    if (_is_red == false) return ;

    _asserv.set_velocity(400);

    _asserv.move_forward(800);
    _asserv.wait_done();

    for (size_t i = 0; i < 4; ++i)
    {
      _asserv.move_forward(400);
      _asserv.wait_done();

      _asserv.turn(90);
      _asserv.wait_done();
    }
  }

  static void* static_entry(void* arg)
  {
    bot* const b = static_cast<bot*>(arg);

    while (b->_status == BOT_STATUS_WAIT) 
      ;

    if (b->_status == BOT_STATUS_RUN)
      b->entry();

    return NULL;
  }
};


// create bots from conf

static bot blue_bot;
static bot red_bot;

static inline bot& get_bot(enum conf::object::object_type t)
{
  return (t == conf::object::OBJECT_TYPE_BLUE_BOT ? blue_bot : red_bot);
}

static inline bool is_bot(enum conf::object::object_type t)
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
    if (is_bot(pos->_type) == false)
      continue ;

    bot& b = get_bot(pos->_type);

    b._is_valid = true;
    b._is_red = (pos->_type == conf::object::OBJECT_TYPE_RED_BOT);

    // configure asserv
    b._asserv.set_angle(pos->_a);

    // create bot thread
    b._status = BOT_STATUS_WAIT;
    pthread_create(&b._thread, NULL, bot::static_entry, (void*)&b);
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


void start_bots()
{
  if (red_bot._is_valid == true)
    red_bot._status = BOT_STATUS_RUN;

  if (blue_bot._is_valid == true)
    blue_bot._status = BOT_STATUS_RUN;
}


void update_bot_velocity(cpBody* body)
{
  // todo: use private user defined pointer
  bot& b = (red_bot.has_physics(body) == true ? red_bot : blue_bot);
  b.update_velocity(body);
}
