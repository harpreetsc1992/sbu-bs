#ifndef _DEFS_H
#define _DEFS_H

#define NAMEMAX 256
#define STDIN  0
#define STDOUT 1
#define STDERR 2

#define NULL ((void*)0)

typedef unsigned long  uint64_t;
typedef          long   int64_t;
typedef unsigned int   uint32_t;
typedef          int    int32_t;
typedef unsigned short uint16_t;
typedef          short  int16_t;
typedef unsigned char   uint8_t;
typedef          char    int8_t;

typedef uint64_t size_t;
typedef int64_t ssize_t;

typedef uint64_t off_t;

typedef uint32_t pid_t;

typedef struct
{
    uint64_t inode_num;
    char filename[NAMEMAX];
    int perm;
    int size;
    char type;
    int sector_loc[10];
    int offset;
    uint64_t address;
    int is_fs;
} FILE;

typedef enum
{
	RWXU,
	RWXG,
	ROTH,
	XOTH
}mode_t;

#endif
