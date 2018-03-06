#include "ourhdr.h"

extern void displaySector(char*, unsigned);
extern void clearAll(char*);
extern void displayDBR(char*);
extern void displayFSINFO(char*);

int main(int argc, char** argv){


	if(argc != 2) err_quit("Usage: ./displayFSINFO device \n");

	displayFSINFO(argv[1]);

	return 0;
}
