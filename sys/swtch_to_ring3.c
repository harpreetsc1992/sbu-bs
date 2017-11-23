#include <sys/swtch_rings.h>

void
initiate_jmp(
			)
{
	jmp_usrmode();
	return;
}

void
tst_usr_fn(
		  )
{
	return;
}
