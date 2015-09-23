#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <stdlib.h>

#include "time.h"

#define EPSILON 0.1
#define THRESHOLD 50
#define TRUE 1
#define FALSE 0

using namespace std;

float** copyDblArray(float** dbl_array, int length) {
	float** new_array = new float*[length];
	for (int i = 0; i < length; i++) {
		new_array[i] = new float[length];
		// memcpy(new_array[i], dbl_array[i], length);

		for (int j = 0; j < length; j++) {
			new_array[i][j] = dbl_array[i][j];
		}
	}
	return new_array;
}

string dblArrayToString(float** dbl_array, int length, string separater) {
	stringstream output;
	for (int i = 0; i < length; i++) {
		for (int j = 0; j < length; j++) {
			output << dbl_array[i][j] << separater;
		}
		output << "\n";
	}
	return output.str();
}

class Hotplate {
private:
	
	class SteadyStateCreater {
	private:
		Hotplate* _hotplate;
		float** _plate;
		float** _old_plate;
		int _length;
		int _steady_row;
		int _steady_col;

		int getNumOver() {
			int num = 0;
			for (int i = 0; i < _length; i++) {
				for (int j = 0; j < _length; j++) {
					if (_plate[i][j] > THRESHOLD) {
						num++;
					}
				}
			}
			return num;
		}

		bool isCellSteady(int row, int col) {
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

		bool isPlateSteady() {
			for (int i = _steady_row; i < _length-1; i++) {
				for (int j = _steady_col; j < _length-1; j++) {
					if (!(_hotplate->_locked_plate[i][j]) && !isCellSteady(i, j)) {
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

	public:
		SteadyStateCreater(Hotplate* hotplate) {
			_hotplate = hotplate;
			_plate = _hotplate->_plate;
			// cout << dblArrayToString(_old_plate, length, "\t") << "\n";
			_length = hotplate->_length;
			_old_plate = copyDblArray(_plate, _length);
			_steady_row = _steady_col = 1;
		}

		~SteadyStateCreater() {
			delete _old_plate;
		}

		void createSteadyState() {
			ofstream outfile;
			outfile.open("aboveThreshold.csv");
			int steps = 0;
			double time1, time2, time3;
			while (!isPlateSteady() && steps < 120) {
				steps++;
				for (int i = 1; i < _length-1; i++) {
					for (int j = 1; j < _length-1; j++) {
						if (!(_hotplate->_locked_plate[i][j])) {
							calcNewCellValue(i, j);
						}
					}
				}
				outfile << getNumOver() << ",\n";
				swapPlates();
			}
			if (steps % 2 == 1) swapPlates();
			cout << "Steps: " << steps << "\n";
			cout << "Over threshold: " << getNumOver() << "\n";
			cout << "Row: " << _steady_row << "\tCol: " << _steady_col << "\n";
		}
	};

protected:
	float** _plate;
	int _length;
	char** _locked_plate;

public:


	string toString(string separater) {
		return dblArrayToString(_plate, _length, separater);
	}

	string toString() {
		return toString("\t");
	}

	void printToFile(string fileName) {
		ofstream outfile;
		outfile.open(fileName.c_str());
		outfile << toString(", ");
		outfile.close();
	}

	void initLockedCells() {
		int row, col = 0;
		// Every 20 rows = 100
		for(row = 0; row < _length; row++) {
			if((row % 20) == 0) {
				for(col = 0; col < _length; col++) {
					_plate[row][col] = 100;
					_locked_plate[row][col] = TRUE;
				}
			}
		}
		// Every 20 cols = 0
		for(col = 0; col < _length; col++) {
			if((col % 20) == 0) {
				for(row = 0; row < _length; row++) {
					_plate[row][col] = 0;
					_locked_plate[row][col] = TRUE;
				}
			}
		}
	}

	void initRegularCells() {
		for (int i = 0; i < _length; i++) {
			float* row = new float[_length];
			_locked_plate[i] = new char[_length];
			for (int j = 0; j < _length; j++) {
				row[j] = 50;
				_locked_plate[i][j] = FALSE;
			}
			_plate[i] = row;
		}
	}

	Hotplate(int length) {
		_length = length;
		_plate = new float*[_length];
		_locked_plate = new char*[_length];

		initRegularCells();
		initLockedCells();
		// cout << toString() << "\n";
	}

	~Hotplate() {
		for (int i = 0; i < _length; i++) {
			delete _plate[i];
			delete _locked_plate[i];
		}
		delete _plate;
		delete _locked_plate;
	}

	void createSteadyState() {
		SteadyStateCreater creater(this);
		creater.createSteadyState();
	}


	


};
