#include <stdio.h>
#include "conf.hh"

// main

int main(int ac, char** av)
{
  conf conf;

  if (conf.load(av[1]) == -1)
    printf("invalid conf\n");

#if CONFIG_DEBUG
  conf.print();
#endif

  return 0;
}
