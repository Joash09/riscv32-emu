#ifndef DRAM_H_
#define DRAM_H_

#include <stdint.h>

// Load into memory
void store_word(unsigned char*, uint32_t, uint32_t);
void store_half_word(unsigned char*, uint32_t, uint16_t);
void store_byte(unsigned char*, uint32_t, uint8_t);

// Targeting 32 bit CPU registers
uint32_t load_word(unsigned char*, uint32_t);
uint16_t load_half_word(unsigned char*, uint32_t);
uint8_t load_byte(unsigned char*, uint32_t);

#endif // DRAM_H_
