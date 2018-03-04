#ifndef _FS_H
#define _FS_H

#include <sys/tarfs.h>
#include <sys/defs.h>


#define MAX_NAME 0x100
#define MAXCHILD 0x8
#define SIZE_OF_SECTOR 0x200
#define MAX_NUM_INODES 0x20
#define NUM_DIRECT_BLOCKS 0x8
#define MAX_FILE_SIZE 0x1000 
#define MAX_NUM_BLOCKS 0x200
#define INODES_PER_BLOCK 0x2
#define SUPERBLOCK_SECTOR 0x1
#define INODE_START_SECTOR 0x2
#define NUM_BMAP_SECTORS 0x1
#define BLOCK_BMAP_START_SECTOR 0x12
#define BLOCK_START_START_SECTOR 0x13
#define MAGIC_NUM 0x435EFC
#define MAX_DENTRIES 0x10
#define MAX_PARENTS 0x10

extern uint16_t num_dentries;
extern uint16_t num_inodes;

enum ftype
{
	IS_DIRECTORY,
	IS_FILE
};

typedef struct 
{
	uint64_t s_inodestart;              // Starting Sector No. at inode start
	uint64_t s_ninodes;                 // MAX number of inodes
	uint64_t s_freeinodescount;         // No. of free inodes
	uint64_t s_inode_bmap[2];           // Inode bitmap
	uint64_t s_nblocks;                 // Max number of blocks
	uint64_t s_blockbmapstart;          // Starting Sector No. of block bitmap
	uint64_t s_freeblockscount;         // No. of free blocks
	uint64_t s_blockdatastart;          // Starting Sector No. of block data
	uint64_t s_max_fsize;               // Max File Size
	uint64_t s_magic; 					// Magic Bits
}superblock;

typedef struct 
{
	uint32_t i_num;
	uint64_t i_size;                    // Size of file
	uint32_t ref_count;					// Reference Count
	uint64_t i_block_count;             // Number of blocks used
	uint64_t i_block[NUM_DIRECT_BLOCKS];// Pointers to Direct Block
}inode;

extern inode ino[MAX_NUM_INODES];

struct dirent
{
    char filename[MAX_NAME];
    inode *inode_entry;
	struct dirent *d_parent;
};

typedef struct dirent dentry;

extern dentry dent[MAX_DENTRIES];

//file_seek

int
file_open(
		  const char *pathname
		 );

int
file_read(
		  int fd, 
		  void *buf, 
		  size_t count
		 );

//file_write

//file_close

#endif
