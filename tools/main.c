#include "ourhdr.h"

extern void displaySector(char*, unsigned);

int main(int argc, char** argv){

	int index = 0;

	if(argc != 3) err_quit("Usage: ./displaySector device sectorNo\n");
	index = atoi(argv[2]);

	displaySector(argv[1],index);

	return 0;
}
