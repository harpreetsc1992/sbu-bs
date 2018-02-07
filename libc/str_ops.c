#include <memory.h>
#include <sys/kern_ops.h>
#include <string.h> 

char *check = "-1";

int
strcmp(
       const char *str1,
       const char *str2
      )
{
    while (*str1 == *str2++)
    {
        if (*str1++ == 0)
        {
            return 0;
        }
    }

    return (*(const unsigned char *)str1 - *(const unsigned char *)(str2 - 1));
}

int
strncmp(
        const char *str1,
        const char *str2,
        uint64_t n
       )
{
	int i = 0;
	while ((*str1 == *str2++) && (i < n))
	{
		if (*str1++ == 0)
		{
			return 0;
		}
	}

	return (*(const unsigned char *)str1 - *(const unsigned char *)(str2 - 1));
}

uint64_t
strlen(
       const char *str
      )
{
    int i = 0;
	if (str[i] == '\0')
	{
		return i;
	}

    while(str[i++] != '\0');

    return i;
}

void 
strcpy(
       char *dst,
       const char *src
      )
{
    char *str = dst;
    while (*src)
    {
        *dst++ = *src++;
    }
    *dst = '\0';

	dst = str;

    return;
}

void
strncpy(
        char *dst, 
	    const char *src, 
	    uint64_t n
	   )
{
	uint64_t i = 0;
	char *str = dst;

	for (i = 0; i < n; i++) 
	{
		*dst++ = *src++;
		if (*src == '\0') 
		{
			break;
        }
    }
    *dst++ = '\0';
	
	dst = str;
}

char*
strcat(
       char *str1,
       const char *str2
      )
{
    uint64_t len1 = strlen(str1);
    uint64_t len2 = strlen(str2);
    uint64_t i = 0;

    for(i = 0; i < len2 ; i++)
    {
        str1[len1 + i] = str2[i];
    }
    str1[len1 + i] = '\0';

    return str1;
}

void
memcpy(
	   void *dest, 
	   void *src, 
	   size_t n
	  )
{	
	int i; 

	// Typecast src and dest addresses to (char *)
   	char *csrc = (char *)src;
   	char *cdest = (char *)dest;

   	// Copy contents of src[] to dest[]
   	for (i = 0; i < n; i++)
	{
       	cdest[i] = csrc[i];
	}
}

void
memmove(
		void *dest, 
		void *src, 
		size_t n
	   )
{
   	// Typecast src and dest addresses to (char *)
   	char *csrc = (char *)src;
   	char *cdest = (char *)dest;
 
   	// Create a temporary array to hold data of src
   	char temp[n];
 
   	// Copy data from csrc[] to temp[]
   	for (int i = 0; i < n; i++)
	{
       	temp[i] = csrc[i];
	}
 
   	// Copy data from temp[] to cdest[]
   	for (int i = 0; i < n; i++)
	{
       	cdest[i] = temp[i];
	}
}

char *
strtok(
	   char *str,
	   char *delim
	  )
{
	static char* _buffer;
    if(str != NULL)
    {
        _buffer = str;
    }
    if(_buffer[0] == '\0')
    {
        return NULL;
    }
    if (!strcmp(check, _buffer))
    {
        return NULL;
    }

    char *ret = _buffer, *b;
    const char *d;

    for(b = _buffer; *b !='\0'; b++)
    {
        for(d = delim; *d != '\0'; d++)
        {
            if(*b == *d)
            {
                *b = '\0';
                _buffer = b + 1;

                // skip the beginning delimiters
                if(b == ret)
                {
                    ret++;
                    continue;
                }
                check = ret;
                return ret;
            }
        }
    }
    check = ret;
    return ret;
}


