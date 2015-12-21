.PHONY: all project clean

all: project

project: seq pthread linear log rebuild omp

seq: src/seqMain.c
	@echo "Compiling Sequential project"
	@gcc -std=gnu99 -O3 src/seqMain.c -o bin/seqMain.o

pthread: src/pthreadMain.c
	@echo "Compiling Pthread project"
	@gcc -lpthread -std=gnu99 -O3 src/pthreadMain.c -o bin/pthreadMain.o

linear: src/linearMain.c
	@echo "Compiling Linear project"
	@gcc -lpthread -std=gnu99 -O3 src/linearMain.c -o bin/linearMain.o

log: src/logMain.c
	@echo "Compiling Log project"
	@gcc -lpthread -std=gnu99 -O3 src/logMain.c -o bin/logMain.o

rebuild: src/rebuildMain.c
	@echo "Compiling Rebuild project"
	@gcc -lpthread -std=gnu99 -O3 src/rebuildMain.c -o bin/rebuildMain.o

omp: src/ompMain.c
	@gcc -std=c99 -O3 -fopenmp src/ompMain.c -o bin/ompMain.o

clean:
	@rm -f bin/*
