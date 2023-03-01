#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include "mlpt.h"
#include "config.h"


size_t ptbr = 0;
size_t* ptbr_pointer;

size_t offset_mask(){
    return pow(2,(POBITS)) - 1;
}

void page_allocate(size_t va){
    if (ptbr == 0){
        //SET PBTR
        size_t mem_size = 1 << POBITS;
        //allocate memory
        posix_memalign((void **)&ptbr, mem_size, mem_size);
        ptbr_pointer = (size_t*)ptbr;
        memset((size_t*)ptbr_pointer, 0, mem_size); // fill with zeros
    }

    //get the vpn size in bits based on POBITS
    size_t vpn_size = POBITS -3;

    //extract vpn from the va
    size_t vpn = ((va >> POBITS) >> (vpn_size * (LEVELS - 1))) & ((1 << vpn_size) - 1);

    //calvulate pointer to the page table entry based on the extracted vpn
    size_t *pte = ptbr_pointer+vpn;

    if ((*pte & 1) == 0){
        //not valid so allocate memory
        size_t new_pointer;
        size_t mem_size = 1 << POBITS;
        posix_memalign((void **) & new_pointer, mem_size, mem_size);
        memset((size_t*)new_pointer, 0, mem_size); // fill with zeros

        //set valid bit to 1 and assign to pte
        *pte = new_pointer + 1;

        //ptbr_pointer =(size_t *)new_pointer;
    }
}

size_t translate(size_t va){
    //if ptbr has not been set yet return -1
    if (ptbr == 0){
        return -1;
    }

    //get the vpn size in bits based on POBITS
    size_t vpn_size = POBITS-3;

    //extract vpn from the va
    size_t vpn = ((va >> POBITS) >> (vpn_size * (LEVELS - 1))) & ((1 << vpn_size) - 1);

    //calvulate pointer to the page table entry based on the extracted vpn
    size_t *pte = ptbr_pointer + vpn;

    //if valid bit is 0 return -1
    if ((*pte & 1) == 0){
        return -1;
    }

    //extract ppn from pte by inverse masking the POBITS bits
    size_t ppn = (*pte & ~offset_mask());
    //extract offset from va by masking the POBITS bits
    size_t offset = (va & offset_mask());
    //final physical address calculation
    return ppn + offset;
}

// int main()
// {	
//     //testing LEVELS = 4 and POBITS = 12
//     // 0 pages have been allocated
//     assert(ptbr == 0);

//     page_allocate(0x456789abcdef);
//     // 5 pages have been allocated: 4 page tables and 1 data
//     assert(ptbr != 0);

//     page_allocate(0x456789abcd00);
//     // no new pages allocated (still 5)

//     int *p1 = (int *)translate(0x456789abcd00);
//     *p1 = 0xaabbccdd;
//     short *p2 = (short *)translate(0x456789abcd02);
//     printf("%04hx\n", *p2); // prints "aabb\n"

//     assert(translate(0x456789ab0000) == 0xFFFFFFFFFFFFFFFF);

//     page_allocate(0x456789ab0000);
//     // 1 new page allocated (now 6; 4 page table, 2 data)

//     assert(translate(0x456789ab0000) != 0xFFFFFFFFFFFFFFFF);

//     page_allocate(0x456780000000);
//     // 2 new pages allocated (now 8; 5 page table, 3 data)
// }