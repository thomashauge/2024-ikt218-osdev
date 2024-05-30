#include "song/song.h"
#include "pit.h"
#include "common.h"
#include "libc/stdio.h"

// Function to enable the PC speaker
void enable_speaker(){
    // Read the current state of the PC speaker control register
    uint8_t speaker_state = inb(PC_SPEAKER_PORT);
    
    /*
    PC Speaker Control Register (port 0x61):
    Bit 0: Speaker gate
            0: Speaker disabled
            1: Speaker enabled
    Bit 1: Speaker data
            0: Data is not passed to the speaker
            1: Data is passed to the speaker
    */
    
    // If bits 0 and 1 are not both set, enable the speaker by setting bits 0 and 1 to 1
    if (speaker_state != (speaker_state | 3)) {
        outb(PC_SPEAKER_PORT, speaker_state | 3);
    }
}

// Function to disable the PC speaker
void disable_speaker() {
    // Read the current state of the PC speaker control register
    uint8_t speaker_state = inb(PC_SPEAKER_PORT);
    // Clear bits 0 and 1 to disable the speaker
    outb(PC_SPEAKER_PORT, speaker_state & 0xFC);
}

// Function to play a sound at a given frequency
void play_sound(uint32_t frequency) {
    if (frequency == 0) {
        return; // If frequency is 0, return without doing anything
    }

    // Calculate the divisor for the desired frequency
    uint16_t divisor = (uint16_t)(PIT_BASE_FREQUENCY / frequency);

    // Set up the PIT to the desired frequency
    // Command byte: Channel 2, Access mode: lobyte/hibyte, Operating mode: 3 (square wave), Binary mode: 0
    outb(PIT_CMD_PORT, 0b10110110);
    // Send the low byte of the divisor
    outb(PIT_CHANNEL2_PORT, (uint8_t)(divisor & 0xFF));
    // Send the high byte of the divisor
    outb(PIT_CHANNEL2_PORT, (uint8_t)(divisor >> 8));

    // Enable the speaker by setting bits 0 and 1 of the control register
    uint8_t speaker_state = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, speaker_state | 0x03);
}

// Function to stop any sound being played by the PC speaker
void stop_sound(){
    // Read the current state of the PC speaker control register
    uint8_t speaker_state = inb(PC_SPEAKER_PORT);
    // Clear bits 0 and 1 to disable the speaker
    outb(PC_SPEAKER_PORT, speaker_state & ~0x03);
}

// Function to play a song by iterating through its notes
void play_song_impl(Song *song) {
    // Enable the speaker before starting the song
    enable_speaker();
    for (uint32_t i = 0; i < song->length; i++) {
        Note* note = &song->notes[i]; // Get the current note
        printf("Note: %d, Freq=%d, Sleep=%d\n", i, note->frequency, note->duration);
        play_sound(note->frequency); // Play the note's frequency
        sleep_interrupt(note->duration); // Delay for the note's duration
        stop_sound(); // Stop the sound after the note's duration
    }
    // Disable the speaker after finishing the song
    disable_speaker();
}

// Function to play a song, acts as a wrapper around play_song_impl
void play_song(Song *song) {
    play_song_impl(song);
}
