//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Wed Oct  6 22:08:06 2010 texane
// Last update Fri Oct  8 23:38:15 2010 texane
//


#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "physics.hh"
#include "asserv.hh"
#include "dtor.hh"
#include "rtod.hh"


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

  const double rads = dtor((double)_a._value);

  const int x = _x._value + (int)(::cos(rads) * (double)d);
  const int y = _y._value + (int)(::sin(rads) * (double)d);

  set_command(CMD_OP_MOVE_FORWARD, x, y, _conf_v._value);

  unlock_command();
}

void asserv::move_to(unsigned int x, unsigned int y)
{
  
}

void asserv::turn(unsigned int a, int w)
{
  lock_command();
  set_command(CMD_OP_TURN, a, _a._value, w);
  unlock_command();
}

void asserv::turn_left(unsigned int a)
{
  turn(a, -360);
}

void asserv::turn_right(unsigned int a)
{
  turn(a, 360);
}

void asserv::turn_to(unsigned int a)
{
  // compute how much to turn from the current angle

  lock_command();

  const int curr_a = _a._value;

  int w = 360;

  int diff_a = ((int)a - curr_a) % 360;

  if (diff_a < -180)
  {
    diff_a = 360 + diff_a;
  }
  else if (diff_a < 0)
  {
    diff_a *= -1;
    w = -360;
  }
  else if (diff_a > 180)
  {
    diff_a = 360 - diff_a;
    w = -360;
  }

  set_command(CMD_OP_TURN, diff_a, curr_a, w);

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
      if (d > 10.f)
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

      break;
    }

  case CMD_OP_TURN:
    {
      // in degrees
      const cpFloat cons_a = (cpFloat)_cmd._args[0];
      const cpFloat orig_a = (cpFloat)_cmd._args[1];
      const cpFloat curr_a = (cpFloat)rtod(body->a);

      // angular speed, degrees per second      
      cpFloat w = (cpFloat)dtor((cpFloat)_cmd._args[2]);

      const int diff = (int)::fabs(curr_a - orig_a);
      if (diff >= cons_a)
      {
	complete_command(CMD_STATUS_SUCCESS);
	w = 0.f;
      }

      // update asserv state
      _a.write((int)curr_a);

      // update body angular velocty
      body->w = w;

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
