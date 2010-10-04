##
## Made by fabien le mentec <texane@gmail.com>
## 
## Started on  Sun Oct 25 10:42:55 2009 texane
## Last update Mon Oct  4 08:40:54 2010 fabien le mentec
##


CC	:=	g++
LD	:=	g++
CFLAGS	:=	-Wall -Werror -O3 -I../src
LFLAGS	:=	
RM	:=	rm -f


BIN	:=	ss

SRCS	:=	../src/main.cc		\
		../src/conf.cc		\
		../src/view.cc

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
