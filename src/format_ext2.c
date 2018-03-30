#include <assert.h>
#include "ourhdr.h"
#include "ext2.h"

/* format the device.
 * specifications and define in ourhdr.h. */

static int fd;

static void mark_inode_bitmap(uint32_t inode_num, struct filsys_ext2* e2fs){
	inode_num--;
	uint32_t group_index = inode_num / INODS_PER_GRP;
	uint32_t inode_off = inode_num % INODS_PER_GRP;
	uint32_t byte_off = inode_off / 8;
	uint32_t bit_off = inode_off % 8;

	uint64_t write_pos = group_index * GRP_SZ + (2 + e2fs->group_desc_blocks) * BLK_SZ + byte_off;
	lseek(fd, write_pos, SEEK_SET);
	uint8_t tmp;
	read(fd, &tmp, sizeof(uint8_t));
	assert((tmp &= !(1u << bit_off)) == 0);
	tmp |= (1u << bit_off);

	lseek(fd, write_pos, SEEK_SET);
	write(fd, &tmp, sizeof(tmp));

}

/* Write an inode to correspoding inode table.
 *
 * inode_num: inode number.
 * pinode: pointer to a ext2_inode.
 * fd: fd of device
 * e2fs: pointer to struct filsys_ext2
 */
static void write_inode(uint32_t inode_num, struct ext2_inode* pinode, struct filsys_ext2* e2fs){
	inode_num--;
	uint32_t group_index = inode_num / INODS_PER_GRP;
	uint32_t inode_off = inode_num % INODS_PER_GRP;

	uint64_t write_pos = group_index * GRP_SZ + (e2fs->data_blk_offset - INOD_TBL_BLKS) * BLK_SZ + inode_off * EXT2_GOOD_OLD_INODE_SIZE;
	lseek(fd, write_pos, SEEK_SET);
	write(fd, pinode, EXT2_GOOD_OLD_INODE_SIZE);
}

/* Scan the directory entries to find out write position for new 
 * directory entry.
 *
 * pinode : directory's inode pointer.
 * ent_size: new directory entry's size.
 */
uint64_t scan_dir(struct ext2_inode * pinode,int ent_size){

}

/* write directory entry to a directory file.
 *
 * inode_num: inode number of the directory.(counted from 1).
 * pdir_ent: pointer to struct ext2_dir_entry.
 */
static void write_dir_entry(uint32_t inode_num, struct ext2_dir_entry* pdir_ent,  struct filsys_ext2* e2fs){
	inode_num--;
	uint32_t group_index = inode_num / INODS_PER_GRP;
	uint32_t inode_off = inode_num % INODS_PER_GRP;

	struct ext2_inode tmp_inode;
	uint64_t write_pos = group_index * GRP_SZ + (e2fs->data_blk_offset - INOD_TBL_BLKS)*BLK_SZ + inode_off * EXT2_GOOD_OLD_INODE_SIZE;
	
	lseek(fd, write_pos, SEEK_SET);
	read(fd, &tmp_inode, sizeof(tmp_inode));

	write_pos = scan_dir(&tmp_inode, pdir_ent->rec_len);

}

void format_ext2(struct filsys_ext2 * e2fs){
	if ( (fd = open(e2fs->device, O_RDWR)) < 0)
		err_sys("open error in format");
	struct ext2_super_block super_block = {
		.s_inodes_count = e2fs->total_inode_count,
		.s_blocks_count = e2fs->block_count,
		.s_r_blocks_count = 0, 
		.s_free_blocks_count =  e2fs->block_count- e2fs->group_count* e2fs->data_blk_offset,
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
	struct ext2_group_desc* pdesc; 
	pdesc = (struct ext2_group_desc*) malloc (sizeof(struct ext2_group_desc));
	if( !pdesc ) 
		err_sys("fail to malloc");
	memset(pdesc, 0, sizeof(struct ext2_group_desc));
	disp(e2fs->data_blk_offset);;
	/* write fsinfo per group */
	for(int i = 0; i < e2fs->group_count; i++){
		clearSectors(fd, i*GRP_SZ / SEC_SZ, e2fs->data_blk_offset *BLK_SZ/SEC_SZ);
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
			pdesc->bg_free_blocks_count = BLKS_PER_GRP - e2fs->data_blk_offset;
			pdesc->bg_free_inodes_count = INODS_PER_GRP;
			pdesc->bg_used_dirs_cnt = 0;
			write(fd, pdesc, sizeof(struct ext2_group_desc));
		}
		/* write block bitmap block */
		uint8_t val = 0xff;
		lseek(fd, i * GRP_SZ + (1 + e2fs->group_desc_blocks) * BLK_SZ, SEEK_SET);
		for(int k = 0; k < e2fs->data_blk_offset/8; k++){
			write(fd, &val, 1);
		}
		val = e2fs->data_blk_offset % 8;
		val = (1u << val) - 1;
		write(fd, &val, 8);
	}
	free(pdesc);
	/* create root directory */
	struct ext2_inode *pinode;
	pinode = (struct ext2_inode*) malloc (sizeof (struct ext2_inode));
	if( !pinode)
		err_sys("fail to malloc");
	memset(pinode, 0, sizeof(struct ext2_inode));

	pinode->i_mode = EXT2_S_IFDIR | EXT2_S_IALL;
	pinode->i_size = 0;
	uint32_t cur_time = time(0);
	pinode->i_atime = cur_time;
	pinode->i_ctime = cur_time;
	pinode->i_mtime = cur_time;
	pinode->i_links_count = 2;
	pinode->i_blocks = BLK_SZ / 512;
	pinode->i_flags = EXT2_UNRM_FL;
	pinode->i_block[0] = e2fs->data_blk_offset;
	pinode->i_generation = 0;
	pinode->i_file_acl = 0;
	pinode->i_dir_acl = 0;
	pinode->i_faddr = 0;

	mark_inode_bitmap(2, e2fs);
	lseek(fd, (e2fs->group_desc_blocks + 1)* BLK_SZ, SEEK_SET);
	uint8_t temp = 1;
	write(fd, &temp, sizeof(temp));
	write_inode(2, pinode, e2fs);

	struct ext2_dir_entry* pdir_ent;
	pdir_ent = (struct ext2_dir_entry*) malloc(sizeof(struct ext2_dir_entry));
	if( !pdir_ent)
		err_sys("fail to malloc");
	memset(pdir_ent, 0, sizeof(struct ext2_dir_entry));

	pdir_ent->inode = 2;
	pdir_ent->rec_len = 12;
	pdir_ent->name_len = 1;
	pdir_ent->file_type = 0;
	strcpy(pdir_ent->name, ".");
	
	lseek(fd, e2fs->data_blk_offset * BLK_SZ, SEEK_SET);
	write(fd, pdir_ent, pdir_ent->rec_len);


	strcpy(pdir_ent->name, "..");
	write(fd, pdir_ent, pdir_ent->rec_len);


	close(fd);
}

/* create a file:
 * args: 
 * 1. parent directory's inode number
 * 2. file type, file size, file name.
 * 3. inode number??
 * 4. 
 * steps:
 * 1. allocate inode number ?
 * 2. write inode bitmap
 * 3. write block bitmap
 * 4. write inode table block
 * 5. update parent directory's entries.
 */

uint32_t alocate_inode_number(){
	;
}
