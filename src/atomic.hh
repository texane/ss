//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Wed Oct  6 23:15:48 2010 texane
// Last update Wed Oct  6 23:35:26 2010 texane
//


#ifndef ATOMIC_HH_INCLUDED
# define ATOMIC_HH_INCLUDED


typedef struct atomic_int
{
  volatile int _value;

  atomic_int(int value = 0) : _value(value) {}

  void write(int value) { _value = value; }
  int read() const { return _value; }

} atomic_int_t;


#endif // ATOMIC_HH_INCLUDED
