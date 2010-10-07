//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Tue Oct  5 22:23:19 2010 texane
// Last update Tue Oct  5 23:58:42 2010 texane
//


#ifndef CHIPMUNK_HH_INCLUDED
# define CHIPMUNK_HH_INCLUDED


#include <chipmunk/chipmunk.h>


class conf;


#define CONFIG_TICK_MS 40


void next_space(cpSpace*);
void draw_space(cpSpace*);
void destroy_space(cpSpace*);
cpSpace* create_space(conf&);


#endif // CHIPMUNK_HH_INCLUDED