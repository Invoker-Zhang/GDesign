#include "ourhdr.h"

void format(char* device ){
	int fd;
	uint64 sector_size = SECTOR_SIZE;
	uint64 sectors_per_cluster = SECTORS_PER_CLUSTER;
	uint64 total_size;
	uint64 total_sectors;
	uint64 reserved_sectors = RESERVED_SECTORS;
	uint64 fat_start;
	uint64 fat_sectors;
	uint64 data_start;
	uint64 data_sectors;
	uint64 cluster_number;
	char dummy_boot_code[] = "\x0e"	/* push cs */
    "\x1f"			/* pop ds */
    "\xbe\x5b\x7c"		/* mov si, offset message_txt */
    /* write_msg: */
    "\xac"			/* lodsb */
    "\x22\xc0"			/* and al, al */
    "\x74\x0b"			/* jz key_press */
    "\x56"			/* push si */
    "\xb4\x0e"			/* mov ah, 0eh */
    "\xbb\x07\x00"		/* mov bx, 0007h */
    "\xcd\x10"			/* int 10h */
    "\x5e"			/* pop si */
    "\xeb\xf0"			/* jmp write_msg */
    /* key_press: */
    "\x32\xe4"			/* xor ah, ah */
    "\xcd\x16"			/* int 16h */
    "\xcd\x19"			/* int 19h */
    "\xeb\xfe"			/* foo: jmp foo */
    /* message_txt: */
    "This is not a bootable disk.  Please insert a bootable floppy and\r\n"
    "press any key to try again ... \r\n";


	if( (fd = open(device, O_RDWR, FILE_MODE)) < 0)		err_quit("open error");

	disp(sector_size);
	if( (total_size = lseek(fd, 0, SEEK_END)) < 0 )		err_quit("lseek error");

	disp(total_size);
	total_sectors = total_size / sector_size;
	disp(total_sectors);
	disp16(total_sectors);

	fat_start		= reserved_sectors;
	fat_sectors		= 4 * (total_sectors - reserved_sectors)
		/ (4*FAT_NUMBER + sector_size*sectors_per_cluster);
	data_start		= fat_start + fat_sectors*FAT_NUMBER;
	data_sectors	= total_sectors - data_start -1;
	cluster_number	= data_sectors / SECTORS_PER_CLUSTER ;

	clearSectors(fd, 0, data_start);

	/* write DBR sector */
	DBR		DBR_sector = {0};

	DBR_sector.DBR_JmpCode[0] = 0xeb;
	DBR_sector.DBR_JmpCode[1] = 0x3c;
	DBR_sector.DBR_JmpCode[2] = 0x90;

	strncpy(DBR_sector.DBR_FacCode,"cefs    ",8);
	DBR_sector.DBR_BPB.BPB_BytePerSec = sector_size;
	DBR_sector.DBR_BPB.BPB_SecPerClus = SECTORS_PER_CLUSTER;
	DBR_sector.DBR_BPB.BPB_RsvdSecCnt = RESERVED_SECTORS;
	DBR_sector.DBR_BPB.BPB_NumFATs		= FAT_NUMBER;
	DBR_sector.DBR_BPB.BPB_Media		= 0xf8;
	DBR_sector.DBR_BPB.BPB_SecPerTrk	= 0x3f; 
	DBR_sector.DBR_BPB.BPB_NumHeads		= 0xff; 
	DBR_sector.DBR_BPB.BPB_HidSec		= 0x800; 
	DBR_sector.DBR_BPB.BPB_ToSec32		= total_sectors;
	DBR_sector.DBR_BPB.BPB_FATSz32		= fat_sectors;
	DBR_sector.DBR_BPB.BPB_Flags		= 0; 
	DBR_sector.DBR_BPB.BPB_FSVer		= 0; 
	DBR_sector.DBR_BPB.BPB_RootClus		= 2;
	DBR_sector.DBR_BPB.BPB_FSIfo		= 1;
	DBR_sector.DBR_BPB.BPB_BkBootSec	= 6;

	DBR_sector.DBR_BS.BS_DrvNum = 128;		//??
	DBR_sector.DBR_BS.BS_BootSig = 41;		//??
	DBR_sector.DBR_BS.BS_VolId = 671137093; //??
	strncpy(DBR_sector.DBR_BS.BS_VolSysType,"NO NAME    ",11);
	strncpy(DBR_sector.DBR_BS.BS_FilSysType,"FAT32   ",8);
	
	memcpy(DBR_sector.DBR_BootCode,dummy_boot_code,sizeof(dummy_boot_code));

	DBR_sector.DBR_BootSign[0]			= 0x55;
	DBR_sector.DBR_BootSign[1]			= 0xAa;
	
	lseek(fd, 0, SEEK_SET);
	write(fd, &DBR_sector, sizeof(DBR_sector) );
	lseek(fd, 6*sector_size, SEEK_SET);
	write(fd, &DBR_sector, sizeof(DBR_sector));

	// write fs information
	FSINFO	FSINFO_sector = {0};
	FSINFO_sector.FSINFO_Sym			= 0x41615252;
	FSINFO_sector.FSINFO_Used			= 0x61417272;
	FSINFO_sector.FSINFO_LastClus				= cluster_number;
	FSINFO_sector.FSINFO_SrchEnt		= 3;
	FSINFO_sector.FSINFO_EndSign		= 0xaa550000;
	
	if( lseek(fd, 1*sector_size, SEEK_SET) < 0 ) 
		err_sys("lseek error");
	if(write(fd, &FSINFO_sector, sizeof(FSINFO_sector)) < 0)
		err_sys("write error");
	lseek(fd, 7*sector_size, SEEK_SET);
	write(fd, &FSINFO_sector, sizeof(FSINFO_sector) );


	//write initial fat entries
	uint32	fat_entry = 0x0ffffff8;
	lseek(fd, fat_start * sector_size + 0 * 4,SEEK_SET);
	write(fd, &fat_entry, sizeof(fat_entry) );
	fat_entry		= 0x0fffffff;
	write(fd, &fat_entry, sizeof(fat_entry) );
	fat_entry		= 0x0fffffff;
	write(fd, &fat_entry, sizeof(fat_entry) );

	fat_entry = 0x0ffffff8;
	lseek(fd, (fat_start + fat_sectors)*sector_size,SEEK_SET);
	write(fd, &fat_entry, sizeof(fat_entry));
	fat_entry = 0x0fffffff;
	write(fd, &fat_entry, sizeof(fat_entry));
	fat_entry = 0x0fffffff;
	write(fd, &fat_entry, sizeof(fat_entry));
/*
	char fileName[12] = "video      ";
	time_t t;
	struct tm * curTime = NULL;
	time(&t);
	curTime = gmtime(&t);

	SHORT_FDT rootDir = {0};
	fillFDT(rootDir, fileName, 0x08, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, curTime->tm_sec/2 , curTime->tm_min, curTime->tm_hour, curTime->tm_mday, curTime->tm_mon, curTime->tm_year, 0);
	
	lseek(fd, data_start* sector_size, SEEK_SET);
	write(fd, &rootDir, sizeof(rootDir));
	
	*/

	close(fd);
	printf("format successed\n");

	//todel
	disp(fat_start);
	disp(fat_sectors);
	disp(data_start);
	disp(data_sectors);
	disp(cluster_number);
}


