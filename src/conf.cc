#include <list>
#include <utility>
#include <string>
#include "conf.hh"

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>


using std::list;
using std::pair;
using std::string;


typedef pair<string, string> keyval_t;


// internal, file reading

static char line_buf[1024];

typedef struct mapped_file
{
  unsigned char* base;
  size_t off;
  size_t len;
} mapped_file_t;

static int map_file(mapped_file_t* mf, const char* path)
{
  int error = -1;
  struct stat st;

  const int fd = open(path, O_RDONLY);
  if (fd == -1)
    return -1;

  if (fstat(fd, &st) == -1)
    goto on_error;

  mf->base = (unsigned char*)mmap
    (NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
  if (mf->base == MAP_FAILED)
    goto on_error;

  mf->off = 0;
  mf->len = st.st_size;

  error = 0;

 on_error:
  close(fd);

  return error;
}

static void unmap_file(mapped_file_t* mf)
{
  munmap(mf->base, mf->len);
  mf->base = (unsigned char*)MAP_FAILED;
  mf->len = 0;
}

static int read_line(mapped_file_t* mf, char** line)
{
  const unsigned char* end = mf->base + mf->len;
  const unsigned char* const base = mf->base + mf->off;
  const unsigned char* p;
  size_t skipnl = 0;
  char* s;

  *line = line_buf;

  for (p = base, s = line_buf; p != end; ++p, ++s)
  {
    if (*p == '\n')
    {
      skipnl = 1;
      break;
    }

    *s = (char)*p;
  }

  *s = 0;

  if ((p == base) && (!skipnl))
    return -1;

  mf->off += (p - base) + skipnl;

  return 0;
}

static int next_keyval(mapped_file_t* mf, keyval_t& keyval)
{
  char* line;
  char* key;
  char* val;

  if (read_line(mf, &line) == -1)
    return -1;

  for ( ; *line == ' '; ++line)
    ;

  if ((*line == 0) || (*line == '#'))
    goto on_empty_keyval;

  for (key = line; (*line) && (*line != ' '); ++line)
    ;

  if (*line == 0)
    goto on_empty_keyval;

  *line = 0;

  for (line = line + 1; *line == ' '; ++line)
    ;

  if (*line == 0)
    goto on_empty_keyval;

  for (val = line; (*line) && (*line != ' '); ++line)
    ;

  *line = 0;

  keyval.first = string(key);
  keyval.second = string(val);

  return 0;

  // empty keyval but dont stop processing
 on_empty_keyval:
  keyval.first.clear();
  keyval.second.clear();
  return 0;
}


static enum conf::object::object_type str_to_type(const string& s)
{
  if (s.compare("red_bot") == 0)
    return conf::object::OBJECT_TYPE_RED_BOT;
  else if (s.compare("blue_bot") == 0)
    return conf::object::OBJECT_TYPE_BLUE_BOT;
  else if (s.compare("pawn") == 0)
    return conf::object::OBJECT_TYPE_PAWN;
  else if (s.compare("king") == 0)
    return conf::object::OBJECT_TYPE_KING;
  else if (s.compare("queen") == 0)
    return conf::object::OBJECT_TYPE_QUEEN;
  else if (s.compare("wall") == 0)
    return conf::object::OBJECT_TYPE_WALL;

  return conf::object::OBJECT_TYPE_INVALID;
}


// exported

int conf::load(const char* path)
{
  // key val format based

  mapped_file_t mf = {NULL, 0, 0};
  keyval_t keyval;

  string& key = keyval.first;
  string& val = keyval.second;

  object_t object;

  if (map_file(&mf, path) == -1)
    return -1;

  while (next_keyval(&mf, keyval) != -1)
  {
    if (key.empty())
      continue ;

    if (key.compare("begin") == 0)
      memset(&object, 0, sizeof(object));
    else if (key.compare("end") == 0)
      _objects.push_back(object);
    else if (key.compare("type") == 0)
      object._type = str_to_type(val);
    else if (key.compare("x") == 0)
      object._x = ::atof(val.c_str());
    else if (key.compare("y") == 0)
      object._y = ::atof(val.c_str());
    else if (key.compare("w") == 0)
      object._w = ::atof(val.c_str());
    else if (key.compare("h") == 0)
      object._h = ::atof(val.c_str());
  }

  unmap_file(&mf);

  return 0;
}


#if CONFIG_DEBUG

#include <algorithm>
#include <stdio.h>

static string type_to_str(enum conf::object::object_type type)
{
  if (type == conf::object::OBJECT_TYPE_RED_BOT)
    return string("red_bot");
  else if (type == conf::object::OBJECT_TYPE_BLUE_BOT)
    return string("blue_bot");
  else if (type == conf::object::OBJECT_TYPE_PAWN)
    return string("pawn");
  else if (type == conf::object::OBJECT_TYPE_KING)
    return string("king");
  else if (type == conf::object::OBJECT_TYPE_QUEEN)
    return string("queen");
  else if (type == conf::object::OBJECT_TYPE_WALL)
    return string("wall");
  return string("invalid");
}

typedef struct printer
{
  void operator()(const conf::object& o) const
  {
    printf("%s\n", type_to_str(o._type).c_str());
  }

} printer_t;

void conf::print() const
{
  std::for_each(_objects.begin(), _objects.end(), printer_t());
}

#endif
