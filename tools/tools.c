#include "ourhdr.h"

void displaySector(char* device, unsigned offset){
	int fd;
	unsigned char x = 0;

	if( (fd = open(device, O_RDONLY)) < 0)	err_sys("open error");
	if( lseek(fd, offset * 512, SEEK_SET) < 0)	err_sys("lseek error");

	for(int i = 0; i < 32; i++){

		for(int j = 0 ; j < 16; j++){
			if( read(fd, &x, sizeof(x)) < 0)	err_sys("read error");
			printf("%2x ",x);
		}

		printf("\n");
	}

	close(fd);
}

#define BUF_SIZE	4096
void clearAll(char *device){
	int fd;
	int writeNum;

	if( (fd = open(device, O_WRONLY)) < 0)	err_sys("open error");

	unsigned long long end = 0;
	end = lseek(fd, 0, SEEK_END);

	unsigned long	blockNum = (end - 1) / BUF_SIZE + 1;
	
	char buf[BUF_SIZE] = {0};
	lseek(fd, 0, SEEK_SET);
	for(int i = 0; i < blockNum; i++){
		write(fd, buf, BUF_SIZE);
		fprintf(stderr,"%d%% completed\n",100 * i / blockNum);
	}

	close(fd);
}

void displayDBR(char* device){
	DBR		DBR_sector = {0};
	int fd;
	if( (fd = open(device, O_RDONLY)) <0) 
		err_sys("open error");
	if( read(fd, &DBR_sector, sizeof(DBR_sector)) < 0)
		err_sys("read error");
	disp(DBR_sector.DBR_BPB.BPB_BytePerSec);
	disp(DBR_sector.DBR_BPB.BPB_SecPerClus);
	disp(DBR_sector.DBR_BPB.BPB_RsvdSecCnt);
	disp(DBR_sector.DBR_BPB.BPB_NumFATs);
	disp(DBR_sector.DBR_BPB.BPB_RootEntCnt);
	disp(DBR_sector.DBR_BPB.BPB_ToSec16);
	disp(DBR_sector.DBR_BPB.BPB_Media);
	disp(DBR_sector.DBR_BPB.BPB_FATSz16);
	disp(DBR_sector.DBR_BPB.BPB_SecPerTrk);
	disp(DBR_sector.DBR_BPB.BPB_NumHeads);
	disp(DBR_sector.DBR_BPB.BPB_HidSec);
	disp(DBR_sector.DBR_BPB.BPB_ToSec32);
	disp(DBR_sector.DBR_BPB.BPB_FATSz32);
	disp(DBR_sector.DBR_BPB.BPB_Flags);
	disp(DBR_sector.DBR_BPB.BPB_FSVer);
	disp(DBR_sector.DBR_BPB.BPB_RootClus);
	disp(DBR_sector.DBR_BPB.BPB_FSIfo);
	disp(DBR_sector.DBR_BPB.BPB_BkBootSec);

	disp(DBR_sector.DBR_BS.BS_DrvNum);
	disp(DBR_sector.DBR_BS.BS_BootSig);
	disp(DBR_sector.DBR_BS.BS_VolId);
	for(int i = 0; i < 11; i++){
		printf("%c",DBR_sector.DBR_BS.BS_VolSysType[i]);
	}
	printf("\n");
	for(int i = 0; i < 8; i++){
		printf("%c",DBR_sector.DBR_BS.BS_FilSysType[i]);
	}
	printf("\n");
	

}
void displayFSINFO(char* device){
	DBR		DBR_sector = {0};
	int fd;
	if( (fd = open(device, O_RDONLY)) < 0)
		err_sys("open error");
	if( read(fd, &DBR_sector, sizeof(DBR_sector)) < 0)
		err_sys("read error");
	int fsInfoSecNum = DBR_sector.DBR_BPB.BPB_FSIfo;
	lseek( fd, fsInfoSecNum*512,SEEK_SET);
	FSINFO FSINFO_sector = {0};
	if( read(fd, &FSINFO_sector, sizeof(FSINFO_sector)) < 0)
		err_sys("read error");
	disp(FSINFO_sector.FSINFO_Sym);
	disp(FSINFO_sector.FSINFO_Used);
	disp(FSINFO_sector.FSINFO_LastClus);
	disp(FSINFO_sector.FSINFO_SrchEnt);
	disp16(FSINFO_sector.FSINFO_Sym);
	disp16(FSINFO_sector.FSINFO_Used);
	disp16(FSINFO_sector.FSINFO_LastClus);
	disp16(FSINFO_sector.FSINFO_SrchEnt);

}
