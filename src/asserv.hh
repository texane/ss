//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Wed Oct  6 22:56:41 2010 texane
// Last update Fri Oct  8 22:38:02 2010 texane
//


#ifndef ASSERV_HH_INCLUDED
# define ASSERV_HH_INCLUDED


#include "atomic.hh"


// forward decl
struct cpBody;


class asserv
{
  // concurrent with bot scheduling
  atomic_int_t _lock;

  // user defined values
  atomic_int_t _conf_v;

  // actual cached state from physics
  atomic_int_t _x;
  atomic_int_t _y;
  atomic_int_t _v;
  atomic_int_t _a; // degrees

  // last issued command
  enum cmd_op
  {
    CMD_OP_NONE = 0,
    CMD_OP_MOVE_FORWARD,
    CMD_OP_TURN,
    CMD_OP_INVALID
  };

  enum cmd_status
  {
    CMD_STATUS_SUCCESS = 0,
    CMD_STATUS_INPROGRESS,
    CMD_STATUS_FAILURE
  };

  typedef struct cmd
  {
    unsigned char _status;
    unsigned char _op;
    int _args[4];
  } cmd_t;

  volatile cmd_t _cmd;

  enum locked_tag { LOCKED_TAG = 0 };
  void set_command(enum cmd_op, int = 0, int = 0, int = 0);
  void set_command(enum locked_tag, enum cmd_op, int = 0, int = 0, int = 0);
  void complete_command(enum cmd_status);
  void lock_command();
  void unlock_command();
  enum cmd_status read_status();

public:
  asserv();

  // accessors
  void set_velocity(int);
  void set_position(int, int);
  void get_position(int&, int&);

  // angle accessor, degrees
  int get_angle();
  void set_angle(int);

  // commands
  void move_forward(int);
  void move_to(unsigned int, unsigned int);
  void turn(unsigned int, int = 360);
  void turn_left(unsigned int);
  void turn_right(unsigned int);
  void turn_to(unsigned int);

  // command completion
  void wait_done();

  // schedule asserv
  void next(struct cpBody*);
};


#endif // ! ASSERV_HH_INCLUDED
