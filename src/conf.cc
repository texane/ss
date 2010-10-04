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

  if (p == base)
    return -1;

  mf->off += (p - base) + skipnl;

  return 0;
}

static int next_keyval(mapped_file_t* mf, pair& keyval)
{
  return 0;
}


// exported

int conf::load(conf::all_t& all, const char* path)
{
  // key val format based

  int error = -1;
  mapped_file_t mf = {NULL, 0, 0};
  pair<string, string> keyval;

  if (map_file(&mf, path) == -1)
    return -1;

  while (next_keyval(&mf, keyval) != -1)
  {
  }

  // success
  error = 0;

 on_error:
  unmap_file(&mf);

  return error;
}
