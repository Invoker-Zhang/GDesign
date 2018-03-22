run:a.out
	sudo ./a.out
a.out:ourhdr.h fat32.h formatting.c pre_allocation.c test.c libs.c write.c 
	gcc formatting.c pre_allocation.c test.c libs.c write.c
