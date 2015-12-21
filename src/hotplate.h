#ifndef kjr_hotplate
#define kjr_hotplate

#include <stdbool.h>

typedef struct {
	int size;
	float** curr_plate;
	float** prev_plate;
	bool** lock_plate;
} hotplate;

void printPlate(float** plate, int size) {
	int i, j;
	printf("\naddress: %p\n", plate);
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			printf("%.1f, ", plate[i][j]);
		}
		printf("\n");
	}
}

void hotplate_initRegular(float** curr_plate, float** prev_plate, bool** lock_plate, int size) {
	// printf("initRegularCells\n");
	int i, j;
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			curr_plate[i][j] = 50;
			prev_plate[i][j] = 50;
			lock_plate[i][j] = false;
		}
	}
}

void hotplate_initLocked(float** curr_plate, float** prev_plate, bool** lock_plate, int size) {
	// printf("initLockedCells\n");
	int row, col = 0;
	// Every 20 rows = 100
	for(row = 0; row < size; row += 20) {
		for(col = 0; col < size; col++) {
			curr_plate[row][col] = 100;
			prev_plate[row][col] = 100;
			lock_plate[row][col] = true;
		}
	}
	// Every 20 cols = 0
	for(row = 0; row < size; row++) {
		for(col = 0; col < size; col += 20) {
			curr_plate[row][col] = 0;
			prev_plate[row][col] = 0;
			lock_plate[row][col] = true;
		}
		// Last col = 0
		curr_plate[row][size - 1] = 0;
		prev_plate[row][size - 1] = 0;
		lock_plate[row][size - 1] = true;
	}
	
	// Last row = 100
	for (col = 0; col < size; col++) {
		curr_plate[size - 1][col] = 100;
		prev_plate[size - 1][col] = 100;
		lock_plate[size - 1][col] = true;	
	}
}

void initSubArrays(float** curr_plate, float** prev_plate, bool** lock_plate, int size) {
	for (int i = 0; i < size; i++) {
		curr_plate[i] = malloc(size * sizeof(float));
		prev_plate[i] = malloc(size * sizeof(float));
		lock_plate[i] = malloc(size * sizeof(bool));
	}
}

void initHotplate(hotplate* plate) {
	int size = plate->size;
	float** curr_plate = malloc(size * sizeof(float*));
	float** prev_plate = malloc(size * sizeof(float*));
	bool** lock_plate = malloc(size * sizeof(bool*));

	// initialize sub-arrays
	initSubArrays(curr_plate, prev_plate, lock_plate, size);
	// initialize regular cells
	hotplate_initRegular(curr_plate, prev_plate, lock_plate, size);
	// initialize locked cells
	hotplate_initLocked(curr_plate, prev_plate, lock_plate, size);

	plate->curr_plate = curr_plate;
	plate->prev_plate = prev_plate;
	plate->lock_plate = lock_plate;
	
	return;
}

void freeHotplate(hotplate* plate) {
	int size = plate->size;

	for (int i = 0; i < size; i++) {
		free(plate->curr_plate[i]);
		free(plate->prev_plate[i]);
		free(plate->lock_plate[i]);
	}

	free(plate->curr_plate);
	free(plate->prev_plate);
	free(plate->lock_plate);

	return;
}



#endif

















