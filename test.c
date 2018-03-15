#include "ourhdr.h"


int main(){
//	format("/dev/sdb1");
//	pre_allocation("/dev/sdb1");
	int fd = open("/dev/sdb1",O_RDWR);
	int buf[100];
	long k = 10000000;
	while(k--){
		for(int i = 0; i < 100; i++)
			buf[i] = i;
		circle_write(fd, buf, 100* sizeof(buf[0]));
	}
	return 0;
}
