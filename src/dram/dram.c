#include <stdint.h>

void store_word(unsigned char *dram, uint32_t base_addr, uint32_t val) {
  dram[base_addr] = val & 0xFF;
  dram[base_addr + 1] = (val >> 8) & 0xFF;
  dram[base_addr + 2] = (val >> 16) & 0xFF;
  dram[base_addr + 3] = (val >> 24) & 0xFF;
}

void store_half_word(unsigned char *dram, uint32_t base_addr, uint16_t val) {
  dram[base_addr] = val & 0xFF;
  dram[base_addr + 1] = (val >> 8);
}

void store_byte(unsigned char *dram, uint32_t base_addr, uint8_t val) {
  dram[base_addr] = val;
}

uint32_t load_word(unsigned char *dram, uint32_t base_addr) {
  return dram[base_addr]
          | dram[base_addr + 1] << 8
          | dram[base_addr + 2] << 16
          | dram[base_addr + 3] << 24;
}

uint32_t load_half_word(unsigned char *dram, uint32_t base_addr) {
  return dram[base_addr]
          | dram[base_addr + 1] << 8;
}

uint32_t load_byte(unsigned char *dram, uint32_t base_addr) {
  return dram[base_addr];
}
