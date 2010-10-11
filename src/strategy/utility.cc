//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Oct 11 20:42:39 2010 texane
// Last update Mon Oct 11 20:46:08 2010 texane
//


#include <sys/types.h>
#include "sensor.hh"


// read sharps

static inline unsigned int min(unsigned int a, unsigned int b)
{
  return a < b ? a : b;
}

void read_sharps(sensor* sharps, unsigned int* values, size_t count)
{
  for (size_t i = 0; i < count; ++i)
    values[i] = sharps[i].read();
}

unsigned int get_min_sharp(sensor* sharps, size_t count)
{
  // assume 0 < count < 32
  unsigned int values[32];
  if (count > 32)
    count = 32;

  // return the min distance
  read_sharps(sharps, values, count);

  unsigned int res = values[0];
  for (size_t i = 1; i < count; ++i)
    if (values[i] < res)
      res = values[i];

  return res;
}
