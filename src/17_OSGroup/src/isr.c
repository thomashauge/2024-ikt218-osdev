#include "interrupts.h"
#include "libc/stdint.h"
#include "libc/stddef.h"


void register_interrupt_handler(uint8_t n, isr_t handler, void* context)
{
    int_handlers[n].handler = handler;
    int_handlers[n].data = context;
}

// This function is called by our ASM interrupt handler stub.
void isr_handler_function(registers_t regs)
{
    // This step is crucial. The processor sign-extends the 8-bit interrupt number
    // to a 32-bit value. Hence, if the most significant bit (0x80) is set, 
    // regs.int_no will become very large (around 0xffffff80).
    uint8_t int_no = regs.int_no & 0xFF;
    struct int_handler_t intrpt = int_handlers[int_no];
    if (intrpt.handler != 0)
    {
        // Call the registered handler if it exists.
        intrpt.handler(&regs, intrpt.data);
    }
    else
    {
        // Uncomment these lines to log unhandled interrupts.
        /*monitor_write("Unhandled interrupt: ");
        monitor_write_hex(int_no);
        monitor_put('\n');*/
        
        // Infinite loop to halt execution for unhandled interrupts.
        for(;;);
    }
}
