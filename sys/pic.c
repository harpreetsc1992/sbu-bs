#include <sys/pic.h>
#include <sys/defs.h>

void init_pic()
{
/*    // Master => Control Port: 0x20 and Data Port: 0x21
    // Slave  => Control Port: 0xA0 and Data Port: 0xA1
    
    // ICW1: Initialize Master and Slave to 0x11    
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    
    // ICW2: Remap the IRQ table: Master from 32 to 39 and Slave 40 to 47 
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    
    // ICW3: Tell Master and Slave PICs their cascade identity
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    // ICW4: 8086/88 mode
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    // Masking of Ports
    outb(PIC1_DATA, 0x0);
    outb(PIC2_DATA, 0x0);
*/
	unsigned char a1, a2;
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
//	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
//	io_wait();
	outb(PIC1_DATA, 0x20);                 // ICW2: Master PIC vector offset
//	io_wait();
	outb(PIC2_DATA, 0x28);                 // ICW2: Slave PIC vector offset
//	io_wait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
//	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
//	io_wait();
 
	outb(PIC1_DATA, ICW4_8086);
//	io_wait();
	outb(PIC2_DATA, ICW4_8086);
//	io_wait();
 
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
