#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"



void *mem;
struct block *freelist;
struct block *allocated_list;


void *smalloc(unsigned int nbytes) {
	//TODO
    // 1. Search for blocks in freelist that are >= nbytes 
    // 2. If none found - return NULL, else 
    // 3. If equal size, remove freelist block and push to allocated_list 
    // 4. If > size, split freelist block into nbytes and leftover, then remove nbytes block and push to 
    // allocated_list  
    
    // Check if nbytes is a multiple of 8, otherwise set to next highest multiple of 8
    if(nbytes % 8 != 0){
        nbytes = nbytes - (nbytes % 8) + 8;
    }
    
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
            // Create and set new block 
            // Advance freelist block addr by nbytes 
            // Reduce freelist block size  
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
    // push allocated block into allocated_list
    if (allocated != NULL) { return allocated->addr;}
    else {return NULL;}
}


int sfree(void *addr) {
    // check that addr is a valid block in allocated_list 
    // and set to cur 
    struct block* cur = allocated_list;
    int flag = 0; 
    while (cur->addr != addr){
        if (cur->addr == addr){ 
            flag = 1; 
            break;
        } 
        cur = cur->next; 
    }
    if (flag == 0){ return -1;} 
	//TODO
    // Remove allocated_list block and push to freelist
    struct block* previous = allocated_list;
    while(previous->next != cur){
        previous = previous->next;
    } 
    previous->next = cur->next;
    cur->next = freelist; 
    freelist = cur; 
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

    // Initialize first freelist block
    freelist = malloc(sizeof(struct block)); 
    freelist->addr = mem; 
    freelist->size = size; 
    freelist->next = NULL; 
    allocated_list = NULL; 
}

void mem_clean(){
	//TODO
        // free memory in freelist and allocated_list 
    // Have to traverse and free from the end. 
    // sfree all allocated, then free all free?
    struct block* cur = NULL;
    cur = allocated_list;
    while(cur != NULL){
        struct block* next = cur->next;
        free(cur);
        cur = next;
    }
   cur = freelist->addr; 
   while(cur->addr != NULL){
       struct block* next = cur->next;
       free(cur);
       cur = next;
   }
}

