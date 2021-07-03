NCLUDE_DIRS = 
LIB_DIRS = 

CDEFS= 
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lpthread -lrt

PRODUCT=heap_mq

HFILES=
CFILES= heap_mq.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	${PRODUCT}

clean:
	-rm -f *.o *.NEW *~ *.d
	-rm -f ${PRODUCT} ${GARBAGE}

heap_mq:	heap_mq.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ heap_mq.o $(LIBS)
.c.o:
	$(CC) $(CFLAGS) -c $<
