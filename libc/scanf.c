#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/defs.h>
#include <memory.h>
#include <utils.h>
#include <stdlib.h>

void 
scanf(
	  const char *str, 
	  ...
	 )
{
	char *read_buf = malloc(64);
    uint32_t len;
    va_list arg;
    const char *ptr = NULL;
    va_start(arg, str);

    for (ptr = str; *ptr; ptr++) 
	{
        if (*ptr == '%') 
		{
            switch (*++ptr) 
			{
                case 's':
                    len = read(STDIN, (char *)read_buf, 0);
                    memcpy((void *) va_arg(arg, char *), (void *) read_buf, len); 
				break;

                case 'c':
                    va_arg(arg, char *);
                    len = read(STDIN, (char *)read_buf, 0);
                    char *ch = (char *)read_buf; 
					if (ch);
				break;

				case 'd':
                    va_arg(arg, uint32_t *);
                    len = read(STDIN, (char *)read_buf, 0);
                    uint32_t *dec = (uint32_t *)stoi(read_buf);
					if (dec);
                    break;
               
                default:
                break;
            }
        }
    }
    va_end(arg); 
}
