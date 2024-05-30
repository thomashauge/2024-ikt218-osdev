#ifndef DESCRIPTOR_TABLES_H
#define DESCRIPTOR_TABLES_H
#include "libc/system.h"
#define GDT_ENTRIES 5
#define IDT_ENTRIES 256
// This defines the GDT entry
struct gdt_entry_t {
  uint16_t limit_low_part;
  uint16_t base_low_part;
  uint8_t base_middle_part;
  uint8_t access_byte;
  uint8_t granularity_byte;
  uint8_t base_high_byte;
} __attribute__((packed));

// This defines the IDT entry
struct idt_entry_t {
  uint16_t base_low_part;
  uint16_t selector_segment;
  uint8_t reserved;
  uint8_t flags_bits;
  uint16_t base_high_part;
} __attribute__((packed));

// Definning both the GDT and IDT pointers
struct gdt_ptr_t {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed));

struct idt_ptr_t {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed));

// Initialize the GDT and IDT
void init_gdt();
void init_idt();

// Loading the GDT and IDT
void gdt_load();
void idt_load();

void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

static struct idt_entry_t idt[IDT_ENTRIES];
static struct idt_ptr_t idt_ptr;
static struct gdt_entry_t gdt[GDT_ENTRIES];
static struct gdt_ptr_t gdt_ptr;

#endif // DESCRIPTOR_TABLES_H
