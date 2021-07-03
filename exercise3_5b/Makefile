INCLUDE_DIRS =
LIB_DIRS =
CC=gcc

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lpthread -lrt -lm

PRODUCT= mutex_lock_timed

HFILES=
CFILES= mutex_lock_timed.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:    ${PRODUCT}

clean:
	-rm -f *.o *.NEW *~ *.d
	-rm -f ${PRODUCT} ${GARBAGE}

mutex_lock_timed:    mutex_lock_timed.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ mutex_lock_timed.o $(LIBS)

depend:

.c.o:
	$(CC) -MD $(CFLAGS) -c $<


