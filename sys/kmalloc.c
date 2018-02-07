#include <sys/memory.h>
/*
void
*kmalloc(
		 uint64_t sz
		)
{
	int i = 0;
	int cntr = 0;
	int num_pages = (sz / PAGE_SIZE);
	uint64_t *_page;
	
	uint64_t *page = (uint64_t *)page_alloc();
	cntr++;
	_page = page;

	i = (sz % PAGE_SIZE);

	while (cntr < num_pages)
	{
		_page = (uint64_t *)page_alloc();
		page + (cntr * PAGE_SIZE/sizeof(uint64_t)) = _page;
		cntr++;
	}

	if (i > 0)
	{
		_page = (uint64_t *)page_alloc();
		page + (cntr * PAGE_SIZE/sizeof(uint64_t)) = _page;
	}

	return page;
}
*/
