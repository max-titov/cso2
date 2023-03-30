#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "tlb.h"


size_t sets_n = 16;
size_t ways_n = 4;
size_t elements_n = 4;
size_t buffer[16][4][4];


size_t calc_index(size_t va) { 
    return (va >> POBITS) & 0xF;
}

size_t calc_tag(size_t va) {
    return va >> (POBITS+4);
}

size_t calc_vpn(size_t va) { 
    return va >> POBITS;
}

void tlb_clear() {
    for (int i = 0; i < sets_n; i++) {
        for (int j = 0; j < ways_n; j++) {
            for (int k = 0; k < elements_n; k++) {
                buffer[i][j][k] = 0;
            }
            
        }
    }
}

int tlb_peek(size_t va) {
	size_t index = calc_index(va);
    size_t tag = calc_tag(va);
    size_t vpn = calc_vpn(va);
    for (int i = 0; i < ways_n; i++) {
        if (tag == buffer[index][i][3]) { 
            if (buffer[index][i][2] == 1) {
                return buffer[index][i][1]; 
            }
        }
    }
    return 0;
}

int retrieve_LRU(size_t index) {
    for (int i = 0; i < ways_n; i++) {
        if (buffer[index][i][2] == 0) {
            return i;
        }
    }
    for (int i = 0; i < ways_n; i++) {
        if (buffer[index][i][1] == 4) {
            return i;
        }
    }
    return 0;
}

void change_LRU(size_t index, size_t way) {
    size_t prev_LRU = buffer[index][way][1];
    if (prev_LRU == 0) {
        buffer[index][way][1] = 1;
        for (int i = 0; i < ways_n; i++) {
            if (buffer[index][i][2] == 1 && i != way) {
                buffer[index][i][1]++;
            }
        }
        return;
    }
    buffer[index][way][1] = 0;
    for (int i = 0; i < ways_n; i++) {
        if (buffer[index][i][2] == 1 && buffer[index][i][1] < prev_LRU) {
            buffer[index][i][1]++;
        }
    }
}

size_t tlb_translate(size_t va) {
	size_t index = calc_index(va);
    size_t tag = calc_tag(va);
    size_t vpn = calc_vpn(va);


	size_t mask = (1 << POBITS) - 1;
    size_t offset = va & mask;

    size_t ppn = 0;
	size_t pa = 0;

	int found = 0;
    for (int way = 0; way < ways_n; i++) {
        if (buffer[index][way][2] == 1 && tag == buffer[index][way][3]) { 
			found = 1;
			ppn = buffer[index][way][0];
			ppn = (ppn >> POBITS) << POBITS;
			pa = ppn + offset;
			change_LRU(index, way);
        }
    }

	if (!found){
		size_t va_no_offset = vpn << POBITS;
		ppn = translate(va_no_offset);
		if (ppn == -1) return -1;
		ppn = (ppn >> POBITS) << POBITS;
		int way = retrieve_LRU(index);
		pa = ppn + offset;
		buffer[index][way][0] = pa;
		buffer[index][way][2] = 1; 
		buffer[index][way][3] = tag; 
		change_LRU(index, way);
	}

    return pa;
}
