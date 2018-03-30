# Sepcification of ext2 file system
## 1. ext2 filsys layout

### 1.1 Disk partition layout : 


| Constants			| Size	| Description	|
|-------------------|-------|---------------|
|`BLK_SZ`			|4KB	|block size		|
|`BLKS_PER_GRP`		|32K	|blocks per group |			
|`GRP_SZ`			|128MB	| group size	|
|`INODS_PER_GRP`	|128	| inode count per group |
|`INOD_TBL_BLKS`	|4		| blocks count of inode table |

|	|Boot block		|Block group 0	| Block group 1		| ...	|
|---| ------------	| ------------	| ----------------	| ---	|
|size|1024			| group size	| group size		| ...	|

Actually, boot block are contained in block group 0.Boot block and super block are in block 0 of block group 0.

Block group structure :

|	|Super block	|Group descritor table | block bitmap	| inode bitmap	| inode table	| data blocks	|
|---| -------------	| ---------------------|----------------|---------------|---------------|---------------|
|blocks count|	1	|`fs->group_desc_blocks`| 1				| 1				|`INOD_TBL_BLKS`	|	|

- Super block describes basic information of file system, such as magic number, block size, group size, partition size, block count, inodes count and so on.
- Group descriptor table occupys some blocks space. It's consist of group descritors. Each group descriptor describe one block group accordingly.To figure out what group descriptor is, see `ext2_group desc` structure.
- Block bitmap only occupys one block.This's fixed feature that we could't modify it.Every bit in this block indicates corresponding block in this block group is used or not. Blocks count per group is limited by block bitmap size.
- Inode bitmap also occupys only one block. Every one bit indicates wheather the corresponding inode in inode table is used or not. We won't allocate so many files per block because of file size is large enough.
- Inode table is consist of inode structure. Each inode structure describe a file's location and properties.


### 2.2 Structures

```
struct ext2_super_block;		/* super block structure */
struct ext2_group_desc;			/* group descriptor structure */
struct ext2_inode;				/* inode structure */
struct filsys_ext2;				/* layout of ext2 filsys */
```

