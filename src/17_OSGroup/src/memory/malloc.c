#include "memory/memory.h"
#include "libc/system.h"

#define MAX_PAGE_ALIGNED_ALLOCS 32

uint32_t last_alloc = 0;
uint32_t heap_end = 0;
uint32_t heap_begin = 0;
uint32_t pheap_begin = 0;
uint32_t pheap_end = 0;
uint8_t *pheap_desc = 0;
uint32_t memory_used = 0;

// Initialize the kernel memory manager
void init_kernel_memory(uint32_t* kernel_end)
{
    // Set the starting address for the heap, just after the kernel end
    last_alloc = (uint32_t)kernel_end + 0x1000;
    heap_begin = last_alloc;

    // Define the end of the page-aligned heap
    pheap_end = 0x400000;  // 4 MB
    pheap_begin = pheap_end - (MAX_PAGE_ALIGNED_ALLOCS * 4096);
    heap_end = pheap_begin;

    // Clear the heap memory
    memset((char *)heap_begin, 0, heap_end - heap_begin);

    // Allocate memory for the page-aligned heap descriptor
    pheap_desc = (uint8_t *)malloc(MAX_PAGE_ALIGNED_ALLOCS);

    // Print the starting address of the kernel heap
    printf("Kernel heap starts at 0x%x\n", last_alloc);
}

// Print the current memory layout
void print_memory_layout()
{
    // Display memory usage statistics
    printf("Memory used: %d bytes\n", memory_used);
    printf("Memory free: %d bytes\n", heap_end - heap_begin - memory_used);
    printf("Heap size: %d bytes\n", heap_end - heap_begin);
    printf("Heap start: 0x%x\n", heap_begin);
    printf("Heap end: 0x%x\n", heap_end);
    printf("PHeap start: 0x%x\nPHeap end: 0x%x\n", pheap_begin, pheap_end);
}

// Free a block of memory
void free(void *mem)
{
    // Adjust the pointer to get the allocation header
    alloc_t *alloc = (alloc_t *)((uint8_t *)mem - sizeof(alloc_t));

    // Update memory usage and set the block status to free
    memory_used -= alloc->size + sizeof(alloc_t);
    alloc->status = 0;
}

// Free a block of page-aligned memory
void pfree(void *mem)
{
    // Check if the memory is within the page-aligned heap range
    if(mem < (void *)pheap_begin || mem > (void *)pheap_end) return;

    // Calculate the page ID
    uint32_t ad = (uint32_t)mem;
    ad -= pheap_begin;
    ad /= 4096;

    // Mark the page descriptor as free
    pheap_desc[ad] = 0;
}

// Allocate a block of page-aligned memory
char* pmalloc(size_t size)
{
    // Loop through the page descriptors to find a free page
    for(int i = 0; i < MAX_PAGE_ALIGNED_ALLOCS; i++)
    {
        if(pheap_desc[i]) continue;

        // Mark the page as allocated
        pheap_desc[i] = 1;

        // Print the allocated memory range
        printf("PAllocated from 0x%x to 0x%x\n", pheap_begin + i*4096, pheap_begin + (i+1)*4096);

        // Return the address of the allocated page
        return (char *)(pheap_begin + i*4096);
    }

    // Print an error message if allocation fails
    printf("pmalloc: FATAL: failure!\n");
    return 0;
}

// Allocate a block of memory
void* malloc(size_t size)
{
    // Return NULL if the requested size is zero
    if(!size) return 0;

    // Loop through existing allocations to find a suitable free block
    uint8_t *mem = (uint8_t *)heap_begin;
    while((uint32_t)mem < last_alloc)
    {
        alloc_t *a = (alloc_t *)mem;
        printf("mem=0x%x a={.status=%d, .size=%d}\n", mem, a->status, a->size);

        if(!a->size)
            goto nalloc;

        // If the block is allocated, move to the next block
        if(a->status) {
            mem += a->size;
            mem += sizeof(alloc_t);
            mem += 4; // Alignment padding
            continue;
        }

        // If the block is free and big enough, allocate it
        if(a->size >= size)
        {
            a->status = 1;
            printf("RE:Allocated %d bytes from 0x%x to 0x%x\n", size, mem + sizeof(alloc_t), mem + sizeof(alloc_t) + size);
            memset(mem + sizeof(alloc_t), 0, size);
            memory_used += size + sizeof(alloc_t);
            return (char *)(mem + sizeof(alloc_t));
        }

        // Move to the next block
        mem += a->size;
        mem += sizeof(alloc_t);
        mem += 4; // Alignment padding
    }

    nalloc:;
    // If there is not enough space in the heap, trigger a panic
    if(last_alloc + size + sizeof(alloc_t) >= heap_end)
    {
        panic("Cannot allocate bytes! Out of memory.\n");
    }

    // Create a new allocation block
    alloc_t *alloc = (alloc_t *)last_alloc;
    alloc->status = 1;
    alloc->size = size;

    // Update the last allocation pointer
    last_alloc += size;
    last_alloc += sizeof(alloc_t);
    last_alloc += 4; // Alignment padding

    // Print the allocated memory range
    printf("Allocated %d bytes from 0x%x to 0x%x\n", size, (uint32_t)alloc + sizeof(alloc_t), last_alloc);

    // Update the memory usage counter
    memory_used += size + 4 + sizeof(alloc_t);

    // Clear the allocated memory
    memset((char *)((uint32_t)alloc + sizeof(alloc_t)), 0, size);

    // Return the address of the allocated memory
    return (char *)((uint32_t)alloc + sizeof(alloc_t));
}
