//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Tue Oct  5 22:23:19 2010 texane
// Last update Mon Oct 11 21:25:35 2010 texane
//


#ifndef PHYSICS_PHYSICS_HH_INCLUDED
# define PHYSICS_PHYSICS_HH_INCLUDED


#include <chipmunk/chipmunk.h>


class conf;


#define CONFIG_TICK_MS 40


void update_space(cpSpace*);
void draw_space(cpSpace*);
void destroy_space(cpSpace*);
cpSpace* create_space(conf&);
void remove_shape(cpSpace*, cpShape*);
void insert_shape(cpSpace*, cpShape*, double, double);


#endif // PHYSICS_PHYSICS_HH_INCLUDED
