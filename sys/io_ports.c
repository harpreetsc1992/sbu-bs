#include <sys/defs.h>
#include <sys/io_ports.h>

void
outb(
	 uint16_t port,
	 uint8_t val
	)
{
	__asm__ __volatile__(
			"outb %0, %1\t\n"
			:: "a"(val), "Nd"(port)
			);
}

unsigned char
inb(
	uint16_t port
   )
{
	unsigned char val;
    __asm__ __volatile__(
			"inb %1, %0\t\n"
			: "=a"(val)
			: "dN"(port)
			);
	return val;
}
