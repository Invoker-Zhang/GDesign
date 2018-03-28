			Fat32 disk partition layout:
/* --------------------------------------------- */ Reserved area
				DBR sector
				fsinfo sector
				backup sectors
/* --------------------------------------------- */ Fat area
			| root directory |
			| folders		 |
			| allocinfo file |
			| index files	 |
			| video files	 |
/* --------------------------------------------- */ Data area
								cluseter number
			| root directory |		2
			| folders		 | 3 ~ folderNum+2
			|alloc info file | folderNum + 3
			| index files    | folderNum+4 ~ folderNum*3+3
			|video block file| folderNum*3+4 ~ 
/* --------------------------------------------- */ End.There may be several orphan sectors.
