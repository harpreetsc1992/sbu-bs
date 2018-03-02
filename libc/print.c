#include <stdarg.h>
#include <string.h>
#include <sys/syscall.h>
#include <stdlib.h>

void
vsprintf(
         char *buf,
         const char *fmt,
         va_list args
        )
{
    int len_fmt = (int) strlen(fmt);
    int ptr = 0;
    int i = 0, j;
    char *s;
    unsigned long value;

    for (; i < len_fmt && fmt[i]; i++)
    {
        if (fmt[i] != '%')
        {
            *(buf+ptr) = fmt[i];
			ptr++;
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
                    while (value > 0)
                    {
                        unsigned int n = value / 10;
                        int r = value % 10;
                        buf[ptr++] = r + '0';
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
printf(
	   const char *fmt,
	   ...
	  )
{
	va_list args;

	char *buf = malloc(64);
    va_start(args, fmt);
    vsprintf(buf, fmt, args);

    va_end(args);
	
	__syscall1(2, (uint64_t) buf);
}
