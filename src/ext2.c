#include "ext2.h"
#include "ourhdr.h"

int init_struct_e2fs(const char* device, struct filsys_ext2* fs){
	fs->device = device;
	int fd;
	if( (fd = open(device, O_RDONLY)) < 0)
		return -1;
	fs->total_size = lseek(fd, 0, SEEK_END);
	fs->block_count = (fs->total_size) / BLK_SZ;
	if( fs->block_count % GRP_SZ < GRP_SZ / 10){
		fs->group_count = fs->block_count / BLKS_PER_GRP;
	} else {
		fs->group_count = fs->block_count / BLKS_PER_GRP + 1;
	}
	fs->total_inode_count = fs->group_count * INODS_PER_GRP;
	fs->group_desc_blocks = (fs->group_count * 32 - 1) / BLK_SZ + 1;

	disp16(fs->total_size);
	disp16(fs->block_count);
	disp16(fs->group_count);
	disp16(fs->group_desc_blocks);
	disp16(fs->total_inode_count);

	close(fd);
	return 0;
}
