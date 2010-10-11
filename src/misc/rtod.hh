//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Thu Oct  7 20:15:33 2010 texane
// Last update Mon Oct 11 21:14:30 2010 texane
//


#ifndef MISC_RTOD_HH_INCLUDED
# define MISC_RTOD_HH_INCLUDED


#include <math.h>


// radians to degrees
static inline double rtod(double r)
{
  return (180.f * r) / M_PI;
}


#endif // RTOD_HH_INCLUDED
