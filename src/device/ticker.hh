//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sun Oct 10 10:57:25 2010 texane
// Last update Fri Oct 15 17:31:04 2010 texane
//


#ifndef DEVICE_TICKER_HH_INCLUDED
# define DEVICE_TICKER_HH_INCLUDED


#include "misc/atomic.hh"


class ticker
{
private:
  unsigned int _ms_per_tick;
  atomic_int_t _tick_count;

public:
  void set_info(unsigned int);
  void update();
  void reset();
  unsigned int read();
  unsigned int get_msecs();
};


#endif // ! DEVICE_TICKER_HH_INCLUDED
