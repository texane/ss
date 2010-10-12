//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Oct 11 20:41:15 2010 texane
// Last update Tue Oct 12 10:10:48 2010 texane
//


#ifndef STRATEGY_UTILITY_HH_INCLUDED
# define STRATEGY_UTILITY_HH_INCLUDED


#include <sys/types.h>


class sensor;

void read_sharps(sensor*, unsigned int*, size_t);
unsigned int get_min_sharp(sensor*, size_t);


#endif // STRATEGY_UTILITY_HH_INCLUDED
