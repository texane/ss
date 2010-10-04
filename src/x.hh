//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Mon Aug 17 17:06:48 2009 texane
// Last update Sun Oct 25 17:27:29 2009 texane
//



#ifndef X_H_INCLUDED
# define X_H_INCLUDED



struct x_color;
struct x_event;



/* event types
 */

#define X_EVENT_TICK 0
#define X_EVENT_QUIT 1
#define X_EVENT_KUP_SPACE 2
#define X_EVENT_KDOWN_SPACE 3


#ifdef __cplusplus
extern "C" {
#endif

int x_initialize(unsigned int);
void x_cleanup(void);
int x_set_refresh_rate(unsigned int);
void x_loop(int (*)(const struct x_event*, void*), void*);
int x_alloc_color(const unsigned char*, const struct x_color**);
void x_free_color(const struct x_color*);
void x_draw_pixel(int, int, const struct x_color*);
void x_draw_line(int, int, int, int, const struct x_color*);
void x_draw_square(int, int, int, const struct x_color*);
void x_draw_circle(int, int, int, const struct x_color*);
int x_event_get_type(const struct x_event*);
int x_get_width(void);
int x_get_height(void);

#ifdef __cplusplus
}
#endif



#endif /* ! X_H_INCLUDED */
