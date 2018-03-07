#include "ourhdr.h"

extern void displaySector(char*, unsigned);
extern void clearAll(char*);
extern void displayDBR(char*);
extern void displayFSINFO(char*);
extern void displayCluster(char*,unsigned);

int main(int argc, char** argv){


	if(argc != 3) err_quit("Usage: ./displayCluster device offset\n");

	int off = atoi(argv[2]);

	displayCluster(argv[1],off);

	return 0;
}
