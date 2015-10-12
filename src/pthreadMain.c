#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

#include "time.h"
#include "pthreadbarrier.h"

#define EPSILON 0.1
#define THRESHOLD 50
#define LENGTH 324
#define true 1
#define false 0
#define MAX_THREADS 64

float** _plate;
float** _old_plate;
bool** _locked_plate;
bool* _is_section_steady;
int* _above_threshold;
int _nthreads;
int _iterations;
bool _is_steady;
int _section_size;

pthread_t threadstructs[MAX_THREADS];
pthread_mutex_t _is_steady_lock;
pthread_barrier_t _barr;
const pthread_barrierattr_t *restrict _attr = 0;

float** createOldArray() {
	// printf("createOldArray\n");
	float** new_array = malloc(LENGTH * sizeof(float*));
	int i, j;
	for (i = 0; i < LENGTH; i++) {
		// printf("copy row\n");
		new_array[i] = malloc(LENGTH * sizeof(float));
		// memcpy(new_array[i], dbl_array[i], LENGTH);

		for (j = 0; j < LENGTH; j++) {
			// printf("copy col\n");
			new_array[i][j] = _plate[i][j];
		}
	}
	return new_array;
}

int getNumOverInPlate(float threshold) {
	int num = 0;
	int i, j;
	for (i = 0; i < LENGTH; i++) {
		for (j = 0; j < LENGTH; j++) {
			if (_plate[i][j] > threshold) {
				num++;
			}
		}
	}
	return num;
}

int getNumOverInSection(int tid, float threshold) {
	int num = 0;
	int i, row, col;
	for (i = 0; i < _section_size; i++) {
		row = (tid * _section_size) + i;
		for (col = 0; col < LENGTH; col++) {
			if (_plate[row][col] > threshold) {
				num++;
			}
		}
	}
	return num;
}

char isCellSteady(int row, int col) {
	float up = _plate[row-1][col];
	float left = _plate[row][col-1];
	float center = _plate[row][col];
	float right = _plate[row][col+1];
	float down = _plate[row+1][col];
	return fabs(center - (up + left + right + down) / 4.0f) < EPSILON;
}

void calcNewCellValue(int row, int col) {
	float up = _old_plate[row-1][col];
	float left = _old_plate[row][col-1];
	float center = _old_plate[row][col];
	float right = _old_plate[row][col+1];
	float down = _old_plate[row+1][col];
	float val = (up + left + right + down + (center * 4.0f)) / 8.0f;
	_plate[row][col] = val;
}

bool isSectionSteady(int tid) {
	int i, row, col;
	for (i = 0; i < _section_size; i++) {
		row = (tid * _section_size) + i;
		for (col = 0; col < LENGTH; col++) {
			if (!(_locked_plate[row][col]) && !isCellSteady(row, col)) {
				return false;
			}
		}
	}
	return true;
}

bool isPlateSteady() {
	bool is_steady = true;
	int i;
	for (i = 0; i < _nthreads; i++) {
		if (_is_section_steady[i]) {
			_is_section_steady[i] = false;
		}
		else {
			is_steady = false;
		}
	}
	return is_steady;
}

void advanceSection(int tid) {
	int i, row, col;
	for (i = 0; i < _section_size; i++) {
		row = (tid * _section_size) + i;
		for (col = 0; col < LENGTH; col++) {
			if (!(_locked_plate[row][col])) {
				calcNewCellValue(row, col);
			}
		}
	}
}

void swapPlates() {
	float** temp = _plate;
	_plate = _old_plate;
	_old_plate = temp;
}

void prepNextIteration() {
	_iterations++;
	_is_steady = isPlateSteady();
	if (!_is_steady) swapPlates();
}

void* createSteadyStateSection(void* arg) {
	int tid = (int)arg;
	printf("createSteadyStateSection in thread %d\n", tid);

	while (!_is_steady && _iterations < 500) {
		advanceSection(tid);
		_is_section_steady[tid] = isSectionSteady(tid);
		_above_threshold[tid] = getNumOverInSection(tid, THRESHOLD);

		pthread_barrier_wait(&_barr);
		if (!tid) prepNextIteration();
		pthread_barrier_wait(&_barr);
	}
	
	return 0;
}

