#ifndef DEVICE_REDBLU_HH_INCLUDED
# define DEVICE_REDBLU_HH_INCLUDED


#include "physics/physics.hh"


class redblu
{
  // redblu led implementation

private:
  // relative to bot pos
  double _x;
  double _y;
  double _h;

  // concurrency between physics update and sensing
  volatile bool _is_sensing __attribute__((aligned));
  volatile bool _has_updated __attribute__((aligned));
  volatile unsigned int _is_red __attribute__((aligned));

public:
  redblu();

  void update(cpSpace*, cpBody*);
  void set_info(double, double, double = 0.f);
  unsigned int read();
  
};


#endif // ! DEVICE_REDBLU_HH_INCLUDED
