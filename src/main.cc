//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Oct  4 19:53:39 2010 texane
// Last update Sun Oct 10 11:10:33 2010 texane
//


#include <stdio.h>
#include <stdlib.h>
#include "x.hh"
#include "conf.hh"
#include "physics.hh"
#include "bot.hh"


// event handlers

static int on_event(const struct x_event* ev, void* arg)
{
  switch (x_event_get_type(ev))
  {
  case X_EVENT_TICK:
    update_space((cpSpace*)arg);
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

  bot::create_bots(conf);

  // trigger every 40ms
  if (x_initialize(CONFIG_TICK_MS) == -1)
    return -1;

  // create the physics
  cpSpace* const space = create_space(conf);

  // start bots
  bot::start_bots();

  // loop until done
  x_loop(on_event, (void*)space);

#if CONFIG_DEBUG
  conf.print();
#endif

  bot::destroy_bots();

  destroy_space(space);

  return 0;
}
