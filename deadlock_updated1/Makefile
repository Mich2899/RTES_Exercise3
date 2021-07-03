INCLUDE_DIRS =
LIB_DIRS =
CC=gcc

CDEFS=
CFLAGS= -O3 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lpthread -lrt

PRODUCT=deadlock_updated

HFILES=
CFILES= deadlock_updated.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:    ${PRODUCT}

clean:
	-rm -f *.o *.NEW *~ *.d
	-rm -f ${PRODUCT} ${GARBAGE}

deadlock_updated:    deadlock_updated.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ deadlock_updated.o $(LIBS)

depend:

.c.o:
	$(CC) -MD $(CFLAGS) -c $<

