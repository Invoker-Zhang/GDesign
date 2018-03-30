#ifndef __ext2_h
#define __ext2_h

#include "ourhdr.h"

#define EXT2_SUPER_MAGIC		0XEF53

/* File system states */
#define	EXT2_FS_VALID		0X0001		/* Unmount cleanly */
#define EXT2_FS_ERROR		0X0002		/* Errors detected */
#define EXT2_FS_ORPHAN		0X0004		/* Orphans being recoverd */

/* actions when error occurs */
#define EXT2_ERRORS_CONTINUE	1 /* Continue execution */
#define EXT2_ERRORS_RO			2 /* Remount fs read-only */
#define EXT2_ERRORS_PANIC		3 /* Panic */
#define EXT2_ERRORS_DEFAULT		EXT2_ERRORS_CONTINUE  

/*  creator os */
#define EXT2_OS_LINUX			0
#define EXT2_OS_HURD			1
#define EXT2_OS_MASIX			2
#define EXT2_OS_FREEBSD			3
#define EXT2_OS_LITES			4

/* revision level value */
#define EXT2_GOOD_OLD_REV		0	/* Revision 0 */
#define EXT2_DYNAMIC_REV		1	/* Revision 1 with variable inode size,
									   extended attributes, etc. */

/* default resuid */
#define EXT2_DEF_RESUID			0
#define EXT2_DEF_RESGID			0

/* inode size */
#define EXT2_GOOD_OLD_INODE_SIZE	128

/* constants relative to data blocks, used in inode.i_block[] */
#define EXT2_NDIR_BLOCKS	12
#define EXT2_IND_BLOCK		EXT2_NDIR_BLOCKS
#define EXT2_DIND_BLOCK		(EXT2_IND_BLOCK + 1)
#define EXT2_TIND_BLOCK		(EXT2_DIND_BLOCK + 1)
#define EXT2_N_BLOCKS		(EXT2_TIND_BLOCK + 1)

struct ext2_super_block{
	uint32_t s_inodes_count;	/* inode number of filsys */
	uint32_t s_blocks_count;	/* block number of filsys */
	uint32_t s_r_blocks_count;	/* number of reserved blocks(for super user) */
	uint32_t s_free_blocks_count;	/* number of free blocks(not used and reserved) */
	uint32_t s_free_inodes_count;	/* number of free inodes */
	uint32_t s_first_data_block;	/* first data block */
	uint32_t s_log_block_size;	/* block size : 1024 * 2^# */
	uint32_t s_log_frag_size;	/* fragment size : 1024 * 2^#,this feature isn't used. 
								   Keep it to equal to  block size */
	uint32_t s_blocks_per_group;	/* blocks per group */
	uint32_t s_frags_per_group;	/* fragments per group */
	uint32_t s_inodes_per_group;	/* inodes per group */
	uint32_t s_mtime;			/* mount time */
	uint32_t s_wtime;			/* write time */
	uint16_t s_mnt_count;		/* Mount count */
	uint16_t s_max_mnt_count;	/* Maximal mount count */
	uint16_t s_magic;			/* Magic signature */
	uint16_t s_state;			/* File system state */
	uint16_t s_errors;			/* Behaviours when detecting errors */
	uint16_t s_min_rev_level;	/* minor revision level */
	uint32_t s_last_check;		/* time of last check */
	uint32_t s_check_interval;	/* max. time between checks */
	uint32_t s_creator_os;		/* Os */
	uint32_t s_rev_level;		/* revision level */
	uint16_t s_def_resuid;		/* Default uid for rest blocks */
	uint16_t s_def_resgid;		/* Default gid for rest blocks */
#if 0
	/* 
	 * These fields are for EXT2_DYNAMIC_REV superblock only.*/

