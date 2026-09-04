SRC := $(wildcard src/*.c)
TARGET := elfmane
PATH_FOR_BIN := ~/.local/bin/

run: compile
	./${TARGET}

path: compile
	chmod +x ${TARGET}
	mv ${TARGET} ${PATH_FOR_BIN}

compile:
	gcc ${SRC} -o ${TARGET}

clean:
	rm -f ./${TARGET} 
	rm -f ${PATH_FOR_BIN}${TARGET}
