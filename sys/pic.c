#include <sys/pic.h>
#include <sys/defs.h>

void init_pic()
{
	unsigned char a1, a2;
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
	outb(PIC1_DATA, 0x20);                 // ICW2: Master PIC vector offset
	outb(PIC2_DATA, 0x28);                 // ICW2: Slave PIC vector offset
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
 
	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);
 
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

void 
PIC_sendEOI(
			unsigned char irq
		   )
{
	if(irq >= 8)
	{
		outb(PIC2_COMMAND,PIC_EOI);
 	}
	outb(PIC1_COMMAND,PIC_EOI);
}

void
IRQ_set_mask(
			 unsigned char IRQline
			) 
{
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}
 
void 
IRQ_clear_mask(
			   unsigned char IRQline
			  ) 
{
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}