	uint32_t s_first_inode;		/* First Non-reserved inode */
	uint16_t s_inode_size;		/* size of inode structure */
	uint16_t s_block_group_nr;	/* block group # of this superblock */
	uint32_t s_feature_compat;	/* compatible feature set */
	uint32_t s_feature_incompat;	/* incompatible feature set */
	uint8_t	 s_uuid[16];		/* 128-bit uuid for volumn */
	char	s_volumn_name[16];	/* volumn name */
	char	s_last_mounted;		/* directory where last mounted */
	uint32_t s_algorithm_usage_bitmap;	/* For compression */
	/* Performance hints */
	uint8_t s_prealloc_blocks;	/* Nr of blocks to try to preallocate */
	uint8_t s_prealloc_dir_blocks;	/* Nr of blocks to preallocate for dirs */
	uint16_t s_reserved_gdt_blocks;	/* per group table for online growth */
	/* Journaling support */
	uint8_t	s_journal_uuid[16];		/* uuid of journal superblock */
	uint32_t s_journal_inum;	/* inode nummber of journal file */
	uint32_t s_journal_dev;		/* device number of journal file */
	uint32_t s_last_orphan;		/* start of list of inodes to delete */
	uint32_t s_hash_seed[4];		/* HTREE hash seed */
	uint8_t s_def_hash_vision;	/* default hash vision to use */
	uint8_t s_jnl_backup_type;	/* default type of journal backup */
	uint16_t s_desc_size;		/* group desc. size: INCOMPAT 64-bit */
	uint32_t s_default_mnt_opts;	
	uint32_t s_first_meta_bg;	/* first metablock group */
	uint32_t s_mkfs_time;		/* when the filesystem was created */
	uint32_t s_jnl_blocks[17];	/* backup of the journal inodes */
	uint32_t s_block_count_hi;	/* block count high 32 bits */
	uint32_t s_r_block_count_hi;	/* reserved block count high 32 bits */
	uint32_t s_free_block_hi;	/* free block count */
	uint16_t s_min_extra_isize; /* all inodes have at least # bytes */
	uint16_t s_want_extra_isize;	/* New inodes should reserve # bytes */
	uint32_t s_flags;			/* miscellaneous flag */
	uint16_t s_raid_stride;		/* RAID stride */
	uint16_t s_mmp_update_interval;  /* # seconds to wait in MMP checking */
	uint64_t s_mmp_block;            /* Block for multi-mount protection */
	uint32_t s_raid_stripe_width;    /* blocks on all data disks (N*stride)*/
	uint8_t s_log_groups_per_flex;	/* FLEX_BG group size */
	uint8_t s_reserved_char_pad;
	uint16_t s_reserved_pad;		/* Padding to next 32bits */
	uint64_t s_kbytes_written;	/* nr of lifetime kilobytes written */
	uint32_t s_snapshot_inum;	/* Inode number of active snapshot */
	uint32_t s_snapshot_id;		/* sequential ID of active snapshot */
	uint64_t s_snapshot_r_blocks_count; /* reserved blocks for active
					      snapshot's future use */
	uint32_t s_snapshot_list;	/* inode number of the head of the on-disk snapshot list */
#endif

};

/* Structure of a blocks group descriptor */

struct ext2_group_desc{
	uint32_t	bg_block_bitmap;	/* Blocks bitmap block */
	uint32_t	bg_inode_bitmap;	/* Inodes bitmap block */
	uint32_t	bg_inode_table;		/* Inodes table block */
	uint16_t	bg_free_blocks_count;	/* Free block count */
	uint16_t	bg_free_inodes_count;	/* Free inode count */
	uint16_t	bg_used_dirs_cnt;	/* Directories count */
	uint16_t	bg_pad;
	uint32_t	bg_reserved[3];	/* reserved */
};

/* fixed structures */

/* block size */
#define		LOG_BLK_SZ	2
#define		BLK_SZ		( 1024UL << LOG_BLK_SZ )	/* block size in byte */

/* the upper limit of blocks per group.Determined by block bitmap size 
 * 32K */
#define		BLKS_PER_GRP_UPPER_LIMIT	( 8 * BLK_SZ ) 

/* blocks per group we set
 * 32K*/
#define		BLKS_PER_GRP				BLKS_PER_GRP_UPPER_LIMIT

/* the upper limit of group size in byte.
 * 128MB */
#define		GRP_SZ_UPPER_LIMIT			(BLKS_PER_GRP_UPPER_LIMIT * BLK_SZ)

/* group size in byte.
 * 128MB */
#define		GRP_SZ						(BLKS_PER_GRP * BLK_SZ)

/* the upper limit of inodes per group.Determined by inode bitmap size.
 * 32K */
#define		INODS_PER_GRP_UPPER_LIMIT	(8*BLK_SZ)

/* inodes per group we set. Considering video files won't be too small, 
 * we set this as small as much to reduce blocks inode table used. */
#define		INODS_PER_GRP				128U

/* block count inode table occupied
 * 4 */
#define		INOD_TBL_BLKS		(INODS_PER_GRP * EXT2_GOOD_OLD_INODE_SIZE / BLK_SZ)

/* The maximum capacity of the ext2 file system.Determined by block size.
 * (the type of blocks count file is uint32_t)
 * 16TB */
#define		FILSYS_SZ_UPPER_LIMIT		(BLK_SZ << 32)

/* As the group size we decided, this value is the maximum count of groups.
 * Actually, we don't need so many groups.Group descriptors occupy a lot of 
 * blocks after all.We will calculate this value according to partion size
 * in init_struct_e2fs.Also, there will be a member called grp_cnt in struct
 * filsys_ext2.
 * 128K */
#define		MAX_GRP_CNT					(FILSYS_SZ_UPPER_LIMIT / GRP_SZ) // max group count: 



