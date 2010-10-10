//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sun Oct 10 12:59:38 2010 texane
// Last update Sun Oct 10 14:39:53 2010 texane
//


#ifndef CLAMP_HH_INCLUDED
# define CLAMP_HH_INCLUDED


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

  // conccurent grabbing protocol
  volatile bool _is_grabbing __attribute__((aligned));
  volatile bool _has_updated __attribute__((aligned));
  volatile bool _has_grabbed __attribute__((aligned));

public:
  clamp();
  void set_info(double, double, double, double, double);
  void update(cpSpace*, cpBody*);
  bool grab();

  friend struct grab_functor;
};


#endif // ! CLAMP_HH_INCLUDED
