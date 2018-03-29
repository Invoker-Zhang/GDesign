#include "ourhdr.h"

extern void displaySector(char*, unsigned);
extern void clearAll(char*);
extern void displayDBR(char*);
extern void displayFSINFO(char*);
extern void displayCluster(char*,unsigned);
extern void display_super_block(char*);

int main(int argc, char** argv){


	if(argc != 2) err_quit("Usage: ./displaySuperBlock device\n");


	display_super_block(argv[1]);

	return 0;
}
