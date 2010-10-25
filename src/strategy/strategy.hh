//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Oct 11 19:46:49 2010 texane
// Last update Thu Oct 14 20:11:45 2010 texane
//


#ifndef STRATEGY_STRATEGY_HH_INCLUDED
# define STRATEGY_STRATEGY_HH_INCLUDED


#include <string>


class bot;


class strategy
{
public:
  virtual ~strategy() {}

  // strategy interface
  virtual void main(bot&) = 0;

  // instanciate given a name
  static strategy* factory(const std::string&);
};


// declare a strategy. must be declared as friend
// in the bot class (refer to bot.hh) and add the
// necessary factory code in factory.cc.

#define DECLARE_STRATEGY_CLASS(__name)	\
  class __name : public strategy {	\
  public:				\
    virtual void main(bot&);		\
  }

DECLARE_STRATEGY_CLASS(wander);
DECLARE_STRATEGY_CLASS(tiler);
DECLARE_STRATEGY_CLASS(moveto);
DECLARE_STRATEGY_CLASS(distri);
DECLARE_STRATEGY_CLASS(test);
DECLARE_STRATEGY_CLASS(bsod);


// util class, for functions common to strats
class util
{
public:
  static void front_low_sharps(bot&, unsigned int[4]);
  static unsigned int min_front_low_sharp(bot&);
  static unsigned int front_high_middle_sharp(bot&);
  static size_t read_front_sharps_get_min(bot&, unsigned int[12]);
  static void get_front_position(bot&, unsigned int&, unsigned int&);
};


#endif // ! STRATEGY_STRATEGY_HH_INCLUDED
