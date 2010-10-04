#ifndef VIEW_HH_INCLUDED
# define VIEW_HH_INCLUDED


#include "conf.hh"


namespace view
{
  typedef struct context
  {
    // x context pointer

    double _wscale;
    double _hscale;

  } context_t;

  int initialize(view::context_t&, const conf::all_t&);
  void update(view::context_t&);
}


#endif // ! VIEW_HH_INCLUDED
