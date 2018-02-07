#include <sys/kprintf.h>
#include <sys/kern_ops.h>
#include <stdarg.h>

int x, y;
unsigned short *placetextat = (unsigned short *) VIDEO_ADDR;
short print_to_screen = 1;

void
vasprintf(
          char *buf,
          const char *fmt,
          va_list args
         )
{
    int len_fmt = (int) kstrlen(fmt);
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

char buf_timer[256] = {-1};
void
print_timer(
			const char *fmt,
		 	...
		   )
{
	va_list args;

    va_start(args, fmt);
    vasprintf(buf_timer, fmt, args);
	va_end(args);

	int x_coord = TIMER_X, y_coord = VERT_BITS;
	if (print_to_screen)
    {
        unsigned char *c = (uint8_t *) buf_timer;
        while (*c)
        {
			placechar(*c, x_coord, y_coord, 0x07);
			x_coord++;
			c++;
		}
	}
}

char buf[4096] = {-1};

void
kprintf(
	    const char *fmt, 
 	    ...
       )
{
    va_list args;
    
    va_start(args, fmt);
    vasprintf(buf, fmt, args);
    va_end(args);

    if (print_to_screen)
    {
    	unsigned char *c = (uint8_t *) buf;
		while (*c)
		{
			if (x == HOR_BITS) 
			{
				x = 0;
				y++;
			}
			if (y == VERT_BITS) 
			{
				y = 0;
				x = 0;
			}
			if (*c == '\n') 
			{
				y++;
				x = 0;
			}
			if (*c == '\b')
			{
				if ((x == 0) && (y != 0))
				{
					placechar(' ', x, y, 0x07);
					y--;
					x = HOR_BITS;
				}
				else
				{
					placechar(' ', x, y, 0x07);
					x--;
				}
			} 
			else 
			{
				placechar(*c, x, y, 0x07);
				x++;
			}
			c++;
		}		     
    }
}

int 
putc(
	 char c
	)
{
	if (x == HOR_BITS)
    {
        x = 0;
        y++;
    }
    if (y == VERT_BITS)
    {
        y = 0;
        x = 0;
    }

    if ((char) c == '\n')
    {
        x = 0;
        y++;
    }
    if ((char) c == '\t')
    {
        switch ((x + TABSPACE) - HOR_BITS)
        {
            case 0:
                x = 4;
                y++;
            break;
            case 1:
                x = 3;
                y++;
            break;
            case 2:
                x = 2;
                y++;
            break;
            case 3:
                x = 1;
                y++;
            break;
            case 4:
                x = 0;
                y++;
            break;
            default:
                x += TABSPACE;	
         	break;
        }
    }
    if ((char) c == '\b')
    {
        if ((x == 0) && (y != 0))
        {
            placechar(' ', x, y, 0x07);
            y--;
            x = HOR_BITS;
        }
        else
        {
            placechar(' ', x, y, 0x07);
            x--;
        }
    }
    else
    {
        placechar(c, x, y, 0x07);
        x++;
    }
    return c;
}
