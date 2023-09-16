TARGET1 = thread-example-a
SRCS1 = thread.c

TARGET2 = thread-example-b
SRCS2 = thread-b.c

TARGET3 = thread-example-c
SRCS3 = thread-c.c

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all:
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS1} ${LIBS} -o ${TARGET1}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS2} ${LIBS} -o ${TARGET2}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS3} ${LIBS} -o ${TARGET3}

clean:
	${RM} -f *.o ${TARGET1}
	${RM} -f *.o ${TARGET2}
	${RM} -f *.o ${TARGET3}
