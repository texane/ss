//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Thu Oct  7 20:15:33 2010 texane
// Last update Mon Oct 11 21:14:38 2010 texane
//


#ifndef MISC_DTOR_HH_INCLUDED
# define MISC_DTOR_HH_INCLUDED


#include <math.h>


// degree to radians
static inline double dtor(double d)
{
  return (M_PI * d) / 180.f;
}


#endif // DTOR_HH_INCLUDED
