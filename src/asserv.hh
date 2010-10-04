#ifndef ASSERV_HH_INCLUDED
# define ASSERV_HH_INCLUDED


class asserv
{
public:
  void move_forward(double);
  void rotate(double);
  void set_pos(double, double);
  void get_pos(double, double);
  bool is_done();
};


#endif // ! ASSERV_HH_INCLUDED
