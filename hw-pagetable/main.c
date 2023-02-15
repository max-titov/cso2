#include <stdio.h>
#include <assert.h>
#include "mlpt.h"
#include "config.h"

void page_allocate(size_t va)
{
	return;
}

size_t translate(size_t va)
{
	return 0;
}

int main()
{
	return 0;
	// 0 pages have been allocated
	assert(ptbr == 0);

	page_allocate(0x456789abcdef);
	// 5 pages have been allocated: 4 page tables and 1 data
	assert(ptbr != 0);

	page_allocate(0x456789abcd00);
	// no new pages allocated (still 5)

	int *p1 = (int *)translate(0x456789abcd00);
	*p1 = 0xaabbccdd;
	short *p2 = (short *)translate(0x456789abcd02);
	printf("%04hx\n", *p2); // prints "aabb\n"

	assert(translate(0x456789ab0000) == 0xFFFFFFFFFFFFFFFF);

	page_allocate(0x456789ab0000);
	// 1 new page allocated (now 6; 4 page table, 2 data)

	assert(translate(0x456789ab0000) != 0xFFFFFFFFFFFFFFFF);

	page_allocate(0x456780000000);
	// 2 new pages allocated (now 8; 5 page table, 3 data)
}