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
    
    //alter nbytes to be multiple of 8 
    if(nbytes % 8 != 0){
        nbytes = nbytes - (nbytes % 8) + 8;
    }


    struct block cur = freelist; 
    struct block allocated = NULL; 
    while(cur->next != NULL){
        if (cur->next->size == nbytes){
            allocated = cur->next; 
            cur->next = cur->next->next; 
        } else if(cur->size > nbytes){
            allocated = cur;  
        }    

        cur = cur->next; 
        
    }
    allocated->next = allocated_list; 
    return allocated;
}


int sfree(void *addr) {
    // check that addr is a valid block in allocated_list 
    struct block cur = allocated_list;
    int flag = 0; 
    while (cur != addr){
        cur = cur->next; 
        if (cur = addr){ 
            flag = 1; 
            break;
        } 
    }
    if (flag == 0){ return -1;} 
	//TODO
    // Remove allocated_list block and push to freelist
    // addr is the pointer to the block 
    while(cur->next != addr){
        // traverse to the block before the block to be removed 
        cur = cur->next; 
    } 
    cur->next = addr->next; 
    addr->next = freelist; 
    freelist = addr; 
    return -1;
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

	//TODO
        // mem = malloc(size); 
        // Create a new block and set address to mem 
    freelist = mem;
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
    cur = allocated_list->addr;
    while(cur->addr != NULL){
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

