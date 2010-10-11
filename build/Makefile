##
## Made by fabien le mentec <texane@gmail.com>
## 
## Started on  Sun Oct 25 10:42:55 2009 texane
## Last update Mon Oct 11 21:24:29 2010 texane
##


CC	:=	g++
LD	:=	g++
CFLAGS	:=	-Wall -O3 `pkg-config --cflags sdl` -I../src
LFLAGS	:=	`pkg-config --libs sdl` -lchipmunk -lpthread -lm
RM	:=	rm -f


BIN	:=	ss

SRCS	:=	../src/main.cc		\
		../src/conf.cc		\
		../src/bot.cc		\
		../src/physics/physics.cc	\
		../src/graphics/x.cc		\
		../src/device/clamp.cc		\
		../src/device/asserv.cc		\
		../src/device/sensor.cc		\
		../src/device/intersect.cc	\
		../src/device/ticker.cc		\
		../src/strategy/strategy.cc	\
		../src/strategy/utility.cc	\
		../src/strategy/test.cc		\
		../src/strategy/bsod.cc		\
		../src/strategy/wander.cc	\

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
