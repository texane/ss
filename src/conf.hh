#ifndef CONF_HH_INCLUDED
# define CONF_HH_INCLUDED


#include <list>


namespace conf
{
  // robot configuration
  typedef struct bot
  {
    // bot type
    std::string _type;

    double _x;
    double _y;

    bool _is_red;

  } bot_t;


  // object configuration
  typedef struct object
  {
    // object type
    std::string _type;

    double _x;
    double _y;

    // radius in case of a circle
    double _width;
    double _height;

  } object_t;

  // grouped configuration
  typedef struct all
  {
    std::list<bot_t> _bot_confs;

    unsigned int _map_index;

    static const double _space_width = 3000;
    static const double _space_height = 2000;

    list<object_t> _obj_confs;

  } all_t;


  // exported
  int load(all_t&, const char*);
}


#endif // ! CONF_HH_INCLUDED
