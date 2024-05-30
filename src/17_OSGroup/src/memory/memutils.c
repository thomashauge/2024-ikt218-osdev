#include "memory/memory.h"   // Include the header file that defines the functions

// Function to copy memory from source to destination
void* memcpy(void* dest, const void* src, size_t count )
{
    char* dst8 = (char*)dest;  // Cast the destination pointer to char*
    char* src8 = (char*)src;   // Cast the source pointer to char*

    // If the count is odd, copy the first byte separately
    if (count & 1) {
        dst8[0] = src8[0];     // Copy one byte from source to destination
        dst8 += 1;             // Increment destination pointer by one byte
        src8 += 1;             // Increment source pointer by one byte
    }

    // Divide the count by two to copy 2 bytes at a time
    count /= 2;
    while (count--) {
        // Copy each pair of bytes from source to destination
        dst8[0] = src8[0];     // Copy the first byte of the pair
        dst8[1] = src8[1];     // Copy the second byte of the pair

        dst8 += 2;             // Increment destination pointer by 2 bytes
        src8 += 2;             // Increment source pointer by 2 bytes
    }

    return (void*)dest;        // Return the destination pointer
}

// Function to set a block of memory with a 16-bit value
void* memset16 (void *ptr, uint16_t value, size_t num)
{
    uint16_t* p = (uint16_t*)ptr; // Cast the pointer to uint16_t*
    
    // Set each 2-byte element to the given value
    while(num--)
        *p++ = value;

    return ptr;               // Return the pointer to the block of memory
}

// Function to set a block of memory with a byte value
void* memset (void * ptr, int value, size_t num )
{
    unsigned char* p = (unsigned char*)ptr; // Cast the pointer to unsigned char*
    
    // Set each byte to the given value
    while(num--)
        *p++ = (unsigned char)value;

    return ptr;               // Return the pointer to the block of memory
}
