.PHONY: all project clean

all: project

project: seq

seq: src/seqMain.cpp
	@echo "Compiling Sequential project"
	@g++ -O3 src/seqMain.cpp -o bin/sequential.o

clean:
	@rm -f bin/*