void createSteadyState() {
	pthread_barrier_init(&_barr, _attr, _nthreads);
	int thread;

	for (thread = 0; thread < _nthreads; thread++) {
		pthread_create(&threadstructs[thread], 0, createSteadyStateSection, (void *)thread);
	}

	for (thread = 0; thread < _nthreads; thread++) {
		pthread_join(threadstructs[thread], 0);
	}
	
	pthread_barrier_destroy(&_barr);

	if (_iterations % 2 == 1) swapPlates();
	printf("Steps: %d\n", _iterations);
	printf("Over threshold: %d\n", getNumOverInPlate(THRESHOLD));
}

void printToFile(char* filename) {
	FILE* fp;
	fp = fopen(filename, "w");
	int i, j;
	for (i = 0; i < LENGTH; i++) {
		for (j = 0; j < LENGTH; j++) {
			fprintf(fp, "%lf, ", _plate[i][j]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

void printPlate() {
	int i, j;
	for (i = 0; i < LENGTH; i++) {
		for (j = 0; j < LENGTH; j++) {
			printf("%.1f, ", _plate[i][j]);
		}
		printf("\n");
	}
}

void initBarrierArrays() {
	int i;
	for (i = 0; i < _nthreads; i++) {
		_is_section_steady[i] = false;
		_above_threshold[i] = 0;
	}
}

void initLockedCells() {
	// printf("initLockedCells\n");
	int row, col = 0;
	// Every 20 rows = 100
	for(row = 0; row < LENGTH; row += 20) {
		for(col = 0; col < LENGTH; col++) {
			_plate[row][col] = 100;
			_locked_plate[row][col] = true;
		}
	}
	// Every 20 cols = 0
	for(row = 0; row < LENGTH; row++) {
		for(col = 0; col < LENGTH; col += 20) {
			_plate[row][col] = 0;
			_locked_plate[row][col] = true;
		}
		// Last col = 0
		_plate[row][LENGTH - 1] = 0;
		_locked_plate[row][LENGTH - 1] = true;
	}
	
	// Last row = 100
	for (col = 0; col < LENGTH; col++) {
		_plate[LENGTH - 1][col] = 100;
		_locked_plate[LENGTH - 1][col] = true;	
	}
}

void initRegularCells() {
	// printf("initRegularCells\n");
	int i, j;
	for (i = 0; i < LENGTH; i++) {
		_plate[i] = malloc(LENGTH * sizeof(float));
		_locked_plate[i] = malloc(LENGTH * sizeof(float));
		for (j = 0; j < LENGTH; j++) {
			_plate[i][j] = 50;
			_locked_plate[i][j] = false;
		}
	}
}

void cleanUpMemory() {
	int i;
	for (i = 0; i < LENGTH; i++) {
		free(_plate[i]);
		free(_locked_plate[i]);
		free(_old_plate[i]);
	}
	free(_plate);
	free(_locked_plate);
	free(_old_plate);
	free(_is_section_steady);
	free(_above_threshold);
}

void setUp() {
	// printf("setUp\n");
	_plate = malloc(LENGTH * sizeof(float*));
	_locked_plate = malloc(LENGTH * sizeof(char*));
	_is_section_steady = malloc(_nthreads * sizeof(bool));
	_above_threshold = malloc(_nthreads * sizeof(bool));
	_section_size = LENGTH / _nthreads;
	initRegularCells();
	initLockedCells();
	initBarrierArrays();
	_old_plate = createOldArray();
	// printf("init: %d\n", getNumOver());
}

int main(int argc, char* argv[]) {
	if (argc < 2) { printf("Include the number of threads you want as the first parameter"); return 0; }

	double time_b = getTime();
	_nthreads = atoi(argv[1]);

	// cout << plate.toString();
	setUp();
	// printPlate();
	createSteadyState();
	// printPlate();
	cleanUpMemory();
	// cout << plate.toString() << "\n";
	// printToFile("c.csv");

	double time_e = getTime();
	printf("Time: %lf seconds\n", time_e - time_b);
	return 0;
}














