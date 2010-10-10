//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sat Oct  9 08:13:10 2010 texane
// Last update Sun Oct 10 08:52:02 2010 texane
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
  double _x;
  double _y;

  // in radians (arg in degrees)
  double _a;

  // absolute height
  double _h;

#if 0 // unused
  // phyisics model
  double _aperture;
  double _amplitude;
  double _power;
#endif

  // concurrency between physics update and sensing
  volatile unsigned char _is_sensing;
  volatile unsigned char _has_updated;
  volatile unsigned int _dist __attribute__((aligned));

public:
  sensor();

  void set_info(double, double, double, double);

  void update(cpSpace*, cpBody*);
  unsigned int sense();
};


#endif // ! SENSOR_HH_INCLUDED
