#include <multiboot2.h>

#include "libc/stdint.h"
#include "libc/stddef.h"
#include "libc/stdbool.h"
#include "libc/system.h"

#include "pit.h"
#include "common.h"
#include "descriptor_tables.h"
#include "interrupts.h"
#include "monitor.h"
#include "memory/memory.h"

// Structure to hold multiboot information provided by the bootloader
struct multiboot_info {
    uint32_t size;                // Size of the multiboot info structure
    uint32_t reserved;            // Reserved field
    struct multiboot_tag *first;  // Pointer to the first multiboot tag
};

// Forward declaration for the C++ kernel main function
int kernel_main();

// End of the kernel image, defined elsewhere
extern uint32_t end;

// Main entry point for the kernel, called from boot code
// magic: The multiboot magic number, should be MULTIBOOT2_BOOTLOADER_MAGIC
// mb_info_addr: Pointer to the multiboot information structure
int kernel_main_c(uint32_t magic, struct multiboot_info* mb_info_addr) {
    // Initialize the monitor for screen output
    monitor_initialize();
  
    // Set up the Global Descriptor Table (GDT) for segment management
    init_gdt();

    // Set up the Interrupt Descriptor Table (IDT) for interrupt handling
    init_idt();

    // Enable hardware interrupt handling
    init_irq();

    // Initialize the kernel's memory manager, using the end address of the kernel image
    init_kernel_memory(&end);

    // Set up paging for memory management
    init_paging();

    // Print the memory layout to the monitor for debugging
    print_memory_layout();

    // Initialize the Programmable Interval Timer (PIT) for system timing
    init_pit();

    // Print a hello world message to the monitor
    printf("Hello World!\n");
    

    // Call the main function of the kernel written in C++
    return kernel_main();
}
