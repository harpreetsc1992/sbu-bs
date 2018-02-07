/*
void 
write_sector(
			 void* addr, 
			 uint64_t sec_no, 
			 uint64_t sec_off, 
			 uint64_t sz
			)
{
	uint64_t vaddr   = VIRT_PAGE(1) + sec_off;
	uint32_t sec_low = sector_no & 0xFFFFFFFF;
	uint32_t sec_hi  = sector_no >> 32;
    
    // Check for Invalid offset/size
	if (sec_off + size > SIZE_OF_SECTOR)
	return;

    if (size != SIZE_OF_SECTOR) { 
        // Trying to write only few parts in a sector.
        // So need to read whole sector in order to rewrite the remainder part.
        read(&abar->ports[0], sec_low, sec_hi, 1, PHYS_PAGE(1));
    }

    memset((void*)vaddr, 0, SIZE_OF_SECTOR);
    memcpy((void*)vaddr, write_addr, size);
    //kprintf("\nSecW[%d]+%d %d %s", sec_low, sec_off, size, vaddr);

	write(&abar->ports[0], sec_low, sec_hi, 1, PHYS_PAGE(1));

void 
init_superblock(
				superblock *s_star
			   ) 
{
	s_star->s_inodestart = INODE_START_SECTOR;
	s_star->s_ninodes = MAX_NUM_INODES;
	s_star->s_freeinodescount = MAX_NUM_INODES;
	s_star->s_inode_bmap[0] = 0;
	s_star->s_inode_bmap[1] = 0;
	s_star->s_nblocks = MAX_NUM_BLOCKS;
	s_star->s_freeblockscount = MAX_NUM_BLOCKS;
	s_star->s_blockbmapstart = BLOCK_BMAP_START_SECTOR;
	s_star->s_blockdatastart = BLOCK_DATA_START_SECTOR;
	s_star->s_max_fsize = MAX_FILE_SIZE;
	s_star->s_magic = MAGIC_NUM;

    write_sector((void *)s_star, SUPERBLOCK_SECTOR, 0, sizeof(superblock));
};
*/
