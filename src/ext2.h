#ifndef __ext2_h
#define __ext2_h

#include "ourhdr.h"

struct ext2_super_block{
	uint32_t s_inodes_count;	/* inode number of filsys */
	uint32_t s_blocks_count;	/* block number of filsys */
	uint32_t s_r_blocks_count;	/* number of reserved blocks */
	uint32_t s_free_blocks_count;	/* number of free blocks(not used) */
	uint32_t s_free_inodes_count;	/* number of free inodes */
	uint32_t s_first_data_block;	/* first data block */
	uint32_t s_log_block_size;	/* block size */
	uint32_t s_log_cluster_size;	/* cluster size */
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

};


#endif
