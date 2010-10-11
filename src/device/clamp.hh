//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sun Oct 10 12:59:38 2010 texane
// Last update Mon Oct 11 20:59:24 2010 texane
//


#ifndef DEVICE_CLAMP_HH_INCLUDED
# define DEVICE_CLAMP_HH_INCLUDED


#include "atomic.hh"


// forward type decl
struct grab_functor;


class clamp
{
  double _x;
  double _y;
  double _w;
  double _h;
  double _a;

  // current held object
  cpShape* _grabbed_shape;

  // conccurent grabbing protocol
  volatile bool _is_grabbing __attribute__((aligned));
  volatile bool _is_dropping __attribute__((aligned));
  volatile bool _has_updated __attribute__((aligned));
  volatile bool _has_grabbed __attribute__((aligned));

public:
  clamp();
  void set_info(double, double, double, double, double);
  unsigned int grabbing_distance() const;
  void update(cpSpace*, cpBody*);
  bool grab();
  void drop();

  friend struct grab_functor;
};


#endif // ! DEVICE_CLAMP_HH_INCLUDED
