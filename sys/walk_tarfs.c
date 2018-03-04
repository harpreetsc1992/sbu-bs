#include <sys/fs.h>
#include <sys/kern_ops.h>

uint16_t num_dentries;
uint16_t num_inodes;
dentry dent[MAX_DENTRIES];
inode ino[MAX_NUM_INODES];

void
set_dirents(
			uint32_t i
		   )
{
	uint32_t j = 0;
	char *path = NULL;
	char *slash = "/";
	
	kstrcpy(dent[num_dentries].filename, tarfs_fs[i].name);
    ino[num_inodes].i_size = tarfs_fs[i].size;
	ino[num_inodes].ref_count = -1;
	ino[num_inodes].i_block_count = 0;
	ino[num_inodes].i_num = num_inodes;
	dent[num_dentries].inode_entry = &ino[num_inodes];

	path = kstrtok(tarfs_fs[i].name, slash);
	while (path)
	{
		j++;
		path = kstrtok(NULL, slash);
	}

	if (j == 1)
	{
		dent[num_dentries].d_parent = NULL;
	}
	else
	{
		int index = num_dentries - 1;
		while (index >= 0)
		{
        	if (tarfs_fs[index].typeflag == DIRECTORY)
        	{
				dent[num_dentries].d_parent = &dent[index];
				break;
        	}
			else
			{
				index--;
			} 
		}
	}
}	

int 
file_exists(
			char *file
		   )
{
	struct posix_header_ustar *test_fs = (struct posix_header_ustar *)&_binary_tarfs_start; 
	int i = 1, tmp = 512;
	size_t size;

	while (kstrlen(test_fs->name) != 0)
	{
		test_fs = (struct posix_header_ustar *)(&_binary_tarfs_start + tmp);
		size = oct2dec(stoi(test_fs->size));

		if (kstrlen(test_fs->name) == 0)
		{
			return 999;
		}
		if (kstrcmp(test_fs->name, file) == 0)
		{
			return (tmp + 512);
		}
		if (size == 0)
		{
			tmp = tmp + 512;
		}
		else
		{
			tmp += (size % 512 == 0) ? size + 512 : size + (512 - size % 512) + 512;
		}
		i++;
	}
	return 0;
}

int 
get_per_indx(
			 char* dir
		    )
{
    char name[100];
    int len = kstrlen(dir);
    int i = 0;
    kstrcpy(name, dir);
    len = len - 2;
    while(name[len] != '/')
    {
        len--;
        if(len == 0)
		{
            return 999;
		}
    }
    name[++len] = '\0';
    while(kstrcmp(name, tarfs_fs[i].name) !=  0)
	{
        i++;
	}

    return i;
}

int 
get_per_indx_file(
				  char* dir
				 )
{
    char name[100];
    int len = kstrlen(dir);
    int i = 0;
    kstrcpy(name, dir);
    len = len - 1;
    while(name[len] != '/')
    {
        len--;
        if(len == 0)
		{
            return 999;
		}
    }
//    name[++len] = '\0';
    while(kstrcmp(name, tarfs_fs[i].name) !=  0)
	{
        i++;
	}

    return i;
}

void
init_tarfs(
		  )
{
	struct posix_header_ustar *tar_var = (struct posix_header_ustar *)&_binary_tarfs_start;
	
	int tmp = 0;
	uint32_t fsz = 0;
	uint32_t i = 0;

	while ((uint64_t)tar_var < (uint64_t)&_binary_tarfs_end)
	{
		tar_var = (struct posix_header_ustar *)(&_binary_tarfs_start + tmp);

		if (kstrlen(tar_var->name) == 0)
		{
			break;
		}
		fsz = oct2dec(stoi(tar_var->size));
		kstrcpy(tarfs_fs[i].name, tar_var->name);
		
		tarfs_fs[i].size = fsz;
		tarfs_fs[i].addr_hdr = (uint64_t)&_binary_tarfs_start + tmp;
        tarfs_fs[i].typeflag = stoi(tar_var->typeflag);
        if(tarfs_fs[i].typeflag == FILE_TYPE)
		{
            tarfs_fs[i].index = get_per_indx_file(tarfs_fs[i].name);
		}
        else if(tarfs_fs[i].typeflag == DIRECTORY)
        {
            tarfs_fs[i].index = get_per_indx(tarfs_fs[i].name);
        }
		if (fsz == 0)
		{
			tmp = tmp + 512;
		}
		else
		{
			tmp +=  ((fsz % 512 == 0) ? (fsz + 512) : (fsz + (512 - fsz % 512) + 512));
		}

		set_dirents(i);
 		i++;
		num_dentries++;
		num_inodes++;
	}
}
