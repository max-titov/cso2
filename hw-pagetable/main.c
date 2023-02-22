#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "mlpt.h"
#include "config.h"

size_t ptbr = NULL;
size_t byte_to_bits = 8;
size_t pte_size=8; //page table entry
size_t pte_size_bits;
//size_t vpn_size; //virtual page number
size_t vpn_size_bits; //virtual page number
//size_t va_size; //virtual address
size_t va_size_bits; //virtual address

void set_constants(){
	//SET PBTR

	pte_size_bits=pte_size*byte_to_bits;

	double vpn_size_d = log2(pow(2,POBITS)/pte_size)*LEVELS;
	vpn_size_bits=vpn_size_d;
	//vpn_size_bits=vpn_size*byte_to_bits;
	//va_size=vpn_size +POBITS/byte_to_bits;
	va_size_bits=vpn_size_bits+POBITS;
	printf("vpn_size_bits: %zu, va_size_bits: %zu\n",vpn_size_bits,va_size_bits);
}

void page_allocate(size_t va)
{
	if(ptbr == NULL){
		set_constants();
	}
	return;
}

size_t translate(size_t va)
{
	set_constants();
	
	size_t vpn = (va>>POBITS);
	size_t offset = (va&0xFFF); // change to be based on POBITS
	printf("vpn: %zx, offset: %zx\n",vpn,offset);
	//treat each page table as an array and the vpn as the index into that array
	size_t pte = 0; 
	for(int i = 0; i<pte_size_bits; i+=1){ //there probably is a more efficient way to do this
		size_t to_index = vpn*pte_size_bits+i;
		size_t* ptbr_pointer = (size_t*)ptbr;
		printf("ptbr_pointer: %zx, pte: %zx\n",ptbr_pointer,pte);
		pte = (pte<<1)+(ptbr_pointer[to_index]);
	}
	if(!(pte & 1)){ //not valid
		return NULL; // change to all 1s
	}
	size_t ppn = pte>>POBITS; //remove the offset bits from pte to get ppn

	size_t physical_addr = (ppn<<POBITS)+offset; // move ppn by offset bits and add offset bits from va

	return physical_addr;
}

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