#ifndef CONF_HH_INCLUDED
# define CONF_HH_INCLUDED


#include <list>
#include <string>


class conf
{
public:

  // object configuration
  typedef struct object
  {
    enum object_type
    {
      OBJECT_TYPE_BLUE_BOT = 0,
      OBJECT_TYPE_RED_BOT,
      OBJECT_TYPE_PAWN,
      OBJECT_TYPE_KING,
      OBJECT_TYPE_QUEEN,
      OBJECT_TYPE_WALL,
      OBJECT_TYPE_INVALID
    };

    // object type
    enum object_type _type;

    double _x;
    double _y;

    double _w;
    double _h;

    double _a;

    // bot strategy
    std::string _s;

    void clear()
    {
      _type = OBJECT_TYPE_INVALID;
      _x = 0.f;
      _y = 0.f;
      _w = 0.f;
      _h = 0.f;
      _a = 0.f;
      _s.clear();
    }

  } object_t;

  // members
  static const double _space_width = 3000 + 1;
  static const double _space_height = 2100 + 1;

  std::list<object_t> _objects;

  unsigned int _active_count;
  unsigned int _static_count;

  // exported
  int load(const char*);

#if CONFIG_DEBUG
  void print() const;
#endif
};


#endif // ! CONF_HH_INCLUDED
