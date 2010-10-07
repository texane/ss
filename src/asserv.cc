//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Wed Oct  6 22:08:06 2010 texane
// Last update Thu Oct  7 09:05:27 2010 fabien le mentec
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
  _conf_v.write(v);
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

int asserv::get_angle()
{
  return _a.read();
}

void asserv::set_angle(int a)
{
  _a.write(a);
}

void asserv::move_forward(int d)
{
  lock_command();

  const double rads = _a.read() * M_PI / 180.f;

  const int x = _x._value + (int)(::cos(rads) * (double)d);
  const int y = _y._value + (int)(::sin(rads) * (double)d);
  set_command(CMD_OP_MOVE_FORWARD, x, y, _conf_v._value);

  unlock_command();
}

void asserv::turn(int a)
{
  lock_command();
  set_command(CMD_OP_TURN, (_a._value + a) % 360);
  unlock_command();
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
      body->v.y = 0.f;

      // asserv accuracy
      if (d > 30.f)
      {
	body->v.x = (dx * consign_v) / d;
	body->v.y = (dy * consign_v) / d;
      }
      else
      {
	complete_command(CMD_STATUS_SUCCESS);
      }

      // update state
      _x.write((int)body->p.x);
      _y.write((int)body->p.y);
      _v.write((int)::sqrt(body->v.x * body->v.x + body->v.y * body->v.y));
      _a.write((int)body->a);

      break;
    }

  case CMD_OP_TURN:
    {
      const cpFloat consign_a = (cpFloat)_cmd._args[0];
      const cpFloat body_a = (cpFloat)((int)body->a % 360);

      body->w = 0;

      if (::fabs(consign_a - body_a) > 0.1f)
      {
	// angular speed, degrees per second
	body->w = 360;
      }
      else
      {
	complete_command(CMD_STATUS_SUCCESS);
      }

      // update state
      _a.write((int)body_a);

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
