SRC := $(wildcard src/*.c)
TARGET := elfmane
PATH_FOR_BIN := ~/.local/bin/
COMPILE_FLAGS := -Wall -Wextra -Wpedantic -std=c17 -O2 -g
BASH_COMPLETION := ~/.local/share/bash-completion/completions/
BASH_COMPLETION_SCRIPT := shell-completion/

bash: path
	mkdir -p ${BASH_COMPLETION}
	cp ${BASH_COMPLETION_SCRIPT}/${TARGET}.bash ${BASH_COMPLETION}/${TARGET}

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
	rm -f $(BASH_COMPLETION)$(TARGET)
