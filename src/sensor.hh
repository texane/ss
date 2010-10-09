//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sat Oct  9 08:13:10 2010 texane
// Last update Sat Oct  9 14:40:25 2010 texane
//


#ifndef SENSOR_HH_INCLUDED
# define SENSOR_HH_INCLUDED


#include <pthread.h>
#include "physics.hh"


class sensor
{
  // implement a distance sensor

private:
  // rel to bot pos
  int _x;
  int _y;

  // in radians (arg in degrees)
  cpFloat _a;

  // absolute height
  int _h;

#if 0 // unused
  // phyisics model
  int _aperture;
  int _amplitude;
  int _power;
#endif

  // concurrency between physics update and sensing
  volatile unsigned char _is_sensing;
  volatile unsigned char _has_updated;
  volatile unsigned int _dist __attribute__((aligned));

public:
  sensor();

  void set_info(int, int, int, int);

  void update(cpSpace*, cpBody*);
  unsigned int sense();
};


#endif // ! SENSOR_HH_INCLUDED
