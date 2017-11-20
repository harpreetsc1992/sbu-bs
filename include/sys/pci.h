#ifndef _PCI_H
#define _PCI_H

#include <sys/ahci.h>
#include <sys/defs.h>
#include <sys/kprintf.h>

#define SATA_SIG_ATA    0x00000101  // SATA drive
#define SATA_SIG_ATAPI  0xEB140101  // SATAPI drive
#define SATA_SIG_SEMB   0xC33C0101  // Enclosure management bridge
#define SATA_SIG_PM 0x96690101  // Port multiplier

#define AHCI_BASE   0xfebf1000

void
sysOutLong(
		   unsigned short port, 
		   uint32_t val
		  );

uint32_t
sysInLong(
		  unsigned short port
		 );

uint16_t
getRegVal(
		  uint8_t bus,
		  uint8_t port,
		  uint8_t function,
		  uint8_t offset
		 );

uint16_t
getVendorID(
			uint8_t bus,
			uint8_t port,
			uint8_t function
		   );

uint16_t 
getDeviceID(
			uint8_t bus,
			uint8_t port,
			uint8_t function
		   );

uint16_t
getClassId(
		   uint16_t bus,
		   uint16_t device,
		   uint16_t function	
		  );

uint16_t
getSubClassId(
			  uint16_t bus,
			  uint16_t device,
			  uint16_t function
			 );



void
start_cmd(
          hba_port_t *port
         );

void
stop_cmd(
         hba_port_t *port
        );

void
port_rebase(
            hba_port_t *port,
            int portno
           );

/*static*/ int
check_type(
           hba_port_t *port
          );
/*{
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
*/
void
probe_port(
           hba_mem_t *abar
          );

uint64_t
checkDevice(
			uint8_t bus,
			uint8_t port
		   );

void
checkAllBuses(
			 );

#endif
