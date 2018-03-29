#include "ourhdr.h"
#include "fat32.h"
#include "ext2.h"

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



void format_ext2(struct filsys_ext2 * e2fs){
	int fd;

	if ( (fd = open(e2fs->device, O_RDWR)) < 0)
		err_sys("open error in format");

	struct ext2_super_block super_block = {
		.s_inodes_count = e2fs->total_inode_count,
		.s_blocks_count = e2fs->block_count,
		.s_r_blocks_count = 0, 
		.s_free_blocks_count =  e2fs->block_count- e2fs->group_count* (3 + INOD_TBL_BLKS + e2fs->group_desc_blocks),
		.s_free_inodes_count = e2fs->total_inode_count,
		.s_first_data_block = (LOG_BLK_SZ ? 0 : 1),
		.s_log_block_size = LOG_BLK_SZ,
		.s_log_frag_size = LOG_BLK_SZ,
		.s_blocks_per_group = BLKS_PER_GRP,
		.s_frags_per_group = BLKS_PER_GRP,
		.s_inodes_per_group = INODS_PER_GRP,
		.s_mtime = 0,
		.s_wtime = time(0),
		.s_mnt_count = 0,
		.s_max_mnt_count = -1,
		.s_magic = EXT2_SUPER_MAGIC,
		.s_state = EXT2_FS_VALID,
		.s_errors = EXT2_ERRORS_DEFAULT,
		.s_min_rev_level = 0, // 
		.s_last_check = time(0),
		.s_check_interval = 0,
		.s_creator_os = 0, /* linux */
		.s_rev_level = 1,
		.s_def_resuid = 0,
		.s_def_resgid = 0
	};
	disp(sizeof(struct ext2_inode));
	disp(sizeof(struct ext2_group_desc));
	disp16(super_block.s_log_block_size);
	disp16(super_block.s_free_blocks_count);
	disp16(super_block.s_free_inodes_count);
	disp16(super_block.s_blocks_per_group);
	disp16(super_block.s_inodes_per_group);
	
	struct ext2_group_desc* pdesc; 
	pdesc = (struct ext2_group_desc*) malloc (sizeof(struct ext2_group_desc));
	if( !pdesc ) 
		err_sys("fail to malloc");
	memset(pdesc, 0, sizeof(struct ext2_group_desc));

	disp(3+INOD_TBL_BLKS+e2fs->group_desc_blocks);
	/* write fsinfo per group */
		
	
	for(int i = 0; i < e2fs->group_count; i++){
		clearSectors(fd, i*GRP_SZ / SEC_SZ, (e2fs->group_desc_blocks + 3 + INOD_TBL_BLKS)*BLK_SZ/SEC_SZ);
		/* write super block */
		if (i == 0){
			lseek(fd, 1024u, SEEK_SET);
		} else {
			lseek(fd, i * GRP_SZ, SEEK_SET);
		}
		write(fd, &super_block, sizeof(super_block));

		/* write group descritor table */
		lseek(fd, i * GRP_SZ + 1 * BLK_SZ, SEEK_SET);
		for(int j = 0; j < e2fs->group_count; j++){
			pdesc->bg_block_bitmap = 1 + e2fs->group_desc_blocks+ j * BLKS_PER_GRP;
			pdesc->bg_inode_bitmap = pdesc->bg_block_bitmap + 1;
			pdesc->bg_inode_table = pdesc->bg_inode_bitmap + 1;
			pdesc->bg_free_blocks_count = BLKS_PER_GRP - 3 - INOD_TBL_BLKS - e2fs->group_desc_blocks;
			pdesc->bg_free_inodes_count = INODS_PER_GRP;
			pdesc->bg_used_dirs_cnt = 0;
			write(fd, pdesc, sizeof(struct ext2_group_desc));
		}


		/* write block bitmap block */
		uint8_t val = 0xff;
		lseek(fd, i * GRP_SZ + (1 + e2fs->group_desc_blocks) * BLK_SZ, SEEK_SET);
		for(int k = 0; k < (3 + INOD_TBL_BLKS + e2fs->group_desc_blocks)/8; k++){
			write(fd, &val, 1);
		}
		val = (3 + INOD_TBL_BLKS + e2fs->group_desc_blocks) % 8;
		val = (1u << val) - 1;
		write(fd, &val, 8);
		

		/* write inode bitmap block */

		/* write inode table blocks */
	}
	free(pdesc);

	close(fd);
}


