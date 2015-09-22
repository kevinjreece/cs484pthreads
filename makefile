.PHONY: all project clean

all: project

project: cpp c

cpp: src/*.cpp src/*.h
	@echo "Compiling C++ project"
	@g++ -O3 src/*.cpp -o bin/project_cpp.o

c: src/*.c src/*.h
	@echo "Compiling C project"
	@gcc -std=c99 -O3 src/*.c -o bin/project_c.o

clean:
	@rm -f bin/*
