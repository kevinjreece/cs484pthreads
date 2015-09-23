#include <iostream>
#include <iomanip>

#include "Hotplate.h"
#include "time.h"

#define LENGTH 16384

using namespace std;

int main(int argc, char* argv[]) {
	double time_b = getTime();

	Hotplate plate = Hotplate(LENGTH);
	plate.createSteadyState();
	double time_e = getTime();
	printf("Time: %lf seconds\n", time_e - time_b);
	if (argc > 1 && !strcmp(argv[1],"-p")) plate.printToFile("cpp.csv");
	return 0;
}
















