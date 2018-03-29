#include "ourhdr.h"
#include "ext2.h"
#include "fat32.h"

/*
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

void displayCluster(char* device, unsigned offset){
	int fd;
	DBR		DBR_sector;

	if( (fd = open(device, O_RDONLY)) < 0)	err_sys("open error");
	if( read(fd, &DBR_sector, sizeof(DBR)) < 0)		err_sys("read error");

	uint32_t sector_size = DBR_sector.DBR_BPB.BPB_BytePerSec;
	uint32_t sectors_per_cluster = DBR_sector.DBR_BPB.BPB_SecPerClus;
	uint32_t start_data_sector = DBR_sector.DBR_BPB.BPB_RsvdSecCnt + DBR_sector.DBR_BPB.BPB_NumFATs * DBR_sector.DBR_BPB.BPB_FATSz32;
	
	lseek(fd, sector_size * (start_data_sector + offset * sectors_per_cluster), SEEK_SET);

	uint8_t buf[sector_size];
	if( read(fd, buf, sector_size) < 0) err_sys("read error");

	for(int i = 0; i < 32;  i++){

		for(int j = 0; j < 16; j++){
			printf("%2x ",buf[i*16 + j]);
		}
		for(int j = 0; j < 16; j++){
			printf("%c",buf[i*16+j]);
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
		fprintf(stderr,"%.2f%% completed\r",100.0 * i / blockNum);
	}
	printf("\nfinished\n");

	close(fd);
}

void displayDBR(char* device){
	DBR		DBR_sector = {0};
	int fd;
	if( (fd = open(device, O_RDONLY)) <0) 
		err_sys("open error");
	if( read(fd, &DBR_sector, sizeof(DBR_sector)) < 0)
		err_sys("read error");
	for(int i = 0; i < 8; i++){
		printf("%c",DBR_sector.DBR_FacCode[i]);
	}
	printf("\n");
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
*/


void display_super_block(char* device){
	struct ext2_super_block super;
	int fd;

	if( (fd = open(device, O_RDONLY)) <0)
		err_sys("open error");
	lseek(fd, 1024, SEEK_SET);
	if (read(fd, &super, sizeof(super)) < 0)
		err_sys("read error");
	disp(super.s_inodes_count);
	disp(super.s_blocks_count);
	disp(super.s_r_blocks_count);
	disp(super.s_free_blocks_count);
	disp(super.s_free_inodes_count);
	disp(super.s_first_data_block);
	disp(super.s_log_block_size);
	disp(super.s_log_cluster_size);
	disp(super.s_blocks_per_group);
	disp(super.s_clusters_per_group);
	disp(super.s_inodes_per_group);
	disp(super.s_mtime);
	disp(super.s_wtime);
	disp(super.s_mnt_count);
	disp(super.s_max_mnt_count);
	disp16(super.s_magic);
	disp(super.s_state);
	disp(super.s_errors);
	disp(super.s_min_rev_level);
	disp(super.s_last_check);
	disp(super.s_check_interval);
	disp(super.s_creator_os);
	disp(super.s_rev_level);
	disp(super.s_def_resuid);
	disp(super.s_def_resgid);

	close(fd);
}
