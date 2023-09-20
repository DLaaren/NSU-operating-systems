TARGET1 = threads-a
SRCS1 = threads-a.c

TARGET2 = threads-d
SRCS2 = threads-d.c

TARGET3 = threads-e
SRCS3 = threads-e.c

TARGET4 = threads-f
SRCS4 = threads-f.c

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all:
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS1} ${LIBS} -o ${TARGET1}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS2} ${LIBS} -o ${TARGET2}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS3} ${LIBS} -o ${TARGET3}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS4} ${LIBS} -o ${TARGET4}

clean:
	${RM} -f *.o ${TARGET1}
	${RM} -f *.o ${TARGET2}
	${RM} -f *.o ${TARGET3}
	${RM} -f *.o ${TARGET4}