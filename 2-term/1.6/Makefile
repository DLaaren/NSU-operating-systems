TARGET1 = mythread_create
SRCS1 = mythread_create.c

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all:
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS1} ${LIBS} -o ${TARGET1}

clean:
	${RM} -f *.o ${TARGET1}
