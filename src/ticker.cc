//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sun Oct 10 11:03:43 2010 texane
// Last update Sun Oct 10 11:25:17 2010 texane
//


#include <stdio.h>
#include "ticker.hh"
#include "atomic.hh"


void ticker::set_info(unsigned int ms_per_tick)
{
  _ms_per_tick = ms_per_tick;
}


void ticker::update()
{
  _tick_count.inc();
}


unsigned int ticker::get_msecs()
{
  return (unsigned int)_tick_count.read() * _ms_per_tick;
}


void ticker::reset()
{
  _tick_count.write(0);
}