/* Structure of an inode on the disk */
struct ext2_inode{
	uint16_t i_mode;	/* file mode */
	uint16_t i_uid;		/* low 16 bits of own uid */
	uint32_t i_size;	/* file size in byte */
	uint32_t i_atime;	/* access time */
	uint32_t i_ctime;	/* inode change time */

	uint32_t i_mtime;	/* modification time */
	uint32_t i_dtime;	/* deletion time */
	uint16_t i_gid;		/* low 16 bits of group id */
	uint16_t i_links_count;		/* links count */
	uint32_t i_blocks;	/* blocks count */

	uint32_t i_flags;	/* file flags */
	union{
		struct {
			uint32_t l_i_version;	
		}linux1;
		struct {
			uint32_t h_i_translator;
		}hurd1;
	}osd1;	/* os dependent 1 */
	uint32_t i_block[EXT2_N_BLOCKS]; /* Pointers to blocks */
	uint32_t i_generation;		/* file version (for NFS) */
	uint32_t i_file_acl;		/* file ACL */
	uint32_t i_dir_acl;		/* formerly i_dir_acl, directory acl */
	uint32_t i_faddr;			/* fragment addr */
	union{
		struct {
			uint16_t l_i_blocks_hi;
			uint16_t l_i_file_acl_high;
			uint16_t l_i_uid_high;
			uint16_t l_i_gid_high;
			uint16_t l_i_checksum_lo;
			uint16_t l_i_reversed;
		} linux2;
		struct {
			uint8_t h_i_frag;
			uint8_t h_i_fsize;
			uint16_t h_i_mode_high;
			uint16_t h_i_uid_high;
			uint16_t h_i_gid_high;
			uint32_t h_i_author;
		}hurd2;
	}i_osd2; /* os dependent 2 */
};

/* defined i_mode values */
							/* file type */
#define EXT2_S_IFSOCK		0XC000	/* socket */
#define EXT2_S_IFLNK		0XA000	/* symbolic link */
#define EXT2_S_IFREG		0X8000	/* regular file */
#define EXT2_S_IFBLK		0X6000	/* block special file */
#define EXT2_S_IFDIR		0X4000	/* directory */
#define EXT2_S_IFCHR		0X2000	/* character special file */
#define EXT2_S_IFIFO		0X1000	/* fifo */
							/* process execution user/group override */
#define EXT2_S_ISUID		0X0800	/* set process uid */
#define EXT2_S_ISGID		0X0400	/* set process gid */
#define EXT2_S_ISVTX		0X0200	/* sticky bit */
							/* access rights */
#define EXT2_S_IRUSR		0X0100	/* user read */
#define EXT2_S_IWUSR		0X0080	/* user write */
#define EXT2_S_IXUSR		0X0040	/* user execute */
#define EXT2_S_IRGRP		0X0020	/* group read */
#define EXT2_S_IWGRP		0X0010	/* group write */
#define EXT2_S_IXGRP		0X0008	/* group execute */
#define EXT2_S_IROTH		0X0004	/* ohters read */
#define EXT2_S_IWOTH		0X0002	/* others write */
#define EXT2_S_IXOTH		0X0001	/* othres execute */
#define EXT2_S_IALL			0X01FF

/* defined i_flag values */

#define EXT2_SECRM_FL		0X00000001	/* secure deletion */
#define EXT2_UNRM_FL		0X00000002	/* record for undelete */
#define EXT2_COMPR_FL		0X00000004	/* compressed file */
#define EXT2_SYNC_FL		0X00000008	/* synchronous updates */
#define EXT2_IMMUTABLE_FL	0X00000010	/* immutable file */
#define EXT2_APPEND_FL		0X00000020	/* append only */
#define EXT2_NODUMP_FL		0X00000040	/* don't dump/delete file */
#define EXT2_NOATIME_FL		0X00000080	/* don't update .i_atime */

#pragma pack(1)
struct ext2_dir_entry{
	uint32_t inode;		// inode number of one file. 0 not used.
	uint16_t rec_len;	// length of directory entry
	uint8_t	 name_len;	// length of file name
	uint8_t	 file_type; // file type
	char	name[255];	// file name
};
#pragma pack()

struct filsys_ext2{
	const char* device;		/* device name */
	uint64_t	total_size;		/* total size of the partition */
	uint64_t	block_count;	/* total blocks count */
	uint32_t	group_count;		/* total block group count */
	uint32_t	group_desc_blocks;	/* block count group descriptors occupied. */
	uint32_t	total_inode_count;	/* total inode count */
	uint32_t	data_blk_offset; /* data blocks offset against each block group start */
};


extern int init_struct_e2fs(const char* device, struct filsys_ext2 *e2fs);

#endif
