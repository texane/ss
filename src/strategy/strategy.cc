//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Oct 11 19:55:15 2010 texane
// Last update Mon Oct 11 20:49:22 2010 texane
//


#include <string>
#include "strategy/strategy.hh"


strategy* strategy::factory(const std::string& name)
{
  if (name == std::string("wander"))
    return new wander();
  else if (name == std::string("test"))
    return new test();

  // default to bsod
  return new bsod();
}
