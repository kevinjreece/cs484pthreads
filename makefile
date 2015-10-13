.PHONY: all project clean

all: project

project: seq pthread

seq: src/seqMain.c
	@echo "Compiling Sequential project"
	@gcc -std=gnu99 -O3 src/seqMain.c -o bin/seqMain.o

pthread: src/pthreadMain.c
	@echo "Compiling Pthread project"
	@gcc -lpthread -std=gnu99 -O3 src/pthreadMain.c -o bin/pthreadMain.o

clean:
	@rm -f bin/*
