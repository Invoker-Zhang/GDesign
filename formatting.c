#include "ourhdr.h"

/* format the device.
 * specifications and define in ourhdr.h. */

void format(char* device ){
	int fd;
	uint64_t total_size;
	uint64_t total_sectors;
	uint64_t fat_sectors;
	uint64_t data_start;
	uint64_t data_sectors;
	uint64_t cluster_number;

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


	if( (fd = open(device, O_RDWR)) < 0)		err_quit("open error");

	/* compute parameters about the disk */
	if( (total_size = lseek(fd, 0, SEEK_END)) < 0 )		err_quit("lseek error");

	disp(total_size);
	total_sectors = total_size / SEC_SZ;
	disp(total_sectors);
	disp16(total_sectors);

	fat_sectors		= 4 * (total_sectors - RSVD_SECS)
		/ (4*FAT_NUM + SEC_SZ*SECS_PER_CLUS);
	data_start		= FAT_START + fat_sectors*FAT_NUM;
	data_sectors	= total_sectors - data_start -1;
	cluster_number	= data_sectors / SECS_PER_CLUS ;

	/* clear the reserved sectors and fat sectors */

	clearSectors(fd, 0, data_start);

	/* write DBR sector */

	DBR		DBR_sector = {0};

	DBR_sector.DBR_JmpCode[0] = 0xeb;
	DBR_sector.DBR_JmpCode[1] = 0x3c;
	DBR_sector.DBR_JmpCode[2] = 0x90;

	strncpy(DBR_sector.DBR_FacCode,"cefs    ",8);
	DBR_sector.DBR_BPB.BPB_BytsPerSec = SEC_SZ;
	DBR_sector.DBR_BPB.BPB_SecPerClus = SECS_PER_CLUS;
	DBR_sector.DBR_BPB.BPB_RsvdSecCnt = RSVD_SECS;
	DBR_sector.DBR_BPB.BPB_NumFATs		= FAT_NUM;
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

	DBR_sector.DBR_BS.BS_DrvNum = 0x80;		
	DBR_sector.DBR_BS.BS_BootSig = 0x29;		
	DBR_sector.DBR_BS.BS_VolId = 671137093;		/* no meaning */
	strncpy(DBR_sector.DBR_BS.BS_VolSysType,"NO NAME    ",11);
	strncpy(DBR_sector.DBR_BS.BS_FilSysType,"FAT32   ",8);
	
	memcpy(DBR_sector.DBR_BootCode,dummy_boot_code,sizeof(dummy_boot_code));

	DBR_sector.DBR_BootSign[0]			= 0x55;
	DBR_sector.DBR_BootSign[1]			= 0xAa;
	
	lseek(fd, 0, SEEK_SET);
	write(fd, &DBR_sector, sizeof(DBR_sector) );
	lseek(fd, 6*SEC_SZ, SEEK_SET);
	write(fd, &DBR_sector, sizeof(DBR_sector));

	/* write fs information in fsinfo sector */

	FSINFO	FSINFO_sector = {0};
	FSINFO_sector.FSINFO_Sym			= 0x41615252;
	FSINFO_sector.FSINFO_Used			= 0x61417272;
	FSINFO_sector.FSINFO_LastClus				= cluster_number;
	FSINFO_sector.FSINFO_SrchEnt		= 3;
	FSINFO_sector.FSINFO_EndSign		= 0xaa550000;
	
	if( lseek(fd, 1*SEC_SZ, SEEK_SET) < 0 ) 
		err_sys("lseek error");
	if(write(fd, &FSINFO_sector, sizeof(FSINFO_sector)) < 0)
		err_sys("write error");
	lseek(fd, 7*SEC_SZ, SEEK_SET);
	write(fd, &FSINFO_sector, sizeof(FSINFO_sector) );


	/* write initial fat entries */

	uint32_t	fat_entry = 0x0ffffff8;
	lseek(fd, FAT_START * SEC_SZ ,SEEK_SET);
	write(fd, &fat_entry, sizeof(fat_entry) );
	fat_entry		= 0x0fffffff;
	write(fd, &fat_entry, sizeof(fat_entry) );
	fat_entry		= 0x0fffffff;
	write(fd, &fat_entry, sizeof(fat_entry) );

	/* backup area */

	fat_entry = 0x0ffffff8;
	lseek(fd, (FAT_START + fat_sectors)*SEC_SZ,SEEK_SET);
	write(fd, &fat_entry, sizeof(fat_entry));
	fat_entry = 0x0fffffff;
	write(fd, &fat_entry, sizeof(fat_entry));
	fat_entry = 0x0fffffff;
	write(fd, &fat_entry, sizeof(fat_entry));

	close(fd);
	printf("format successed\n");

	/* todel */
	disp(FAT_START);
	disp(fat_sectors);
	disp(data_start);
	disp(data_sectors);
	disp(cluster_number);
}



