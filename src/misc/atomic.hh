//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Wed Oct  6 23:15:48 2010 texane
// Last update Mon Oct 11 21:14:50 2010 texane
//


#ifndef MISC_ATOMIC_HH_INCLUDED
# define MISC_ATOMIC_HH_INCLUDED


typedef struct atomic_int
{
  volatile int _value;

  atomic_int(int value = 0) : _value(value) {}

  void write(int value) { _value = value; }
  int read() const { return _value; }

  void inc() { __sync_fetch_and_add(&_value, 1); }

} atomic_int_t;


#endif // ATOMIC_HH_INCLUDED
