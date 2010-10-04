#include <list>
#include <algorithm>
#include "view.hh"


using std::list;


// wrapper internal types

typedef struct point
{
  double _x;
  double _y;
} point_t;

typedef struct rect
{
  // clockwise order
  point_t _points[4];

  rect(double x, double y, double w, double h)
  {
    _points[0]._x = x - w / 2.f;
    _points[0]._y = y + h / 2.f;

    _points[1]._x = x + w / 2.f;
    _points[1]._y = y + h / 2.f;

    _points[2]._x = x + w / 2.f;
    _points[2]._y = y - h / 2.f;

    _points[3]._x = x - w / 2.f;
    _points[3]._y = y - h / 2.f;
  }
  
} rect_t;

typedef struct circle
{
  double _r;
  double _x;
  double _y;

  rect(double x, double y, double w, double h)
    : _x(x), _y(y), _w(w), _h(h) {}
  
} circle_t;



// scale from space to view

static inline void space_to_view(const view_context& vc, circle_t& c)
{
  // inplace update
}

static inline void space_to_view(const view_context& vc, rect_t& r)
{
}

static inline void rotate_rect(rect_t& r, double a)
{
}


typedef struct object_updater
{
  // functor to redraw objects

  view_context_t& _vc;

  object_updater(view_context_t& vc)
    : _vc(vc) {}

  void operator()(const object_state_t& os) const
  {
    // retrieve the view state
    double x;
    double y;
    double w;
    double h;
    double a;
    bool is_circle;
    unsigned char c[3];

    os.get_view_state(x, y, a, is_circle, w, h, c);

    if (is_circle)
    {
      circle_t circ(x, y, w);
      space_to_view(circ, c);
      draw(_vc, circ, c);
    }
    else if // is_rect
    {
      rect_t rect(x, y, w);
      space_to_view(rect, _vc);
      rotate_rect(rect, a);
      draw(_vc, rect, c);
    }
  }

} object_updater_t;


static void draw_map(view::context_t& vc)
{
}


// exported

int view::initialize(view::context_t& vc, const conf::all_t& conf)
{
  // create an internal x context

  // compute w, h scaling factor
  vc._hscale = conf._view_width / conf._space_width;
  vc._wscale = conf._view_height / conf._space_height;

  return 0;
}

void view::update(view::context_t& vc, const sim::context_t& sim)
{
  // redraw map
  draw_map(vc);

  // update objects
  object_updater_t updater(vc);
  list<object_state_t*>::const_iterator beg = sim._object_states.begin();
  list<object_state_t*>::const_iterator end = sim._object_states.end();
  std::for_each(beg, end, updater);
}
