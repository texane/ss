#include <list>
#include <string>


using std::list;


// physics

typedef struct object_info
{
  cpShape* get_shape() const
  {
  }

} object_info_t;

struct object_shape_adder
{
  cpSpace* _space;

  object_shape_adder(cpSpace* space)
    : _space(space) {}

  void operator()(object_info_t* oi)
  {
    cpShape* const os = oi->get_shape();
    if (os == NULL)
      return ;

    spSpaceAddShape();
  }

} object_shape_adder_t;

static void init_physics(const list<object_info_t>& ois)
{
  object_shape_adder_t adder;
  std::for_each(ois.begin(), ois.end(), adder());
}


// viewer::render_map

void viewer::set_scale()
{
}

static void draw_map()
{
  // draw distribution area
  // draw tiles
}

static void update_graphics()
{
}


// main

int main(int ac, char** av)
{
  return 0;
}
