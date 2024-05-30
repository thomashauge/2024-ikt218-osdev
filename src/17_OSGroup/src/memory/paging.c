#include "libc/system.h"
#include "memory/memory.h"

static uint32_t* page_directory = 0;   // Pointer to the page directory, initialized to zero
static uint32_t page_dir_loc = 0;      // Location of the page directory, initialized to zero
static uint32_t* last_page = 0;        // Pointer to the last page, initialized to zero

/* Paging will be set up as follows:
 * - Reserve 0-8MB for kernel usage
 * - Heap will be from approximately 1MB to 4MB
 * - Paging structures will start from 4MB
 */

// Function to map virtual addresses to physical addresses
void paging_map_virtual_to_phys(uint32_t virt, uint32_t phys)
{
    uint16_t id = virt >> 22;        // Extract the upper 10 bits of the virtual address to use as an index in the page directory
    for(int i = 0; i < 1024; i++)    // Loop through all 1024 entries in the page table
    {
        last_page[i] = phys | 3;    // Set the page table entry to the physical address with the present and write permissions
        phys += 4096;               // Increment the physical address by the page size (4 KB)
    }
    // Set the page directory entry to point to the page table with present and write permissions
    page_directory[id] = ((uint32_t)last_page) | 3;
    // Move to the next page table in memory
    last_page = (uint32_t *)(((uint32_t)last_page) + 4096);
}

// Function to enable paging
void paging_enable()
{
    asm volatile("mov %%eax, %%cr3" : : "a"(page_dir_loc)); // Load the physical address of the page directory into the CR3 register
    asm volatile("mov %cr0, %eax");         // Load the CR0 register into the EAX register
    asm volatile("orl $0x80000000, %eax");  // Set the paging enable bit in the EAX register
    asm volatile("mov %eax, %cr0");         // Load the EAX register back into the CR0 register to enable paging
}

// Function to initialize paging
void init_paging()
{
    printf("Setting up paging\n");
    page_directory = (uint32_t*)0x400000;      // Set the page directory to start at 4 MB
    page_dir_loc = (uint32_t)page_directory;   // Set the physical address of the page directory
    last_page = (uint32_t *)0x404000;          // Set the last page to start at 4 MB + 4 KB (for the page table)
    for(int i = 0; i < 1024; i++)              // Loop through all 1024 entries in the page directory
    {
        page_directory[i] = 0 | 2;             // Set the page directory entry to not present with supervisor-level read/write permissions
    }
    // Map the first 4 MB of virtual memory to the first 4 MB of physical memory
    paging_map_virtual_to_phys(0, 0);
    // Map the next 4 MB of virtual memory to the next 4 MB of physical memory
    paging_map_virtual_to_phys(0x400000, 0x400000);
    // Enable paging
    paging_enable();
    printf("Paging was successfully enabled!\n");
}
