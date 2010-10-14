//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Oct 11 19:55:15 2010 texane
// Last update Thu Oct 14 20:11:58 2010 texane
//


#include <string>
#include "strategy/strategy.hh"


strategy* strategy::factory(const std::string& name)
{
  if (name == std::string("wander"))
    return new wander();
  if (name == std::string("tiler"))
    return new tiler();
  if (name == std::string("distri"))
    return new distri();
  else if (name == std::string("test"))
    return new test();

  // default to bsod
  return new bsod();
}
