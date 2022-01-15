run: ld2
	./ld2
ld2: build/ld2.o
	gcc build/ld2.o build/rcgl.o -lSDL2 -g -o ld2

build/ld2.o: ld2.c get_parse.h loadBSV.h
	gcc -Wall -pedantic -c ld2.c -g -o build/ld2.o

build/rcgl.o: rcgl/rcgl.c rcgl/rcgl.h
	gcc -c rcgl/rcgl.c -g -o build/rcgl.o

setupDevEnv:
	git clone https://github.com/ChartreuseK/rcgl.git
	mkdir build