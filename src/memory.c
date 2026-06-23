#include "memory.h"

void memory_init(chip8_memory *mem)
{
    (void)mem;
    // TODO (§3.1, §3.3): zero all of mem->ram[], then call memory_load_font(mem).
}

void memory_load_font(chip8_memory *mem)
{
    (void)mem;
    // TODO (§3.3, §7.3): write the 16 × 5-byte glyphs from the §7.3 table into
    //   mem->ram[CHIP8_FONT_START .. CHIP8_FONT_START + 79].
    //   Digit 0: F0 90 90 90 F0  …  Digit F: F0 80 F0 80 80.
}

int memory_load_rom(chip8_memory *mem, const uint8_t *rom, size_t len)
{
    (void)mem;
    (void)rom;
    (void)len;
    // TODO (§4, §13): if len > CHIP8_MEM_SIZE - CHIP8_ROM_START return -1;
    //   otherwise memcpy(mem->ram + CHIP8_ROM_START, rom, len), return 0.
    return 0;
}

uint8_t memory_read_byte(const chip8_memory *mem, uint16_t addr)
{
    (void)mem;
    (void)addr;
    // TODO (§3.1, §13): return mem->ram[addr & 0xFFF].
    return 0;
}

void memory_write_byte(chip8_memory *mem, uint16_t addr, uint8_t value)
{
    (void)mem;
    (void)addr;
    (void)value;
    // TODO (§3.1, §13): mem->ram[addr & 0xFFF] = value.
}
