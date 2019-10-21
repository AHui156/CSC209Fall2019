/*
 * This custom test shows that a data type with size that is larger than the allocated block
 * it is written to will cause an overflow into the adjacent block memory.
 * A long double of size 10 bytes is written into the first block. 
 * A second block of 8 bytes is allocated. 
 * See that an overflow of 2 bytes has occured in the second block. 
 * A solution to this would be to prevent casting of void* pointers to data types larger than 8 bytes. 
 * 
 * We also see that mmap will reserve size of one page of 4096 bytes even though we specified a size 
 * less than one page. 
 * We again allocate a block of 8 bytes and assign the max long double value to it. Despite exceeding the 
 * SIZE passed into mmap - a segmentation fault does not occur. 
 * 
 */

#include <stdio.h>                                                                                              
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"

#include <float.h>
#include <limits.h>


#define SIZE 8 * 3

int main(){
    mem_init(SIZE); 
    void* ptr = smalloc(8); 
    printf("The size of a long double is: %lu bytes", sizeof(long double));
    *(long double*)ptr = LDBL_MAX; 
    void* ptr2 = smalloc(8);
    printf("The following blocks are allocated:\n");
    print_allocated();
    printf("Current memory contents\n");
    printf("-> Overflow into ptr2 has occured.\n");
    print_mem();

    void* ptr3 = smalloc(8);  
    *(long double*)ptr3 = LDBL_MAX; 
    printf("No segementation fault has occured.\n");
    print_mem();

    void* ptr4 = smalloc(4096 - (8 * 4)); 
    void* ptr5 = smalloc(8);
    printf("Segmentation fault will occur here.\n");
    *(long double*)ptr5 = LDBL_MAX; 
    mem_clean(); 
    return 0;
}
