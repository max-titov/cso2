#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include "mlpt.h"
#include "config.h"


size_t ptbr = 0;
size_t* ptbr_pointer;
size_t byte_to_bits = 8;
size_t pte_size=8; //page table entry
size_t pte_size_bits;
//size_t vpn_size; //virtual page number
size_t vpn_size_bits; //virtual page number
//size_t va_size; //virtual address
size_t va_size_bits; //virtual address

size_t offset_mask(){
	return pow(2,(POBITS))-1;
}

void set_constants(){
	pte_size_bits=pte_size*byte_to_bits;

	double vpn_size_d = log2(pow(2,POBITS)/pte_size)*LEVELS;
	vpn_size_bits=vpn_size_d;
	//vpn_size_bits=vpn_size*byte_to_bits;
	//va_size=vpn_size +POBITS/byte_to_bits;
	va_size_bits=vpn_size_bits+POBITS;
	//printf("vpn_size_bits: %zu, va_size_bits: %zu\n",vpn_size_bits,va_size_bits);
}

void page_allocate(size_t va)
{
	if(ptbr == 0){
		set_constants();

		//SET PBTR
		posix_memalign((void **)&ptbr, 1<<POBITS, 1<<POBITS);
		ptbr_pointer = (size_t*)ptbr;
		memset(ptbr_pointer, 0, 1<<POBITS); // fill with zeros
	}

	size_t vpn_size_2 = POBITS -3;

	size_t vpn = ((va>>POBITS) >> (vpn_size_2* (LEVELS - 1))) & ((1<<vpn_size_2)-1);

	size_t *pte = ptbr_pointer+vpn;

	if((*pte & 1) == 0){
		//not valid
		size_t new_pointer;
		posix_memalign((void **)&new_pointer, 1<<POBITS, 1<<POBITS);
		memset(new_pointer, 0, 1<<POBITS); // fill with zeros

		*pte = new_pointer+1;

		//ptbr_pointer =(size_t *)new_pointer;
	}
}

size_t translate(size_t va)
{
	if(ptbr == 0){
		return -1;
	}

	size_t vpn_size_2 = POBITS-3;

	size_t vpn = ((va>>POBITS) >> (vpn_size_2* (LEVELS - 1))) & ((1<<vpn_size_2)-1);

	size_t *pte = ptbr_pointer+vpn;

	if((*pte & 1) == 0){
		return -1;
	}

	size_t ppn = (*pte & ~offset_mask());
	size_t offset = (va&offset_mask());
	return ppn + offset;
}

// size_t translate(size_t va)
// {
// 	if(ptbr == 0){
// 		set_constants();
// 	}
	
// 	size_t vpn = (va>>POBITS);
// 	size_t offset = (va&offset_mask());
	
// 	size_t pte = ptbr_pointer[vpn]; 
// 	if(!(pte & 1)){ //not valid
// 		return ~0; //all 1s
// 	}

// 	size_t physical_addr = pte+offset-1; //

// 	return physical_addr;
// }

// int main()
// {	
// 	set_constants();
// 	ptbr = 0x10000;
// 	size_t pa = translate(0x156789);
// 	printf("pa: %zu\n",pa);
// 	return 0;
// 	//testing LEVELS = 4 and POBITS = 12
// 	// 0 pages have been allocated
// 	assert(ptbr == 0);

// 	page_allocate(0x456789abcdef);
// 	// 5 pages have been allocated: 4 page tables and 1 data
// 	assert(ptbr != 0);

// 	page_allocate(0x456789abcd00);
// 	// no new pages allocated (still 5)

// 	int *p1 = (int *)translate(0x456789abcd00);
// 	*p1 = 0xaabbccdd;
// 	short *p2 = (short *)translate(0x456789abcd02);
// 	printf("%04hx\n", *p2); // prints "aabb\n"

// 	assert(translate(0x456789ab0000) == 0xFFFFFFFFFFFFFFFF);

// 	page_allocate(0x456789ab0000);
// 	// 1 new page allocated (now 6; 4 page table, 2 data)

// 	assert(translate(0x456789ab0000) != 0xFFFFFFFFFFFFFFFF);

// 	page_allocate(0x456780000000);
// 	// 2 new pages allocated (now 8; 5 page table, 3 data)
// }