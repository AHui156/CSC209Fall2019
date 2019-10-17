// Some possible things to test for: 
// 1. Writing too much data into one block - overflow into adjacent block 
// 2. Writing past reserved memory - segfault11 
// 3. Writing a data type that's too big - use long double (10 bytes) - overflow into adjacent block 
// 4. Writing in an array that's too long
// We wish to test the limits of mmap and the limits of our smalloc system
// What happens when we write beyond the page? 

#include <stdio.h>                                                                                              
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"


#define SIZE 4096 * 64 

int main(){
    mem_init(SIZE); 
    void* ptr = smalloc(8); 
    void* ptr2 = smalloc(8);
    long double value = 0.001;
    *(long double*)ptr = 0.001; 
    printf("The following blocks are allocated:\n");
    print_allocated();
    print_mem(); 
    mem_clean(); 
    return 0;
}
