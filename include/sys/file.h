#ifndef _FILE_H
#define _FILE_H
/*
typedef enum 
{ 
	FD_NONE, 
	FD_PIPE, 
	FD_INODE 
} type_t;

// in-memory copy of an inode
struct inode 
{
  	uint32_t dev;           // Device number
  	uint32_t inum;          // Inode number
  	int ref;            	// Reference count
  	struct sleeplock lock; 	// protects everything below here
  	int valid;          	// inode has been read from disk?

  	uint16_t type;         	// copy of disk inode
  	uint16_t major;
  	uint16_t minor;
  	uint16_t nlink;
  	uint32_t size;
  	uint32_t addrs[NDIRECT+1];
};

struct file 
{
  	int ref; 				// reference count
	type_t type;
  	char readable;
  	char writable;
  	struct pipe *pipe;
  	struct inode *ip;
  	uint32_t off;
};
*/
#endif
