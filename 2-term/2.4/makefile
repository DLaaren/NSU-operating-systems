TARGET_1 = test
SRCS_1 = test.c

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all: ${TARGET_1}

${TARGET_1}: my_spinlock.h my_mutex.h ${SRCS_1}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_1} ${LIBS} -o ${TARGET_1}

clean:
	${RM} -f *.o ${TARGET_1}
