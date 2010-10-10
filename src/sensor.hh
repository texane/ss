//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sat Oct  9 08:13:10 2010 texane
// Last update Sun Oct 10 21:54:46 2010 texane
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
  volatile bool _is_sensing __attribute__((aligned));
  volatile bool _has_updated __attribute__((aligned));
  volatile unsigned int _dist __attribute__((aligned));

public:
  sensor();

  void set_info(double, double, double, double);

  void update(cpSpace*, cpBody*);
  unsigned int read();
};


#endif // ! SENSOR_HH_INCLUDED
