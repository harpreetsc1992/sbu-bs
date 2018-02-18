#include <sys/tarfs.h>
#include <sys/k_utils.h>

char *
vma_data(
		 uint32_t *sz
		)
{
	int offset = file_exists(curr_file);
	Elf64_Ehdr *elf = (Elf64_Ehdr *)(&_binary_tarfs_start + offset);
    Elf64_Phdr *ph;
    ph = (Elf64_Phdr *)((uint64_t)elf + elf->e_phoff);
    int i = 0;
    for (i = 0; i < elf->e_phnum; i++)
    {
        ph = (Elf64_Phdr *)(((uint64_t)elf + elf->e_phoff) + i);
        if (ph->p_type != ELF_PROG_LOAD)
        {
            continue;
        }
        else
        {
            if (ph->p_filesz > ph->p_memsz)
            {
                kprintf("Invalid file size\n");
            }

           kmemcpy((char*) ph->p_vaddr, (void *) ((uint64_t)elf + (uint64_t)ph->p_offset), ph->p_filesz);
			*sz = ph->p_filesz;
            if (ph->p_filesz < ph->p_memsz)
            {
                memset((char*) ph->p_vaddr + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
				*sz = ph->p_memsz;
            }
			break;
		}
	}
	return (char *)ph->p_vaddr;
}

uint64_t 
opendir(
		char *dir
       )
{
    tarfs_entry tar_e;
    int i = 0;

    while (1)
    {
        tar_e = tarfs_fs[i];
        i++;
		
        if (0 == kstrlen(tar_e.name))
		{
            break;
		}

        if ((0 == kstrcmp(tar_e.name, dir)) && (tar_e.typeflag == DIRECTORY))
		{
            return tar_e.addr_hdr;
		}
    }

    kprintf("\tNo such directory \n");
    kprintf("%s\n", dir);

    return 0;
}

uint64_t 
readdir(
		uint64_t addr
	   )
{
    tarfs_entry tar_e;
    char *dir;
    int i = 0, parent = -1, len = 0;
/*
    if (addr == 999)
	{
        parent = 999; 
	}
*/
    while (1)
    {
        tar_e = tarfs_fs[i];

        if (0 == kstrlen(tar_e.name))
		{
            break;
		}

        if (tar_e.addr_hdr == addr)
        {
            parent = i;
            i++;
            dir = tar_e.name;
            len = kstrlen(dir);
            continue; 
        }

        if ((0 == (kstrncmp(tar_e.name, dir, kstrlen(dir)))) && (tar_e.index == parent))
        {
			kprintf("%s\t", tar_e.name + len);
        }
        i++;
    }

    return tarfs_fs[i].addr_hdr;
}

FILE* 
openfile(
	 	 char *file
		)
{
	FILE *fd = (FILE *)kmalloc(sizeof(FILE));//kmalloc(sizeof(FILE));
	tarfs_entry tar_e;
	int i = 0;

	while (1)
	{   
		tar_e = tarfs_fs[i];
		i++;
	
		if (0 == kstrlen(tar_e.name))
		{
			break;
		}

		if ((0 == kstrcmp(tar_e.name, file)) && (tar_e.typeflag == FILE_TYPE))
        {   
			kstrcpy(fd->filename, file); 
			fd->inode_num = i - 1;
			fd->offset = 0;
			fd->address = tar_e.addr_hdr;
			return fd;
        } 
	}

	kprintf("\n No such file \n");
	kprintf("%s\n", file);

	return NULL;
}

int 
readfile(
		 FILE* file_addr, 
		 int size, 
		 uint64_t buf
		)
{
	struct posix_header_ustar* file_hdr;
	uint64_t file_size;
	char *tmp = NULL;
	char *file_start_addr = NULL;

	int i, bytes;

	if (size < 0)
	{
		return 0;
	}

	if (0 == file_addr->address)
	{
        return 0;
	}

	file_hdr = (struct posix_header_ustar *) file_addr->address;
    file_size = oct2dec(stoi(file_hdr->size));

    if (file_size < size)
	{
        size = file_size;
	}

    bytes = size;
    tmp = (char *)buf;
    file_start_addr = (char *)(file_addr->address + 512);

    i = 0;

    while (size-- > 0)
	{
        tmp[i++] = *file_start_addr++;
    }    

    tmp[i] = '\0';
   
	return bytes;
}

int 
closefile(
		  int fd
		 )
{
	if (fd < 0)
	{
		return -1;
	}

	FILE *file_addr = (FILE *) (uint64_t)fd;
	file_addr->address = 0;
	file_addr->inode_num = 0;
	file_addr->offset = 0;
	kstrcpy(file_addr->filename, "");

	return 1;
}

int 
close_dir(
		  uint64_t addr
		 )
{
	return 1;
}
