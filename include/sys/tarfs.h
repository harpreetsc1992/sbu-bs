#ifndef _TARFS_H
#define _TARFS_H

#include <utils.h>
#include <sys/kern_ops.h>
#include <sys/kprintf.h>
#include <sys/memory.h>
#include <sys/process.h>
#include <sys/elf64.h>

#define DIRECTORY 5
#define FILE_TYPE 0

extern char _binary_tarfs_start;
extern char _binary_tarfs_end;

struct posix_header_ustar {
  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char checksum[8];
  char typeflag[1];
  char linkname[100];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char pad[12];
};

typedef struct
{	
	char name[256];
	int size;
	int typeflag;
	uint64_t addr_hdr;
	int index;
}tarfs_entry;

tarfs_entry tarfs_fs[100];

int
file_exists(
            char *file
           );

void 
init_tarfs(
		  );

int
get_per_indx(
             char* dir
            );

int
get_per_indx_file(
                  char* dir
                 );

char *
vma_data(
         uint32_t *sz
        );

uint64_t
opendir(
         char *dir
       );

uint64_t
readdir(
        uint64_t addr
       );

FILE*
openfile(
      	 char *file
    	);

int
readfile(
         FILE* file_addr,
         int size,
         uint64_t buf
        );

int
closefile(
          int fd
         );

int
close_dir(
          uint64_t addr
         );
#endif
