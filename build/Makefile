##
## Made by fabien le mentec <texane@gmail.com>
## 
## Started on  Sun Oct 25 10:42:55 2009 texane
## Last update Sun Oct 10 13:15:19 2010 texane
##


CC	:=	g++
LD	:=	g++
CFLAGS	:=	-Wall -O3 `pkg-config --cflags sdl` -I../src
LFLAGS	:=	`pkg-config --libs sdl` -lchipmunk -lpthread -lm
RM	:=	rm -f


BIN	:=	ss

SRCS	:=	../src/main.cc		\
		../src/conf.cc		\
		../src/physics.cc	\
		../src/strategy.cc	\
		../src/bot.cc		\
		../src/clamp.cc		\
		../src/asserv.cc	\
		../src/sensor.cc	\
		../src/ticker.cc	\
		../src/intersect.cc	\
		../src/x.cc

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
