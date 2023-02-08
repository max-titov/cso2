#include <stdio.h>
#include <stdlib.h>
#include "primes.h"

int main(int argc, char *argv[])
{
    if(argc>=2){
        for(int i = 1; i<argc;i++){
            char *p;
            int conv = strtol(argv[i], &p, 10);
            pprime(conv);
        }
        return 0;
    }
    int a;
    while(scanf("%d", &a) != EOF){
        pprime(a);
    }
    
    return 0;
}