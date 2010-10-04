//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Jun 29 15:50:24 2009 texane
// Last update Mon Oct  4 19:52:41 2010 texane
//



#ifdef _WIN32
# include <memory.h>
# include <SDL.H>
#else // __linux__
# include <SDL/SDL.h>
#endif // _WIN32
#include "x.hh"



struct x_color
{
  Uint32 value;
  Uint8 rgb[3];
};



struct x_event
{
  int type;
};



static SDL_Surface* g_screen = NULL;
static SDL_TimerID g_timer = NULL;



static Uint32 on_timer(Uint32 interval, void* param)
{
  static SDL_UserEvent event =
    {
#define SDL_TICKEVENT SDL_USEREVENT + 0
      SDL_TICKEVENT,
      0,
      NULL,
      NULL
    };

  SDL_PushEvent((SDL_Event*)&event);

  return interval;
}



int x_initialize(unsigned int msecs)
{
  if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_DOUBLEBUF) < 0)
    return -1;

#define X_WIDTH 640
#define X_HEIGHT 480
  g_screen = SDL_SetVideoMode(X_WIDTH, X_HEIGHT, 16, SDL_SWSURFACE);
  if (g_screen == NULL)
    {
      SDL_Quit();
      return -1;
    }

  g_timer = SDL_AddTimer(msecs, on_timer, NULL);
  if (g_timer == NULL)
    {
      SDL_Quit();
      return -1;
    }

  return 0;
}



void x_cleanup(void)
{
  if (g_timer != NULL)
    {
      SDL_RemoveTimer(g_timer);
      g_timer = NULL;
    }

  if (g_screen != NULL)
    {
      SDL_FreeSurface(g_screen);
      g_screen = NULL;
    }

  SDL_Quit();
}



int x_set_refresh_rate(unsigned int msecs)
{
  if (g_timer != NULL)
    SDL_RemoveTimer(g_timer);

  g_timer = SDL_AddTimer(msecs, on_timer, NULL);
  if (g_timer == NULL)
    return -1;

  return 0;
}



void x_loop(int (*on_event)(const struct x_event*, void*), void* ctx)
{
  SDL_Event event;
  struct x_event x_event;
  int is_done;
  static const struct x_color* black_color;
  static const unsigned char black_rgb[3] = {0, 0, 0};

  if (on_event == NULL)
    return ;

  if (x_alloc_color(black_rgb, &black_color) == -1)
    return ;

  is_done = 0;

  while (!is_done)
    {
      SDL_WaitEvent(&event);
      switch (event.type)
	{
	case SDL_TICKEVENT:
	  {
	    if (SDL_MUSTLOCK(g_screen))
	      SDL_LockSurface(g_screen);

	    SDL_FillRect(g_screen, NULL, black_color->value);

	    x_event.type = X_EVENT_TICK;
	    if (on_event(&x_event, ctx) == -1)
	      is_done = 1;

	    SDL_Flip(g_screen);

	    if (SDL_MUSTLOCK(g_screen))
	      SDL_UnlockSurface(g_screen);

	    break;
	  }

	case SDL_KEYDOWN:
	  {
	    switch (event.key.keysym.sym)
	      {
	      case SDLK_SPACE:
		x_event.type = X_EVENT_KDOWN_SPACE;
		on_event(&x_event, ctx);
		break;

	      default:
		break;
	      }

	    break;
	  }

	case SDL_KEYUP:
	  {
	    switch (event.key.keysym.sym)
	      {
	      case SDLK_SPACE:
		x_event.type = X_EVENT_KUP_SPACE;
		on_event(&x_event, ctx);
		break;

	      default:
		break;
	      }

	    break;
	  }

	case SDL_QUIT:
	  x_event.type = X_EVENT_QUIT;
	  if (on_event(&x_event, ctx) == -1)
	    is_done = 1;
	  break;

	default:
	  break;
	}
    }

  x_free_color(black_color);
}



int x_alloc_color(const unsigned char* rgb, const struct x_color** res)
{
  static struct x_color colors[32];
  static unsigned int i = 0;

  unsigned int j;
  struct x_color* color;

  for (j = 0; j < i; ++j)
    if (!memcmp(colors[j].rgb, rgb, sizeof(colors[0].rgb)))
      {
	*res = &colors[j];
	return 0;
      }

  if (i >= (sizeof(colors) / sizeof(colors[0])))
    return -1;

  color = &colors[i++];

  color->rgb[0] = rgb[0];
  color->rgb[1] = rgb[1];
  color->rgb[2] = rgb[2];

  color->value = SDL_MapRGB(g_screen->format, color->rgb[0], color->rgb[1], color->rgb[2]);

  *res = color;

  return 0;
}



void x_free_color(const struct x_color* color)
{
}



