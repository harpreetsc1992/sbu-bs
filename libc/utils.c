#include <utils.h>
       
char *
itoa(
     int value,
     char *str,
     int base
    )
{
    char *rc, *ptr, *low;
    // Check for supported base.
    if (base < 2 || base > 36)
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if (value < 0 && base == 10)
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while (value);
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while (low < ptr)
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}

uint64_t
stoi(
	 const char *str
	)
{
	uint64_t i = 0;
	int j = 0;
	
	for (;(str[j] >= '0' && str[j] <= '9') && j < 12; j++)
	{
		i = i * 10 + (str[j] - '0');
	}
	
	return i;
}

uint64_t
power(
	  uint64_t base, int pow
	 )
{
	if (pow == 0)
	{
		return 1;
	}
	
	return (base * power(base, pow - 1));
}

uint64_t 
oct2dec(
		uint64_t oct
	   )
{
	uint64_t dec = 0;
	uint64_t pow = 0;
	
	while (oct != 0)
	{
		dec = dec + (oct % 10) * power(8, pow++);
		oct /= 10;
	}
	
	return dec;
}
