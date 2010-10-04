#include "asserv.hh"


// private

void asserv::set_cmd(enum cmd_op op, double a, double b, double c)
{
  _cmd._op = op;

  _cmd._args[0] = a;
  _cmd._args[1] = b;
  _cmd._args[2] = c;
}


// exported

asserv::asserv()
{
  set_cmd(CMD_OP_NONE);
}


void asserv::move_forward(double d)
{
  set_cmd(CMD_OP_MOVE_FORWARD, d);
}


void asserv::rotate(double a)
{
  set_cmd(CMD_OP_ROTATE, a);
}


void asserv::set_pos(double x, double y)
{
  _bot->_phys->set_pos(x, y);
}


void asserv::get_pos(double& x, double& y)
{
  _bot->_phys->get_pos(x, y);
}


void asserv::set_power(bool ison)
{
  if (ison == false)
    _bot->_phys->set_vel(0);
}


bool asserv::is_done()
{
  if (_cmd._op == NONE)
    return true;
  return false;
}


void asserv::next()
{
  switch (_cmd._op)
  {
  case CMD_OP_MOVE_FORWARD:
    double x, y;
    _bot->_phys->get_pos(x, y);
    if ((x == _cmd._args[0]) && (y == _cmd._args[1]))
      _cmd._op = CMD_OP_NONE;
    break;

  case CMD_OP_ROTATE:
    double a;
    _bot->_phys->get_angle(a);
    if (a == _cmd._args[0])
      _cmd._op = CMD_OP_NONE;
    break;

  default:
    break;
  }
}
