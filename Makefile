run:a.out 
	sudo ./a.out /dev/sdc1
a.out:ourhdr.h fat32.h formatting.c pre_allocation.c test.c libs.c write.c fat32.c
	gcc formatting.c pre_allocation.c test.c libs.c write.c fat32.c
