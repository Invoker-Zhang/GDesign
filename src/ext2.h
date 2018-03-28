#ifndef __ext2_h
#define __ext2_h

#include "ourhdr.h"

#define EXT2_MAGIC		0XEF53

/* File system states */
#define	EXT2_FS_VALID		0X0001		/* Unmount cleanly */
#define EXT2_FS_ERROR		0X0002		/* Errors detected */
#define EXT2_FS_ORPHAN		0X0004		/* Orphans being recoverd */

#define EXT2_ERRORS_CONTINUE	1 /* Continue execution */
#define EXT2_ERRORS_RO			2 /* Remount fs read-only */
#define EXT2_ERRORS_PANIC		3 /* Panic */
#define EXT2_ERRORS_DEFAULT		EXT2_ERRORS_CONTINUE  

/* constants relative to data blocks */
#define EXT2_NDIR_BLOCKS	12
#define EXT2_IND_BLOCK		EXT2_NDIR_BLOCKS
#define EXT2_DIND_BLOCK		(EXT2_IND_BLOCK + 1)
#define EXT2_TIND_BLOCK		(EXT2_DIND_BLOCK + 1)
#define EXT2_N_BLOCKS		(EXT2_TIND_BLOCK + 1)

struct ext2_super_block{
	uint32_t s_inodes_count;	/* inode number of filsys */
	uint32_t s_blocks_count;	/* block number of filsys */
	uint32_t s_r_blocks_count;	/* number of reserved blocks */
	uint32_t s_free_blocks_count;	/* number of free blocks(not used) */
	uint32_t s_free_inodes_count;	/* number of free inodes */
	uint32_t s_first_data_block;	/* first data block */
	uint32_t s_log_block_size;	/* block size : 1024 * 2^# */
	uint32_t s_log_cluster_size;	/* cluster size : 1024 * 2^# */
	uint32_t s_blocks_per_group;	/* blocks per group */
	uint32_t s_clusters_per_group;	/* clusters per group */
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
	uint16_t	bg_free_blk_cnt;	/* Free block count */
	uint16_t	bg_free_inod_cnt;	/* Free inode count */
	uint16_t	bg_used_dirs_cnt;	/* Directories count */
	uint16_t	bg_flags;
	uint32_t	bg_exclude_bitmap_lo;	/* Exclude bitmap for snapshots */
	uint16_t	bg_block_bitmap_csum_lo;	/* crc32(s_uuid+grp_num+bitmap) lsb */
	uint16_t	bg_inode_bitmap_csum_lo;	/* crc32(s_uuid+grp_num+bitmap) lsb */
	uint16_t	bg_itable_unused;	/* Unused inodes count */
	uint16_t	bg_checksum;		/* crc16(s_uuid+group_num+group_desc) */
};

/* configurations */

/* block size */
#define		LOG_BLK_SZ	3
#define		BLK_SZ		( 1024UL * 1 << LOG_BLK_SZ )

/* group size */
/* inode */
#define		INODS_PER_GRP	(BLK_SZ)

/* inner structure of per group */
#define		SUPERBLK_OFF	0
#define		SUPERBLK_SZ		1
#define		BG_DESC_OFF		(SUPERBLK_OFF+SUPERBLK_SZ)
#define		BG_DESC_SZ		1
#define		BLK_BITMAP_OFF	(BG_DESC_OFF+BG_DESC_SZ)
#define		BLK_BITMAP_SZ	1
#define		INOD_BITMAP_OFF (BLK_BITMAP_SZ+BLK_BITMAP_OFF)
#define		INOD_BITMAP_SZ	1
#define		INOD_TBL_OFF	(INOD_BITMAP_OFF+INOD_BITMAP_SZ)
#define		INOD_TBL_SZ		(INOD_BITMAP_SZ * 128)

#define		MAX_GRP_CNT		(BLK_SZ*BG_DESC_SZ/32)
			// 2^8
#define		BLKS_PER_GRP	(BLK_SZ*BLK_BITMAP_SZ * 8) 
			// 2^16
#define		GRP_SZ		(BLK_SZ * BLKS_PER_GRP)
			// 2^29
#define		MAX_DSK_SZ		(MAX_GRP_CNT*GRP_SZ)
			//2^37 128G


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
	uint32_t i_size_high;		/* formerly i_dir_acl, directory acl */
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
	}osd2; /* os dependent 2 */
};

struct filsys_ext2{
	const char* device;		/* device name */
	uint64_t	tot_sz;		/* total size of the partition */
	uint64_t	blk_cnt;	/* total blocks count */
	uint32_t	grp_cnt;		/* total block group count */
	uint32_t	tot_inod_cnt;	/* total inode count */
	uint32_t	inod_blks;	/* blocks count inode table occupied */
	uint64_t	last_grp_blks;
};

extern int init_struct_e2fs(const char* device, struct filsys_ext2 *e2fs);

#endif
