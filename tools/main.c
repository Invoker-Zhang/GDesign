#include "ourhdr.h"

extern void displaySector(char*, unsigned);
extern void clearAll(char*);

int main(int argc, char** argv){


	if(argc != 2) err_quit("Usage: ./clearAll device \n");

	clearAll(argv[1]);

	return 0;
}
