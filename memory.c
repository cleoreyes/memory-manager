/* 
  Copyright 2024 Cleo Reyes
  CSE 374
  HW7
  memory.c
  Purpose: Memory management package that produces the same functionality as the standard library malloc and free functions.
*/

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include "mem.h"  // outward facing functions
#include "mem_internal.h"  // private functions

// Default values for us to use later on
#define NODESIZE sizeof(freeNode)
#define MINCHUNK 16        // smallest allowable chunk of memory
#define BIGCHUNK 16000     // default of a very large chunk size

// Global variables for convenience
// these are static so outside code can't use them.
static freeNode* freeBlockList;  // points to list of available memory blocks
static uintptr_t totalMalloc;  // keeps track of memory allocated with malloc

/* The following functions need to be defined to meet the interface
   specified in mem.h.  These functions return or take the 'usable'
   memory addresses that a user would deal with.  They are called
   in the bench code.
*/

/* getmem returns the address of a usable block of memory that is
   at least size bytes large.  This code calls the helper function
   'get_block'
   Pre-condition: size is a positive integer
*/
void* getmem(uintptr_t size) {
  assert(size > 0);

  // make sure size is a multiple of MINCHUNK (16):
  if (size % MINCHUNK != 0) {
    size = size + MINCHUNK -(size % MINCHUNK);
  }

  uintptr_t block = get_block(size);
  if (block == 0) {
    return NULL;
  }

  return((void*)(block+NODESIZE));  // offset to get usable address
}

/* freemem uses the functions developed to add blocks to the 
   list of available free blocks to return a node to the list.
   The pointer 'p' is the address of usable memory, allocated using getmem
*/
void freemem(void* p) {
  if (p == NULL) {
    return;
  }

  // Get to the beginning of the block
  uintptr_t blockAddress = (uintptr_t)p - NODESIZE;

  return_block(blockAddress);
}

// Get a block of desired size
uintptr_t get_block(uintptr_t size) {
  freeNode* prev = NULL;
  freeNode* current = freeBlockList;

  // Traverse through the list until you find a block of suitable size
  while (current != NULL) {
    if (current->size >= size) {
      break;
    }
    prev = current;
    current = current->next;
  }

  if (current == NULL) {
    current = new_block(size);
  }

  if (current->size > size + NODESIZE + MINCHUNK) {
    split_node(current, size);
  }

  // Remove the block from the free list
  if (prev == NULL) {
    freeBlockList = current->next;
  } else {
    prev->next = current->next;
  }
  return (uintptr_t)current;
}

// Create a new block that is sufficient for the provided size
freeNode* new_block(int size) {
  uintptr_t blockSize = 0;
  if (BIGCHUNK + NODESIZE > size + NODESIZE) {
    blockSize = BIGCHUNK + NODESIZE;
  } else {
    blockSize = size + NODESIZE;
  }

  freeNode* newBlock = (freeNode*) malloc(blockSize);
  if (newBlock == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory.");
    exit(EXIT_FAILURE);
  }
  newBlock->size = blockSize - NODESIZE;

  return_block((uintptr_t)newBlock);

  return newBlock;
}

// Split a node to the desired size and its leftover
void split_node(freeNode* n, uintptr_t size) {
  uintptr_t leftoverSize = n->size - size - NODESIZE;
  freeNode* leftoverNode = (freeNode*) ((uintptr_t)n + NODESIZE + size);

  leftoverNode->size = leftoverSize;
  if (n->next != NULL) {
    leftoverNode->next = n->next;
  }

  n->size = size;
  n->next = leftoverNode;
}

// Return the block back into the list of free blocks
void return_block(uintptr_t node) {
  freeNode* returnedNode = (freeNode*)node;
  if (!freeBlockList) {
    freeBlockList = returnedNode;
    freeBlockList->next = NULL;
    return;
  }

  freeNode* prev = NULL;
  freeNode* current = freeBlockList;

  // Traverse through list to find correct placement for returned node
  while (current && ((uintptr_t)current < (uintptr_t)returnedNode)) {
    prev = current;
    current = current->next;
  }

  // Adjust pointers
  if (prev == NULL) {
    freeBlockList = returnedNode;
  } else {
    prev->next = returnedNode;
  }
  returnedNode->next = current;

  // Merge adjacent nodes if necessary
  if (adjacent(returnedNode)) {
    returnedNode->size += NODESIZE + returnedNode->next->size;
    returnedNode->next = returnedNode->next->next;
  }
  if (prev != NULL && adjacent(prev)) {
    prev->size += NODESIZE + returnedNode->size;
    prev->next = returnedNode->next;
  }
}

// Indicates if node is adjacent to its next or not
int adjacent(freeNode* node) {
  if (node == NULL || node->next == NULL) {
    return 0;
  }

  uintptr_t endOfCurrentBlock = (uintptr_t)node + NODESIZE + node->size;
  uintptr_t startOfNextBlock = (uintptr_t)node->next;

  if (endOfCurrentBlock == startOfNextBlock) {
    return 1;
  }
  return 0;
}

/* The following are utility functions that may prove useful to you.
   They should work as presented, so you can leave them as is.
*/
void check_heap() {
  if (!freeBlockList) return;
  freeNode* currentNode = freeBlockList;
  uintptr_t minsize = currentNode->size;

  while (currentNode != NULL) {
    if (currentNode->size < minsize) {
      minsize = currentNode->size;
    }
    if (currentNode->next != NULL) {
      assert((uintptr_t)currentNode <(uintptr_t)(currentNode->next));
      assert((uintptr_t)currentNode + currentNode->size + NODESIZE
              <(uintptr_t)(currentNode->next));
    }
    currentNode = currentNode->next;
  }
  // go through free list and check for all the things
  if (minsize == 0) print_heap( stdout);
  assert(minsize >= MINCHUNK);
}

void get_mem_stats(uintptr_t* total_size, uintptr_t* total_free,
                   uintptr_t* n_free_blocks) {
  *total_size = totalMalloc;
  *total_free = 0;
  *n_free_blocks = 0;

  freeNode* currentNode = freeBlockList;
  while (currentNode) {
    *n_free_blocks = *n_free_blocks + 1;
    *total_free = *total_free + (currentNode->size + NODESIZE);
    currentNode = currentNode->next;
  }
}

void print_heap(FILE *f) {
  printf("Printing the heap\n");
  freeNode* currentNode = freeBlockList;
  while (currentNode !=NULL) {
    fprintf(f, "%" PRIuPTR, (uintptr_t)currentNode);
    fprintf(f, ", size: %" PRIuPTR, currentNode->size);
    fprintf(f, ", next: %" PRIuPTR, (uintptr_t)currentNode->next);
    fprintf(f, "\n");
    currentNode = currentNode->next;
  }
}
