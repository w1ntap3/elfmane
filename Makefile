SRC := $(wildcard src/*.c)
TARGET := main

run: compile
	./${TARGET}

path: compile
	cp ${TARGET} elfmane
	chmod +x ./elfmane
	sudo rm /usr/local/bin/elfmane
	sudo mv elfmane /usr/local/bin

compile:
	gcc ${SRC} -o ${TARGET}

clean:
	rm ${TARGET}
