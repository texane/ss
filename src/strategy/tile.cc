#include <stdio.h>
#include <sys/types.h>
#include "strategy/tile.hh"


/* find a free tile of the same color in the neighborhood */

bool find_free_neighbor_tile(const unsigned int* tiles, bool is_red, unsigned int& x, unsigned int& y)
{
  /* north, clockwise */
  static const int dirs[8][2] =
  {
    {  0, -1 },
    {  1, -1 },
    {  1,  0 },
    {  1,  1 },
    {  0,  1 },
    { -1,  1 },
    { -1,  0 },
    { -1, -1 }
  };

  /* translate to tile repere */

  static const size_t ndirs = sizeof(dirs) / sizeof(dirs[0]);
  for (size_t i = 0; i < ndirs; ++i)
  {
    const int nx = (int)x + dirs[i][0];
    const int ny = (int)y + dirs[i][1];

    if ((nx == -1) || (nx >= (int)tiles_per_row))
      continue ;
    else if ((ny == -1) || (ny >= (int)tiles_per_col))
      continue ;
    else if (is_tile_red(nx, ny) != is_red)
      continue ;
    else if (is_tile_used(tiles, nx, ny))
      continue ;

    /* found a free is_red tile */
    x = (unsigned int)nx;
    y = (unsigned int)ny;

    return true;
  }

  return false;
}


/* print the tile states */

static inline char tile_to_char(unsigned int tile)
{
  if ((tile & TILE_FLAG_SEEN) == 0)
  {
    return '?';
  }
  if (tile & TILE_FLAG_USED)
  {
    if (tile & TILE_FLAG_RED)
      return 'r';
    return 'b';
  }
  return ' ';
}

void print_tiles(const unsigned int* tiles)
{
  for (size_t i = 0; i < tiles_per_row; ++i)
  {
    for (size_t j = 0; j < tiles_per_col; ++j)
      printf("%c", tile_to_char(get_const_tile_at(tiles, i, j)));
    printf("\n");
  }
  printf("\n");
}
