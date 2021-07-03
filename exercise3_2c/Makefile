INCLUDE_DIRS =
LIB_DIRS =
CC=gcc

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lpthread -lrt -lm

PRODUCT=mutex_lock

HFILES=
CFILES= mutex_lock.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:    ${PRODUCT}

clean:
	-rm -f *.o *.NEW *~ *.d
	-rm -f ${PRODUCT} ${GARBAGE}

mutex_lock:    mutex_lock.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ mutex_lock.o $(LIBS)

depend:

.c.o:
	$(CC) -MD $(CFLAGS) -c $<

