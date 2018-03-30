#include <assert.h>
#include "ourhdr.h"
#include "fat32.h"

/* format the device.
 * specifications and define in ourhdr.h. */

void format_fat32(struct filsys_fat32* fs_fat){
	int fd;
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

	if( (fd = open(fs_fat->device, O_RDWR)) < 0)		err_quit("open error");

	/* clear the reserved sectors and fat sectors */

	clearSectors(fd, 0, fs_fat->data_start);

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
	DBR_sector.DBR_BPB.BPB_ToSec32		= fs_fat->tot_secs;
	DBR_sector.DBR_BPB.BPB_FATSz32		= fs_fat->fat_secs;
	DBR_sector.DBR_BPB.BPB_Flags		= 0; 
	DBR_sector.DBR_BPB.BPB_FSVer		= 0; 
	DBR_sector.DBR_BPB.BPB_RootClus		= ROOT_CLUS_NUM;
	DBR_sector.DBR_BPB.BPB_FSIfo		= FS_INFO_SEC;
	DBR_sector.DBR_BPB.BPB_BkBootSec	= BOOT_BK_SEC;

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
	lseek(fd, BOOT_BK_SEC*SEC_SZ, SEEK_SET);
	write(fd, &DBR_sector, sizeof(DBR_sector));

	/* write fs information in fsinfo sector */

	FSINFO	FSINFO_sector = {0};
	FSINFO_sector.FSINFO_Sym			= 0x41615252;
	FSINFO_sector.FSINFO_Used			= 0x61417272;
	FSINFO_sector.FSINFO_LastClus		= fs_fat->clus_num;
	FSINFO_sector.FSINFO_SrchEnt		= 3;
	FSINFO_sector.FSINFO_EndSign		= 0xaa550000;
	
	if( lseek(fd, FS_INFO_SEC *SEC_SZ, SEEK_SET) < 0 ) 
		err_sys("lseek error");
	if(write(fd, &FSINFO_sector, sizeof(FSINFO_sector)) < 0)
		err_sys("write error");
	lseek(fd, FS_INFO_BK_SEC*SEC_SZ, SEEK_SET);
	write(fd, &FSINFO_sector, sizeof(FSINFO_sector) );

	/* write initial fat entries */

	uint32_t	fat_entry = 0x0ffffff8;
	lseek(fd, FAT_START * SEC_SZ ,SEEK_SET);
	write(fd, &fat_entry, sizeof(fat_entry) );
	fat_entry		= FAT_ENT_END;
	write(fd, &fat_entry, sizeof(fat_entry) );
	fat_entry		= FAT_ENT_END;
	write(fd, &fat_entry, sizeof(fat_entry) );

	/* backup area */

	fat_entry = 0x0ffffff8;
	lseek(fd, (FAT_START + fs_fat->fat_secs)*SEC_SZ,SEEK_SET);
	write(fd, &fat_entry, sizeof(fat_entry));
	fat_entry = FAT_ENT_END;
	write(fd, &fat_entry, sizeof(fat_entry));
	fat_entry = FAT_ENT_END;
	write(fd, &fat_entry, sizeof(fat_entry));

	close(fd);
	printf("format successed\n");

	/* todel */
	disp(FAT_START);
	disp(fs_fat->fat_secs);
	disp(fs_fat->data_start);
	disp(fs_fat->data_secs);
	disp(fs_fat->clus_num);
}


