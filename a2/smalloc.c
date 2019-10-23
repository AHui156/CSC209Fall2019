#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"
#include <string.h>

void *mem;
struct block *freelist;
struct block *allocated_list;

void *smalloc(unsigned int nbytes) {
    // Check if nbytes is a multiple of 8, otherwise set to next highest multiple of 8
    if(nbytes % 8 != 0){
        nbytes = nbytes - (nbytes % 8) + 8;
    }
    
    // Traverses freelist to find the first block that is >= nbytes. 
    // If block size == nbytes, remove from freelist and push to allocated_list
    // If block size > nbytes, split block, remove from freelist and push to allocated_list
    struct block* cur_free = freelist; 
    struct block* prev_free = NULL;
    struct block* allocated = NULL;
    while(cur_free != NULL){

        if (cur_free->size == nbytes){
            if (prev_free != NULL){
                prev_free->next = cur_free->next;
            } else {
                freelist = cur_free->next; 
            }
            allocated = cur_free; 
            allocated->next = allocated_list; 
            allocated_list = allocated;
            break;
            
        } else if(cur_free->size > nbytes){
            allocated = malloc(sizeof(struct block)); 
            allocated->addr = cur_free->addr; 
            allocated->size = nbytes; 
            cur_free->addr += nbytes; 
            cur_free->size -= nbytes;    
            allocated->next = allocated_list; 
            allocated_list = allocated;
            break;
        }    
        prev_free = cur_free;
        cur_free = cur_free->next; 
    }
    if (allocated != NULL) { return allocated->addr;}
    else {return NULL;}
}

/* 
 * Resets all data in the address to 0.
 * Removes block with specified addr from allocated_list. 
 * Traverses freelist to insert block where addr order is preserved. 
 */
int sfree(void *addr) {
    // Traverse allocated_list and removes specified block, returns -1 if not found
    struct block* cur_allocated = allocated_list;
    struct block* previous_allocated = NULL;
    int flag = 0; 
    while (cur_allocated != NULL){
        if (cur_allocated->addr == addr){ 
            flag = 1; 
            break;
        } 
        previous_allocated = cur_allocated;
        cur_allocated = cur_allocated->next; 
    }
    if (flag == 0){ return -1; } 

    // Reset all data in the address to 0. 
    memset(addr, 0, cur_allocated->size);

    // Remove the block from allocated_list
    if (previous_allocated == NULL){
        allocated_list = cur_allocated->next;
    } else {
        previous_allocated->next = cur_allocated->next; 
    }

    // Insert the block into freelist at appropriate location
    struct block* cur_free = freelist;
    struct block* previous_free = NULL;  
    if (freelist == NULL){ 
        freelist = cur_allocated; 
        return 0;
    }
    while(cur_free != NULL){
        if(cur_allocated->addr < cur_free->addr){
            break;
        }
        previous_free = cur_free;
        cur_free = cur_free->next; 
    }
    cur_allocated->next = cur_free; 
    if (previous_free != NULL) { previous_free->next = cur_allocated; } 
    else { freelist = cur_allocated; }
    return 0;
}


/* Initialize the memory space used by smalloc,
 * freelist, and allocated_list
 * Note:  mmap is a system call that has a wide variety of uses.  In our
 * case we are using it to allocate a large region of memory. 
 * - mmap returns a pointer to the allocated memory
 * Arguments:
 * - NULL: a suggestion for where to place the memory. We will let the 
 *         system decide where to place the memory.
 * - PROT_READ | PROT_WRITE: we will use the memory for both reading
 *         and writing.
 * - MAP_PRIVATE | MAP_ANON: the memory is just for this process, and 
 *         is not associated with a file.
 * - -1: because this memory is not associated with a file, the file 
 *         descriptor argument is set to -1
 * - 0: only used if the address space is associated with a file.
 */
void mem_init(int size) {
    mem = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if(mem == MAP_FAILED) {
         perror("mmap");
         exit(1);
    }

    freelist = malloc(sizeof(struct block)); 
    freelist->addr = mem; 
    freelist->size = size; 
    freelist->next = NULL; 
    allocated_list = NULL; 
}

/*
 * Traverses through allocated_list and freelist and frees all struct blocks
 */
void mem_clean(){
    struct block* cur = NULL;
    cur = allocated_list;
    while(cur != NULL){
        struct block* next = cur->next;
        free(cur);
        cur = next;
    }
   cur = freelist; 
   while(cur!= NULL){
       struct block* next = cur->next;
       free(cur);
       cur = next;
   }
}

