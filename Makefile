SRC := $(wildcard src/*.c)
TARGET := elfmane
PATH_FOR_BIN := ~/.local/bin/
COMPILE_FLAGS := -Wall

path: compile
	chmod +x ${TARGET}
	mv ${TARGET} ${PATH_FOR_BIN}

run: compile
	./${TARGET}

compile:
	gcc ${SRC} -o ${TARGET} ${COMPILE_FLAGS}

clean:
	rm -f ./${TARGET} 
	rm -f ${PATH_FOR_BIN}${TARGET}
