//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Wed Oct  6 22:08:06 2010 texane
// Last update Thu Oct  7 00:08:25 2010 texane
//


#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "physics.hh"
#include "asserv.hh"


// private

void asserv::set_command(enum cmd_op op, int a, int b, int c)
{
  _cmd._op = op;

  _cmd._status = CMD_STATUS_INPROGRESS;

  _cmd._args[0] = a;
  _cmd._args[1] = b;
  _cmd._args[2] = c;
}

void asserv::set_command
(enum locked_tag, enum cmd_op op, int a, int b, int c)
{
  // locked version
  lock_command();
  set_command(op, a, b, c);
  unlock_command();
}

void asserv::complete_command(enum cmd_status status)
{
  _cmd._status = CMD_STATUS_SUCCESS;
}

void asserv::lock_command()
{
  while (!__sync_bool_compare_and_swap(&_lock._value, 0, 1))
    __asm__ __volatile__ ("pause\n\t");
}

void asserv::unlock_command()
{
  _lock.write(0);
}


// exported

asserv::asserv()
{
  _lock.write(0);
  complete_command(CMD_STATUS_SUCCESS);
}

void asserv::set_velocity(int v)
{
  _v.write(v);
}

void asserv::set_position(int x, int y)
{
  _x.write(x);
  _y.write(y);
}

void asserv::get_position(int& x, int& y)
{
  x = _x.read();
  y = _y.read();
}

void asserv::set_angle(int a)
{
  _a.write(a);
}

void asserv::move_forward(int d)
{
  lock_command();

  const double rads = _a._value * M_PI / 180.f;

  const int x = _x._value + (int)(::cos(rads) * (double)d);
  const int y = _y._value + (int)(::sin(rads) * (double)d);
  set_command(CMD_OP_MOVE_FORWARD, x, y, _v._value);

  unlock_command();
}

void asserv::turn(int a)
{
  set_command(LOCKED_TAG, CMD_OP_TURN, _a.read() + a);
}

void asserv::wait_done()
{
  while (_cmd._status == CMD_STATUS_INPROGRESS)
  {
    // avoid busy looping
    ::usleep(1);
  }
}

void asserv::next(cpBody* body)
{
  // concurrent with bot scheduling
  lock_command();

  if (_cmd._status != CMD_STATUS_INPROGRESS)
    goto on_done;

  switch (_cmd._op)
  {
  case CMD_OP_MOVE_FORWARD:
    {
      const cpFloat consign_x = (cpFloat)_cmd._args[0];
      const cpFloat consign_y = (cpFloat)_cmd._args[1];
      const cpFloat consign_v = (cpFloat)_cmd._args[2];

      const cpFloat dx = consign_x - body->p.x;
      const cpFloat dy = consign_y - body->p.y;
      const cpFloat d = ::sqrt(dx * dx + dy * dy);

      body->v.x = 0.f;
      if (::fabs(dx) > 30.f)
	body->v.x = (dx * d) / consign_v;

      body->v.y = 0.f;
      if (::fabs(dy) > 30.f)
	body->v.y = (dy * d) / consign_v;

      if ((body->v.x == 0.f) && (body->v.y == 0.f))
	complete_command(CMD_STATUS_SUCCESS);

      break;
    }

  default:
    {
      break;
    }
  }

 on_done:
  unlock_command();
}
