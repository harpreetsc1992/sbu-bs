#include <memory.h>

void
memcpy(
	   void *dest, 
	   void *src, 
	   size_t n
	  )
{	
	int i; 

	// Typecast src and dest addresses to (char *)
   	char *csrc = (char *)src;
   	char *cdest = (char *)dest;

   	// Copy contents of src[] to dest[]
   	for (i = 0; i < n; i++)
	{
       	cdest[i] = csrc[i];
	}
}

void
memmove(
		void *dest, 
		void *src, 
		size_t n
	   )
{
   	// Typecast src and dest addresses to (char *)
   	char *csrc = (char *)src;
   	char *cdest = (char *)dest;
 
   	// Create a temporary array to hold data of src
   	char temp[n];
 
   	// Copy data from csrc[] to temp[]
   	for (int i = 0; i < n; i++)
	{
       	temp[i] = csrc[i];
	}
 
   	// Copy data from temp[] to cdest[]
   	for (int i = 0; i < n; i++)
	{
       	cdest[i] = temp[i];
	}
}
