#ifndef BOT_HH_INCLUDED
# define BOT_HH_INCLUDED


class conf;
struct cpPolyShape;


void create_bots(const conf&);
void delete_bots();
void set_bot_physics(bool, cpPolyShape*);
void schedule_bots();


#endif // ! BOT_HH_INCLUDED
