##
## Made by fabien le mentec <texane@gmail.com>
## 
## Started on  Sun Oct 25 10:42:55 2009 texane
## Last update Thu Oct 14 20:11:11 2010 texane
##


CC	:=	g++
LD	:=	g++

SDL_CFLAGS :=	$(shell pkg-config --cflags sdl)
SDL_LFLAGS :=	$(shell pkg-config --libs sdl)

CHIPMUNK_LFLAGS := -lchipmunk

CFLAGS	:=	-Wall -O3 $(SDL_CFLAGS) -I../src
LFLAGS	:=	$(SDL_LFLAGS) $(CHIPMUNK_LFLAGS) -lpthread -lm

RM	:=	rm -f

BIN	:=	ss

SRCS	:=	../src/main.cc			\
		../src/conf.cc			\
		../src/bot.cc			\
		../src/physics/physics.cc	\
		../src/graphics/x.cc		\
		../src/device/clamp.cc		\
		../src/device/asserv.cc		\
		../src/device/sensor.cc		\
		../src/device/intersect.cc	\
		../src/device/ticker.cc		\
		../src/strategy/strategy.cc	\
		../src/strategy/tile.cc		\
		../src/strategy/util.cc		\
		../src/strategy/test.cc		\
		../src/strategy/bsod.cc		\
		../src/strategy/wander.cc	\
		../src/strategy/moveto.cc	\
		../src/strategy/tiler.cc	\
		../src/strategy/distri.cc

OBJS	:=	$(SRCS:.cc=.o)


all	:	$(BIN)
$(BIN)	:	$(OBJS)
		$(LD) $(LFLAGS) -o $(BIN) $(OBJS)
clean	:
		$(RM) $(OBJS)
fclean	:	clean
		$(RM) $(BIN)
re	:	fclean all
%.o	:	%.cc
		$(CC) $(CFLAGS) -o $@ -c $<
