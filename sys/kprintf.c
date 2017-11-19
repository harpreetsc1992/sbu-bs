#include <sys/kprintf.h>
#include <stdarg.h>
#include <sys/defs.h>

short print_to_screen = 1;
unsigned short *placetextat = (unsigned short *) 0xFFFFFFFF800B8000;

int x, y;

int 
strlen(
      	const char *fmt
      )
{
    int i = 0;
    while(fmt[i++]!='\0');
    return i;	
}

size_t
vasprintf(
          char *buf,
          const char * fmt,
          va_list args
         )
{
    int len_fmt = strlen(fmt);
    int ptr = 0;
    int i = 0, j;
    char *s;
    unsigned long value;

    for (; i < len_fmt && fmt[i]; i++)
    {
        if (fmt[i] != '%')
        {
            buf[ptr++] = fmt[i];
            continue;
        }
        i++;

        /*
         * fmt[i] == '%'
         */
        switch (fmt[i])
        {
            case 's': /* If it's a string */
                    s = (char *) va_arg(args, char *);
                    while (*s)
                    {
                        buf[ptr++] = *s++;
                    }
                break;
            case 'c': /* If it's a character */
                    buf[ptr++] = (char) va_arg(args, int);
                break;
            case 'x': /* If it's a hexadecimal */
                    value = (unsigned long) va_arg(args, unsigned long);
                    j = 8;
                    while (j-- > 0)
                    {
                        buf[ptr] = "0123456789abcdef"[(value>>(j*4))&0xF];
                        ptr++;
                    }
                break;
            case 'd': /* If it's a decimal number */
                    value = (unsigned long) va_arg(args, unsigned long);
                    j = 9;
                    while (j > 0)
                    {
                        unsigned int n = value / 10;
                        int r = value % 10;
                        buf[ptr + j - 1] = r + '0';
                        j--;
                        value = n;
                    }
                break;
            case 'p': /* If it's a void pointer address */
                    value = (unsigned long) va_arg(args, unsigned long);
                    buf[ptr] = '0';
                    ptr++;
                    buf[ptr] = 'x';
                    ptr++;
                    j = 8;
                    while (j-- > 0)
                    {
                        buf[ptr] = "0123456789abcdef"[(value>>(j*4))&0xF];
                        ptr++;
                    }
                break;
            default: /* Doing nothing currently for width or escape seq */
                    buf[ptr++] = fmt[i];
                break;
        }
    }

    buf[ptr] = '\0';
    return ptr;
}
      
void 
placechar(
		   unsigned char c,
		   int x,
		   int y,
		   int attr
		 )
{
	unsigned short *where;
	unsigned att = attr << 8;
	where = placetextat + (y * 80 + x);
	*where = c | att;
}

char buf[4096] = {-1};

void
kprintf(
	    const char *fmt, 
 	    ...
       )
{
    va_list args;
//    char buf[4096] = {-1};
    
    va_start(args, fmt);
    int len = vasprintf(buf, fmt, args);

    if(len != 0)
	{
	}
	
    va_end(args);

    if(print_to_screen)
    {
    	unsigned char *c = (uint8_t *) buf;
		while (*c)
		{
			if (x == 80) 
			{
				x = 0;
				y = y + 1;
			}
			if (y == 24) 
			{
				y = 0;
				x = 0;
			}
			if (*c == '\n') 
			{
				y = y + 1;
				x = 0;
			} 
			else 
			{
				placechar(*c, x, y, 0x07);
				
				x = x + 1;
			}
			c++;
		}		     
    }
}
