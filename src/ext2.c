#include "ext2.h"
#include "ourhdr.h"

int init_struct_e2fs(const char* device, struct filsys_ext2* fs){
	fs->device = device;
	int fd;
	if( (fd = open(device, O_RDONLY)) < 0)
		return -1;
	fs->tot_sz = lseek(fd, 0, SEEK_END);
	fs->blk_cnt = (fs->tot_sz ) / BLK_SZ;
	if( fs->blk_cnt % GRP_SZ < GRP_SZ / 10){
		fs->grp_cnt = fs->blk_cnt / BLKS_PER_GRP;
	} else {
		fs->grp_cnt = fs->blk_cnt / BLKS_PER_GRP + 1;
	}
	fs->tot_inod_cnt = fs->grp_cnt * INODS_PER_GRP; 
	fs->inod_blks = (INODS_PER_GRP * 128 - 1) / BLK_SZ + 1;

	disp16(fs->tot_sz);
	disp16(fs->blk_cnt);
	disp16(fs->grp_cnt);
	disp16(fs->tot_inod_cnt);
	disp16(fs->inod_blks);
	close(fd);
	return 0;
}
