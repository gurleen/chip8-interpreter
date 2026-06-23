#ifndef CHIP8_MEMORY_H
#define CHIP8_MEMORY_H

#include <stddef.h>
#include <stdint.h>

#define CHIP8_MEM_SIZE 4096 // §3.1
#define CHIP8_ROM_START 0x200 // §3.2 — default load address
#define CHIP8_FONT_START 0x050 // §3.3 — recommended font location

typedef struct
{
    uint8_t ram[CHIP8_MEM_SIZE];
} chip8_memory;

// TODO (§3.1, §3.3): zero all of RAM, then call memory_load_font().
void memory_init(chip8_memory *mem);

// TODO (§3.3, §7.3): copy the 80-byte built-in font table from §7.3 into
//                    ram[CHIP8_FONT_START .. CHIP8_FONT_START + 79].
void memory_load_font(chip8_memory *mem);

// TODO (§4, §13): copy `len` ROM bytes into ram[CHIP8_ROM_START]; return -1
//                 if the ROM is too large to fit in program space.
int memory_load_rom(chip8_memory *mem, const uint8_t *rom, size_t len);

// TODO (§3.1, §13): bounds-checked single-byte read; mask addr to 12 bits.
uint8_t memory_read_byte(const chip8_memory *mem, uint16_t addr);

// TODO (§3.1, §13): bounds-checked single-byte write; mask addr to 12 bits.
void memory_write_byte(chip8_memory *mem, uint16_t addr, uint8_t value);

#endif /* CHIP8_MEMORY_H */
