#include "pit.h"
#include "interrupts.h"
#include "common.h"

static uint32_t ticks = 0;  // Variable to keep track of the number of ticks

// IRQ handler function for the PIT (Programmable Interval Timer)
void pit_irq_handler(registers_t* regs, void* context) {
    ticks++;  // Increment the tick count on each timer interrupt
}

// Function to initialize the PIT
void init_pit() {
    // Register the IRQ handler for the PIT (IRQ0)
    register_irq_handler(IRQ0, pit_irq_handler, NULL);

    // Send the command byte to the PIT command port
    outb(PIT_CMD_PORT, 0x36);

    // Calculate the frequency divisor for the desired timer frequency
    uint8_t l_divisor = (uint8_t)(PIT_BASE_FREQUENCY / TARGET_FREQUENCY);
    uint8_t h_divisor = (uint8_t)((PIT_BASE_FREQUENCY / TARGET_FREQUENCY) >> 8);

    // Send the frequency divisor to the PIT channel 0 data port
    outb(PIT_CHANNEL0_PORT, l_divisor);  // Lower byte of divisor
    outb(PIT_CHANNEL0_PORT, h_divisor);  // Upper byte of divisor
}

// Function to sleep for a specified number of milliseconds using interrupts
void sleep_interrupt(uint32_t milliseconds){
    uint32_t current_tick = ticks;  // Get the current tick count
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;  // Calculate the number of ticks to wait
    uint32_t end_ticks = current_tick + ticks_to_wait;  // Calculate the tick count at which to stop waiting

    while (current_tick < end_ticks) {
        // Enable interrupts (sti)
        asm volatile("sti");
        // Halt the CPU until the next interrupt (hlt)
        asm volatile("hlt");
        current_tick = ticks;  // Update the current tick count
    }
}

// Function to sleep for a specified number of milliseconds using busy-waiting
void sleep_busy(uint32_t milliseconds){
    uint32_t start_tick = ticks;  // Get the starting tick count
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;  // Calculate the number of ticks to wait
    uint32_t elapsed_ticks = 0;  // Initialize the elapsed tick count
    
    while (elapsed_ticks < ticks_to_wait) {
        // Busy-wait until the tick count increases
        while (ticks == start_tick + elapsed_ticks) {};
        elapsed_ticks++;  // Increment the elapsed tick count
    }
}
