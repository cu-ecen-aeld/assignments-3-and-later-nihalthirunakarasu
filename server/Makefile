INCLUDE_DIRS=
LIB_DIRS=
CC=${CROSS_COMPILE}gcc

CDEFS=
CFLAGS= -O0 -g -Wall -Werror $(INCLUDE_DIRS) $(CDEFS)
LIBS=  
LDFLAGS= -lrt -lpthread

HFILES=
CFILES=aesdsocket.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:aesdsocket

clean:
	-rm -f *.o *.d
	-rm -f aesdsocket

aesdsocket:aesdsocket.c
	$(CC) aesdsocket.c $(CFLAGS) -o $@ $(LDFLAGS)

depend:

.c.o: $(SRCS)
	$(CC) $(CFLAGS) -c $<