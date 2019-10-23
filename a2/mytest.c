// This custom test case will show how the functions mem_init, smalloc, sfree and mem_free work. 
// Also demonstrated is overflow from one block to its adjacent block. 

#include <stdio.h>                                                                                              
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"

#include <string.h>

#define SIZE 8 * 3

int main(){
    printf("The total size reserved is %d\n", SIZE); 
    mem_init(SIZE); 
    void* ptr = smalloc(8); 
    void* ptr2 = smalloc(16);
    puts("Allocated blocks:");
    print_allocated(); 
    puts("Free blocks:");
    print_free(); 
    puts("Attempt to smalloc a 16 byte block:");
    if (smalloc(16) == NULL){
        puts("Unable to smalloc 16 byte block.");
    }    
    printf("We demonstrate overflow by writing 14 characters into %p\n", ptr);
    strncpy(ptr, "14 char string", 14);
    puts("Allocated memory contents:");
    print_mem();
    printf("The stored string at %p: %s\n", ptr, ptr);
    puts("Freeing all allocated blocks.");
    sfree(ptr); ptr = NULL; 
    sfree(ptr2); ptr2 = NULL; 
    puts("See that all sfree block data is reset:");
    ptr = smalloc(8); 
    ptr2 = smalloc(16); 
    print_allocated();
    puts("Allocated memory contents:");
    print_mem(); 
    mem_clean();
    return 0;
}
