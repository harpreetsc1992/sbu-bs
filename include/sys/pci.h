#ifndef _PCI_H
#define _PCI_H

#include <sys/ahci.h>
#include <sys/defs.h>
#include <sys/kprintf.h>

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

uint64_t
checkDevice(
			uint8_t bus,
			uint8_t port
		   );

void
checkAllBuses(
			 );

#endif
