#ifndef ASSERV_HH_INCLUDED
# define ASSERV_HH_INCLUDED


// forward decl
class bot;


class asserv
{
  // physics engine backlink
  bot* _bot;

  // last issued command

  enum cmd_op
  {
    CMD_OP_NONE = 0,
    CMD_OP_MOVE_FORWARD,
    CMD_OP_ROTATE,
    CMD_OP_INVALID
  };

  typedef struct cmd
  {
    enum cmd_op _op;
    double _args[3];
  } cmd_t;

  cmd_t _cmd;

  void set_cmd(enum cmd_op, double = 0.f, double = 0.f, double = 0.f);

public:
  asserv();

  void move_forward(double);
  void rotate(double);
  void set_pos(double, double);
  void get_pos(double, double);
  void set_power(bool);
  bool is_done();

  void next();
};


#endif // ! ASSERV_HH_INCLUDED
