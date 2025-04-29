# Memory Management Library ￼

## Overview ￼

This project is a custom memory management library for C, providing dynamic memory allocation and deallocation functions similar to the standard ‎`malloc` and ‎`free`. The library is designed to help developers understand and experiment with low-level memory management, pointer manipulation, and free list data structures.

## Features ￼
- Custom Allocation: Implements ‎`getmem(size_t size)` to allocate memory blocks aligned to 16 bytes.
- Custom Deallocation: Implements ‎`freemem(void* p)` to free memory and coalesce adjacent free blocks.
- Free List Management: Uses a linked list to track free memory blocks, minimizing fragmentation.
- Memory Statistics: Provides functions to report total memory usage, free memory, and the number of free blocks.
- Debugging Tools: Includes utilities to print the heap state and check the integrity of the free list.

## File Structure ￼
- ‎`mem.h` — Public header for the memory manager interface.
- ‎`mem_internal.h` — Internal header for helper functions.
- ‎`memory.c` — Main implementation file.
- ‎`bench.c` — Benchmarking and testing program.
- ‎`Makefile` — Build and test automation.
- `clint.py` — Style checker for code quality.

## Usage ￼

### Building ￼

To build the library and run the benchmark:￼

### Functions ￼
- ‎`void* getmem(size_t size);`: Allocates a block of at least ‎`size` bytes, aligned to 16 bytes. Returns ‎`NULL` if the request cannot be satisfied.
- `void freemem(void* p);`: Frees a previously allocated block. Adjacent free blocks are merged to reduce fragmentation.
- ‎`void get_mem_stats(size_t* total_size, size_t* total_free, size_t* n_free_blocks);`: Retrieves statistics about memory usage.
- ‎`void print_heap(FILE* f);`: Prints the current state of the free list.
- ‎`void check_heap();`: Checks the integrity of the free list using assertions.

### Implementation Details ￼
 • All memory blocks are aligned to 16 bytes for compatibility and performance.
 • The free list is kept sorted by memory address to simplify merging adjacent blocks.
 • When a large enough block is not available, the library requests a new chunk from the system using ‎`malloc`.
 • Block headers store the size of each block, enabling efficient merging and management.

Example ￼￼

Development Notes ￼
 • Designed for 64-bit systems; uses ‎`uintptr_t` for pointer arithmetic.
 • Extensively tested with the included ‎`bench.c` program.
 • Code style checked with ‎`clint.py`.
