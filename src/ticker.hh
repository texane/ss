//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sun Oct 10 10:57:25 2010 texane
// Last update Sun Oct 10 11:20:09 2010 texane
//


#ifndef TICKER_HH_INCLUDED
# define TICKER_HH_INCLUDED


#include "atomic.hh"


class ticker
{
private:
  unsigned int _ms_per_tick;
  atomic_int_t _tick_count;

public:
  void set_info(unsigned int);
  void update();
  void reset();
  unsigned int get_msecs();
};


#endif // ! TICKER_HH_INCLUDED
