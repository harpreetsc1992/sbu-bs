#include <stdio.h>
#include <sys/defs.h>
#include <sys/pci.h>

#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier

#define	AHCI_BASE	0xfebf1000
 
int got_it;

void 
*memset(
		void *b, 
		int c, 
		int len
	   )
{
  	unsigned char *p = b;

  	while(len > 0)
    {
      	*p = c;
      	p++;
      	len--;
    }

  	return b;
}

void
sysOutLong(
		   unsigned short port,
		   uint32_t val
		  )
{
	__asm__ volatile( "outl %0, %1": : "a"(val), "Nd"(port) );
}

uint32_t
sysInLong(
		  unsigned short port
		 )
{
	uint32_t ret;
    __asm__ volatile( "inl %1, %0" : "=a"(ret) : "Nd"(port) );
	return ret;
}

uint16_t 
getRegVal(
		  uint8_t bus,
		  uint8_t port,
		  uint8_t function,
		  uint8_t offset
		 )
{
	uint64_t address;
	uint64_t lbus = (uint64_t) bus;
	uint64_t lport = (uint64_t) port;
	uint64_t lfunc = (uint64_t) function;
	uint64_t ret = 0;

	address = (uint64_t) ((lbus << 16) | (lport << 11) | (lfunc << 8) | (offset & 0xfc) | (uint32_t) 0x80000000);

	sysOutLong (0xCF8, address);

	if (offset == 0x24)
	{
		ret = (uint16_t) sysInLong(0xCFC);
	}
	else
	{
		ret = (uint16_t)((sysInLong (0xCFC) >> ((offset & 2) * 8)) & 0xffff);
	}
	return ret;
}

uint16_t 
getVendorID(
			uint8_t bus,
			uint8_t port,
			uint8_t function
		   )
{
	uint8_t offset = 0;
	uint16_t r0 = getRegVal(bus, port, function, offset);
	return r0;
}

uint16_t
getDeviceID(
			uint8_t bus,
			uint8_t port,
			uint8_t function
		   )
{
	uint8_t offset = 2;
	uint16_t r0 = getRegVal(bus, port, function, offset);
	return r0;
}

uint16_t 
getClassId(
		   uint16_t bus, 
		   uint16_t device, 
		   uint16_t function
		  )
{
	uint16_t offset = 0xA;
    uint16_t r0 = getRegVal(bus, device, function, offset);
    return (r0 & ~0x00FF) >> 8;
}

uint16_t 
getSubClassId(
			  uint16_t bus, 
			  uint16_t device, 
			  uint16_t function
			 )
{
	uint16_t offset = 0xA;
    uint32_t r0 = getRegVal(bus, device, function, offset);
    return (r0 & ~0xFF00);
}

// Start command engine
void 
start_cmd(
		  hba_port_t *port
		 )
{
	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR);
 
	// Set FRE (bit4) and ST (bit0)
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST; 
}
 
// Stop command engine
void 
stop_cmd(
		 hba_port_t *port
		)
{
	// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;
 
	// Wait until FR (bit14), CR (bit15) are cleared
	while(1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}
 
	// Clear FRE (bit4)
	port->cmd &= ~HBA_PxCMD_FRE;
}

void 
port_rebase(
			hba_port_t *port, 
			int portno
		   )
{
	stop_cmd(port);	// Stop command engine
 
	// Command list offset: 1K*portno
	// Command list entry size = 32
	// Command list entry maxim count = 32
	// Command list maxim size = 32*32 = 1K per port
	port->clb = AHCI_BASE + (portno << 10);
	memset((void *)(port->clb), 0, 1024);
 
	// FIS offset: 32K+256*portno
	// FIS entry size = 256 bytes per port
	port->fb = AHCI_BASE + (32 << 10) + (portno << 8);
	memset((void *)(port->fb), 0, 256);
 
	// Command table offset: 40K + 8K*portno
	// Command table size = 256*32 = 8K per port
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t *)(port->clb);
	for (int i=0; i<32; i++)
	{
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
					// 256 bytes per command table, 64+16+48+16*8
		// Command table offset: 40K + 8K*portno + cmdheader_index*256
		cmdheader[i].ctba = AHCI_BASE + (40 << 10) + (portno << 13) + (i << 8);
		memset((void *)cmdheader[i].ctba, 0, 256);
	}
 
	start_cmd(port);	// Start command engine
}
 
static int 
check_type(
		   hba_port_t *port
		  )
{
	uint32_t ssts = port->ssts;
 
	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;

	if (det == 0)
	{
		return 0;
	}

	if (ipm == 0)
	{
		return 0;
	}
/*
	if (det != HBA_PORT_DET_PRESENT)
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;
*/
	switch (port->sig)
	{
		case SATA_SIG_ATAPI:
			return AHCI_DEV_SATAPI;
		case SATA_SIG_SEMB:
			return AHCI_DEV_SEMB;
		case SATA_SIG_PM:
			return AHCI_DEV_PM;
		default:
			return AHCI_DEV_SATA;
	}
}

void 
probe_port(
		   hba_mem_t *abar
		  )
{
	// Search disk in implemented ports
	uint32_t pi = abar->pi;
	int i = 0;
	while (i < 32)
	{
		if (pi & 1)
		{
			int dt = check_type(&abar->ports[i]);
			if (dt == AHCI_DEV_SATA)
			{
				kprintf("SATA drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SATAPI)
			{
				kprintf("SATAPI drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SEMB)
			{
				kprintf("SEMB drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_PM)
			{
				kprintf("PM drive found at port %d\n", i);
			}
			else
			{
				kprintf("No drive found at port %d\n", i);
			}
		}
 
		pi >>= 1;
		i++;
	}
}

uint64_t 
checkDevice(
			uint8_t bus,
			uint8_t port
		   )
{
	uint8_t function = 0;
	uint32_t vendorID;
	uint32_t deviceID;
	uint32_t classID; 
	uint32_t subClassID;	

	for (; function < 8; function++)
	{
		vendorID = getVendorID(bus, port, function);
		deviceID = getDeviceID(bus, port, function);
		classID = getClassId(bus, port, function);
		subClassID = getSubClassId(bus, port, function);

		if (vendorID == 0xFFFF)
		{
			return 0;
		}
		
		if (vendorID == 0x8086 && deviceID == 0x2922)
		{
			kprintf("Device found %x\n", vendorID);
			kprintf("device ID is %x\n", deviceID);
			got_it = 1;
			return getRegVal(bus, port, function, 0x24);
		}

		if (classID || subClassID);
	}

	kprintf("Vendor ID is %x\n", vendorID);
	kprintf("device ID is %x\n", deviceID);
	kprintf("Class ID is %x\n", classID);
	kprintf("Sub-Class ID is %x\n", subClassID);

	return 0;
}

void 
checkAllBuses(
			 )
{
	uint8_t bus, port;
	uint64_t abar;

	for (bus = 0; bus < 256; ++bus)
	{
		for (port = 0; port < 32; ++port)
		{
			abar = checkDevice(bus, port);	 
			if (got_it)
			{
				kprintf("Bar5 is %x\n", abar);
				return;
			}
		}
	}
}
