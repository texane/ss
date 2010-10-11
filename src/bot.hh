#ifndef BOT_HH_INCLUDED
# define BOT_HH_INCLUDED


#include "ticker.hh"
#include "asserv.hh"
#include "sensor.hh"
#include "clamp.hh"
#include "conf.hh"
#include "physics.hh"


class bot
{
private:
  // physics
  cpSpace* _space;
  cpBody* _body;
  cpPolyShape* _shape;

  // devices
  asserv _asserv;
  sensor _sharps[3];
  ticker _ticker;
  clamp _clamp;

  // instances
  static bot _red_bot;
  static bot _blue_bot;

  // threading
#define THREAD_STATUS_WAIT 0L
#define THREAD_STATUS_RUN 1L
#define THREAD_STATUS_DONE 2L
  volatile long _status __attribute__((aligned));
  pthread_t _thread;

  // is the bot valid
  bool is_valid() const;

  // thread entry
  static void* static_entry(void*);

  // constructor
  bot();

  // strategies
  void wandering_strategy();
  void debug_strategy();

public:
  // physics
  void set_physics(cpSpace*, cpBody*, cpPolyShape*);
  void update_physics();

  // is the red bot
  bool is_red() const;

  // instances accessor
  static bot* get_bot_by_type(enum conf::object::object_type);

  // bot subsystem
  static int create_bots(const conf&);
  static void destroy_bots();
  static void start_bots();
};


void create_bots(const conf&);
void delete_bots();
void start_bots();
bool is_red_bot(const void*);
void* get_bot_context(bool);
bot* get_by_type(enum conf::object::object_type);
void set_bot_physics(bool, struct cpBody*, struct cpPolyShape*);
void update_bot_velocity(struct cpBody*);


#endif // ! BOT_HH_INCLUDED
