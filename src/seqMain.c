#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "time.h"

#define EPSILON 0.1
#define THRESHOLD 50
#define LENGTH 24
#define true 1
#define false 0

float** _plate;
float** _old_plate;
char** _locked_plate;

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

int getNumOver() {
	int num = 0;
	int i, j;
	for (i = 0; i < LENGTH; i++) {
		for (j = 0; j < LENGTH; j++) {
			if (_plate[i][j] > THRESHOLD) {
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

bool isPlateSteady() {
	int i, j;
	for (i = 1; i < LENGTH - 1; i++) {
		for (j = 1; j < LENGTH - 1; j++) {
			if (!(_locked_plate[i][j]) && !isCellSteady(i, j)) {
				return false;
			}
		}
	}
	return true;
}

void swapPlates() {
	float** temp = _plate;
	_plate = _old_plate;
	_old_plate = temp;
}

void createSteadyState() {
	// printf("createSteadyState\n");
	int steps = 0;
	bool isSteady = false;
	while (!isSteady && steps < 500) {
		steps++;
		int i, j;
		for (i = 1; i < LENGTH - 1; i++) {
			for (j = 1; j < LENGTH - 1; j++) {
				calcNewCellValue(i, j);
			}
		}
		isSteady = isPlateSteady();
		swapPlates();
	}
	if (steps % 2 == 1) swapPlates();
	printf("Steps: %d\n", steps);
	printf("Over threshold: %d\n", getNumOver());
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
}

void setUp() {
	// printf("setUp\n");
	_plate = malloc(LENGTH * sizeof(float*));
	_locked_plate = malloc(LENGTH * sizeof(char*));
	initRegularCells();
	initLockedCells();
	_old_plate = createOldArray();
	// printf("init: %d\n", getNumOver());
	// cout << toString() << "\n";
}

int main(int argc, char* argv[]) {
	double time_b = getTime();

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
