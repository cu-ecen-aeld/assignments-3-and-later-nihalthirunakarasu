INCLUDE_DIRS = 
LIB_DIRS = 
CC=gcc

CDEFS=
CFLAGS= -O0 -g -Wall -Werror $(INCLUDE_DIRS) $(CDEFS)
LIBS=  

HFILES=
CFILES=aesdsocket.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:    aesdsocket

clean:
	-rm -f *.o *.d
	-rm -f aesdsocket

aesdsocket: aesdsocket.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o 

depend:

.cpp.o: $(SRCS)
	$(CC) $(CFLAGS) -c $<