//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Tue Oct  5 22:23:19 2010 texane
// Last update Tue Oct  5 22:25:40 2010 texane
//


#ifndef CHIPMUNK_HH_INCLUDED
# define CHIPMUNK_HH_INCLUDED


#include <chipmunk/chipmunk.h>


#define CONFIG_TICK_MS 40


extern void next_space(cpSpace*);
extern void draw_space(cpSpace*);
extern void destroy_space(cpSpace*);
extern cpSpace* create_space(conf&);


#endif // CHIPMUNK_HH_INCLUDED
