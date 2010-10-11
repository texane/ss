//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Oct 11 19:46:49 2010 texane
// Last update Mon Oct 11 20:50:39 2010 texane
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
DECLARE_STRATEGY_CLASS(test);
DECLARE_STRATEGY_CLASS(bsod);


#endif // ! STRATEGY_STRATEGY_HH_INCLUDED
