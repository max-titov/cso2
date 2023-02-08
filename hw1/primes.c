#include <stdio.h>
#include <stdbool.h>

int isprime(int x){
    if(x==0) return 0;
    if(x<=2) return 1;
    for(int i = 2;i<=x/2;i++){
        if(x%i==0){
            return 0;
        }
    }
    return 1;
}

int nextprime(int x)
{
    while(1){
        x++;
        if(!isprime(x)){
            continue;
        } 
        return x;
    }
    
}

void pprime(int x){
    if(isprime(x)){
        printf("%i is prime\n",x);
    }else{
        printf("%i is not prime, but %i is\n",x,nextprime(x));
    }
}