void x_draw_pixel(int x, int y, const struct x_color* color)
{
  switch (g_screen->format->BytesPerPixel)
    {
    case 1:
      {
	Uint8 *bufp;
	bufp = (Uint8 *)g_screen->pixels + y*g_screen->pitch + x;
	*bufp = color->value;
      }
      break;

    case 2:
      {
	Uint16 *bufp;
	bufp = (Uint16 *)g_screen->pixels + y*g_screen->pitch/2 + x;
	*bufp = color->value;
      }
      break;

    case 3:
      {
	Uint8 *bufp;
	bufp = (Uint8 *)g_screen->pixels + y*g_screen->pitch + x;
	*(bufp+g_screen->format->Rshift/8) = color->rgb[0];
	*(bufp+g_screen->format->Gshift/8) = color->rgb[1];
	*(bufp+g_screen->format->Bshift/8) = color->rgb[2];
      }
      break;

    case 4:
      {
	Uint32 *bufp;
	bufp = (Uint32 *)g_screen->pixels + y*g_screen->pitch/4 + x;
	*bufp = color->value;
      }
      break;
    }
}



void x_draw_line(int x0, int y0, int x1, int y1, const struct x_color* color)
{
  int dx;
  int dy;
  int x;
  int y;
  int e;
  int step;

#define swap_ints(a, b)	\
  do {			\
    int __tmp;		\
    __tmp = a;		\
    a = b;		\
    b = __tmp;		\
  } while (0)

  if (x0 == x1)
    {
      if (y0 > y1)
	swap_ints(y0, y1);

      for (y = y0; y < y1; ++y)
	x_draw_pixel(x0, y, color);

      return ;
    }
  else if (y0 == y1)
    {
      if (x0 > x1)
	swap_ints(x0, x1);

      for (x = x0; x < x1; ++x)
	x_draw_pixel(x, y0, color);

      return ;
    }

  dx = x1 - x0;
  if (dx < 0)
    dx *= -1;

  dy = y1 - y0;
  if (dy < 0)
    dy *= -1;

  if (dx > dy)
    {
      if (x0 > x1)
	{
	  swap_ints(x0, x1);
	  swap_ints(y0, y1);
	}

      if (y0 > y1)
	step = -1;
      else
	step = 1;

      e = 0;
      x = x0;
      y = y0;

      while (x <= x1)
	{
	  x_draw_pixel(x, y, color);

	  e += dy;

	  if ((2 * e) >= dx)
	    {
	      y += step;
	      e -= dx;
	    }

	  ++x;
	}
    }
  else
    {
      if (y0 > y1)
	{
	  swap_ints(x0, x1);
	  swap_ints(y0, y1);
	}

      if (x0 > x1)
	step = -1;
      else
	step = 1;

      e = 0;
      x = x0;
      y = y0;

      while (y <= y1)
	{
	  x_draw_pixel(x, y, color);

	  e += dx;

	  if ((2 * e) >= dy)
	    {
	      x += step;
	      e -= dy;
	    }

	  ++y;
	}
    }
}



void x_draw_square(int x0, int y0, int w, const struct x_color* color)
{
  int x;
  int y;
  int x1;
  int y1;

  w /= 2;

  x1 = x0 + w;
  x0 -= w;

  y1 = y0 + w;
  y0 -= w;

  for (x = x0; x <= x1; ++x)
    {
      x_draw_pixel(x, y0, color);
      x_draw_pixel(x, y1, color);
    }

  for (y = y0; y <= y1; ++y)
    {
      x_draw_pixel(x0, y, color);
      x_draw_pixel(x1, y, color);
    }
}



static void draw_circle_points(int cx, int cy, int x, int y, const struct x_color* color)
{
  if (x == 0)
    {
      x_draw_pixel(cx, cy + y, color);
      x_draw_pixel(cx, cy - y, color);
      x_draw_pixel(cx + y, cy, color);
      x_draw_pixel(cx - y, cy, color);
    }
  else if (x == y)
    {
      x_draw_pixel(cx + x, cy + y, color);
      x_draw_pixel(cx - x, cy + y, color);
      x_draw_pixel(cx + x, cy - y, color);
      x_draw_pixel(cx - x, cy - y, color);
    }
  else if (x < y)
    {
      x_draw_pixel(cx + x, cy + y, color);
      x_draw_pixel(cx - x, cy + y, color);
      x_draw_pixel(cx + x, cy - y, color);
      x_draw_pixel(cx - x, cy - y, color);
      x_draw_pixel(cx + y, cy + x, color);
      x_draw_pixel(cx - y, cy + x, color);
      x_draw_pixel(cx + y, cy - x, color);
      x_draw_pixel(cx - y, cy - x, color);
    }
}


void x_draw_circle(int x_center, int y_center, int radius, const struct x_color* color)
{
  int x = 0;
  int y = radius;
  int p = (5 - radius * 4) / 4;

  draw_circle_points(x_center, y_center, x, y, color);

  while (x < y)
    {
      ++x;

      if (p < 0)
	{
	  p += 2 * x + 1;
	}
      else
	{
	  --y;
	  p += 2 * (x - y) + 1;
	}

      draw_circle_points(x_center, y_center, x, y, color);
    }
}


int x_event_get_type(const struct x_event* event)
{
  return event->type;
}


int x_get_width(void)
{
  return X_WIDTH;
}


int x_get_height(void)
{
  return X_HEIGHT;
}
