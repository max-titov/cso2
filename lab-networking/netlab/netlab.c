#include <stdio.h>
#include "netsim.h"

char checksum(size_t data_len, void *data){
	char checksum = 0;
	for(int i = 1; i < data_len; i++){
		checksum = checksum^((char *)data)[i];
	}
	return checksum;
}

void acknowledge (int seq_num){
	char ack[5];
    ack[1] = 'A'; ack[2] = 'C'; ack[3] = 'K'; ack[4] = seq_num+1;
	ack[0]=checksum(5,ack);
	send(5, ack);
}

void pnum(void *num) {
    printf("%d\n", (int)num);
}

void recvd(size_t len, void* _data) {
    // FIX ME -- add proper handling of messages
    char *data = _data;
    fwrite(data+3,1,len,stdout);
    fflush(stdout);

	acknowledge(data[4]);

	int one = setTimeout(pnum, 1000, (void *)1uL);

	clearTimeout(one);

	//waitForAllTimeouts();
	
	
}

int main(int argc, char *argv[]) {
    // this code should work without modification
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s n\n    where n is a number between 0 and 3\n", argv[0]);
        return 1;
    }
    char data[5];
    data[1] = 'G'; data[2] = 'E'; data[3] = 'T'; data[4] = argv[1][0];
    // end of working code
    
    data[0]=checksum(5,data);
	send(5, data);
    // FIX ME -- add action if no reply
	
    waitForAllTimeouts();
}
