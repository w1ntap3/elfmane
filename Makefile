COMPILE_PATH := src/main.c
EXECUTABLE := main

run: compile
	./${EXECUTABLE}

path: compile
	cp ${EXECUTABLE} elfmane
	chmod +x ./elfmane
	sudo rm /usr/local/bin/elfmane
	sudo mv elfmane /usr/local/bin
compile:
	gcc ${COMPILE_PATH} -o ${EXECUTABLE}

clean:
	rm ${EXECUTABLE}