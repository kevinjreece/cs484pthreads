#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "time.h"

#define EPSILON 0.1
#define THRESHOLD 50
#define LENGTH 4096
#define true 1
#define false 0

float** _plate;
float** _old_plate;
char** _locked_plate;
int _length;
int _steady_row;
int _steady_col;

float** copyDblArray(float** dbl_array, int length) {
	float** new_array = (float**)malloc(length * sizeof(float**));
	int i, j;
	// #pragma omp parallel for private(i, j);
	for (i = 0; i < length; i++) {
		new_array[i] = (float*)malloc(length * sizeof(float*));
		// memcpy(new_array[i], dbl_array[i], length);

		for (j = 0; j < length; j++) {
			new_array[i][j] = dbl_array[i][j];
		}
	}
	return new_array;
}

int getNumOver() {
	int num = 0;
	// #pragma omp parallel for private(i, j)
	int i, j;
	for (i = 0; i < _length; i++) {
		for (j = 0; j < _length; j++) {
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
	return fabs(center - (up + left + right + down) / 4) < EPSILON;
}

void calcNewCellValue(int row, int col) {
	float up = _old_plate[row-1][col];
	float left = _old_plate[row][col-1];
	float center = _old_plate[row][col];
	float right = _old_plate[row][col+1];
	float down = _old_plate[row+1][col];
	float val = (up + left + right + down + (center*4)) / 8;
	_plate[row][col] = val;
	// cout << (int)val << "\t";
}

char isPlateSteady() {
	int i, j;
	for (i = _steady_row; i < _length-1; i++) {
		for (j = _steady_col; j < _length-1; j++) {
			if (!(_locked_plate[i][j]) && !isCellSteady(i, j)) {
				_steady_row = i;
				_steady_col = j;
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

void initLockedCells(float** dbl_array) {
	int i;
	// Row 400 columns 0 through 330 are fixed at 100 degrees
	for (i = 0; i <= 330; i++) {
		dbl_array[400][i] = 100;
	}
	// A cell at row 200, column 500 also is fixed at 100 degrees
	dbl_array[200][500] = 100;
}

void createSteadyState() {
	int steps = 0;
	while (!isPlateSteady() && steps < 500) {
		steps++;
		int i, j;
		// #pragma omp parallel for private(i, j)
		for (i = 1; i < _length-1; i++) {
			for (j = 1; j < _length-1; j++) {
				calcNewCellValue(i, j);
			}
		}
		initLockedCells(_plate);
		swapPlates();
	}
	if (steps % 2 == 1) swapPlates();
	printf("Steps: %d\n", steps);
	printf("Over threshold: %d\n", getNumOver());
	printf("Row: %d\tCol: %d\n", _steady_row, _steady_col);
}

void printToFile(char* filename) {
	FILE* fp;
	fp = fopen(filename, "w");
	int i, j;
	for (i = 0; i < _length; i++) {
		for (j = 0; j < _length; j++) {
			fprintf(fp, "%lf, ", _plate[i][j]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

void initLockedCellsArray() {
	_locked_plate = (char**)malloc(_length * sizeof(char**));
	int i, j;
	// #pragma omp parallel for private(i, j)
	for (i = 0; i < _length; i++) {
		_locked_plate[i] = (char*)malloc(_length * sizeof(char*));
		for (j = 0; j < _length; j++) {
			_locked_plate[i][j] = 0;
		}
	}

	// Row 400 columns 0 through 330 are fixed at 100 degrees
	// #pragma omp parallel for private(i)
	for (i = 0; i <= 330; i++) {
		_locked_plate[400][i] = 1;
	}
	// A cell at row 200, column 500 also is fixed at 100 degrees
	_locked_plate[200][500] = 1;
}

void setUp() {
	_length = LENGTH;
	_plate = (float**)malloc(_length * sizeof(float**));
	initLockedCellsArray();
	_steady_row = _steady_col = 1;
	int i, j;
	// #pragma omp parallel for private(i, j)
	for (i = 0; i < _length; i++) {
		float* row = (float*)malloc(_length * sizeof(float*));
		for (j = 0; j < _length; j++) {
			float temp;
			if (i == _length - 1) {
				temp = 100;
			}
			else if (i == 0 || j == 0 || j == _length - 1) {
				temp = 0;
			}
			else {
				temp = 50;
			}
			row[j] = temp;
		}
		_plate[i] = row;
	}
	initLockedCells(_plate);
	// printf("init: %d\n", getNumOver());
	_old_plate = copyDblArray(_plate, _length);
	// cout << toString() << "\n";
}

void cleanUpMemory() {
	int i;
	for (i = 0; i < _length; i++) {
		free(_plate[i]);
	}
	free(_plate);
	free(_locked_plate);
	free(_old_plate);
}

int main(int argc, char* argv[]) {
	double time_b = getTime();

	// cout << plate.toString();
	setUp();
	createSteadyState();
	// cleanUpMemory();
	// cout << plate.toString() << "\n";
	// printToFile("c.csv");

	double time_e = getTime();
	printf("Time: %lf seconds\n", time_e - time_b);
	return 0;
}
