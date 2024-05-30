#include "descriptor_tables.h"

// External function to load the GDT, implemented in assembly
extern void gdt_flush(uint32_t gdt_ptr);

// Function to initialize the Global Descriptor Table (GDT)
void init_gdt() {
    // Set the GDT limit and base address
    gdt_ptr.limit = sizeof(struct gdt_entry_t) * GDT_ENTRIES - 1;
    gdt_ptr.base = (uint32_t)&gdt;

    // Set up the first GDT entry as a null segment (required)
    gdt_set_gate(0, 0, 0, 0, 0); // Null segment

    // Set up the second GDT entry as a code segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment: base=0, limit=4GB, access=0x9A, granularity=0xCF

    // Set up the third GDT entry as a data segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment: base=0, limit=4GB, access=0x92, granularity=0xCF

    // Set up the fourth GDT entry as a user mode code segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment: base=0, limit=4GB, access=0xFA, granularity=0xCF

    // Set up the fifth GDT entry as a user mode data segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment: base=0, limit=4GB, access=0xF2, granularity=0xCF

    // Load the GDT into the CPU using the gdt_flush function
    gdt_flush((uint32_t)&gdt_ptr);
}

// Function to set the value of a GDT entry
void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    // Set the base address of the segment
    gdt[num].base_low_part = (base & 0xFFFF); // Lower 16 bits of the base address
    gdt[num].base_middle_part = (base >> 16) & 0xFF; // Next 8 bits of the base address
    gdt[num].base_high_byte = (base >> 24) & 0xFF; // Upper 8 bits of the base address

    // Set the limit of the segment
    gdt[num].limit_low_part = (limit & 0xFFFF); // Lower 16 bits of the limit
    gdt[num].granularity_byte = (limit >> 16) & 0x0F; // Upper 4 bits of the limit

    // Set the granularity and access flags
    gdt[num].granularity_byte |= gran & 0xF0; // Set the granularity bits
    gdt[num].access_byte = access; // Set the access flags
}
