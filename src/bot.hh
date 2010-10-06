#ifndef BOT_HH_INCLUDED
# define BOT_HH_INCLUDED


class conf;

struct cpPolyShape;
struct cpBody;


void create_bots(const conf&);
void delete_bots();
void schedule_bots();
void set_bot_physics(bool, struct cpBody*, struct cpPolyShape*);
void update_bot_velocity(struct cpBody*, double);


#endif // ! BOT_HH_INCLUDED
