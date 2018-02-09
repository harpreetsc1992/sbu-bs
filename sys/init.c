#include <sys/init.h>
#include <sys/process.h>
#include <sys/gdt.h>
#include <sys/swtch_rings.h>

void
idle(
	)
{
	while (1)
	{
		yield();
	}
}

void
init(
	)
{
	set_tss_rsp((uint64_t *)processes->c_t.rsp);
	uint64_t addr = create_usr_pcb("bin/sbush");
	jmp_usrmode(addr);
}
