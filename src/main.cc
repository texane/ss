//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Oct  4 19:53:39 2010 texane
// Last update Mon Oct  4 20:02:15 2010 texane
//


#include <stdio.h>
#include <stdlib.h>
#include "x.hh"
#include "conf.hh"


// event handlers

static int on_event(const struct x_event* ev, void*)
{
  switch (x_event_get_type(ev))
  {
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

  if (x_initialize(100) == -1)
  {
    return -1;
  }

  x_loop(on_event, NULL);

#if CONFIG_DEBUG
  conf.print();
#endif

  return 0;
}
