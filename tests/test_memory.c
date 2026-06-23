#include <unity.h>

#include "memory.h"

static chip8_memory mem;

static const uint8_t expected_font[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

void setUp(void)
{
    memory_init(&mem);
}

void tearDown(void)
{
}

// memory_init must zero all RAM; spot-check bytes before, between, and after the font region.
void test_init_zeroes_non_font_ram(void)
{
    // Font occupies CHIP8_FONT_START (0x050) to 0x09F; everything else must be zero.
    TEST_ASSERT_EQUAL_UINT8(0, mem.ram[0x000]);
    TEST_ASSERT_EQUAL_UINT8(0, mem.ram[CHIP8_FONT_START - 1]);
    TEST_ASSERT_EQUAL_UINT8(0, mem.ram[CHIP8_FONT_START + 80]);
    TEST_ASSERT_EQUAL_UINT8(0, mem.ram[0x1FF]);
    TEST_ASSERT_EQUAL_UINT8(0, mem.ram[0xFFF]);
}

// The 80-byte font table loaded at CHIP8_FONT_START must exactly match the §7.3 glyphs.
void test_font_bytes_match_spec(void)
{
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_font, &mem.ram[CHIP8_FONT_START], 80);
}

// ROM bytes must appear verbatim starting at CHIP8_ROM_START (0x200), and the function returns 0.
void test_load_rom_places_bytes_at_rom_start(void)
{
    uint8_t rom[] = {0xAB, 0xCD, 0xEF};
    int rc = memory_load_rom(&mem, rom, sizeof(rom));
    TEST_ASSERT_EQUAL_INT(0, rc);
    TEST_ASSERT_EQUAL_UINT8(0xAB, mem.ram[CHIP8_ROM_START]);
    TEST_ASSERT_EQUAL_UINT8(0xCD, mem.ram[CHIP8_ROM_START + 1]);
    TEST_ASSERT_EQUAL_UINT8(0xEF, mem.ram[CHIP8_ROM_START + 2]);
}

// A ROM that exactly fills program space (3584 bytes) is the boundary case that must succeed.
void test_load_rom_accepts_maximum_size(void)
{
    // Largest ROM that exactly fills program space.
    static uint8_t max_rom[CHIP8_MEM_SIZE - CHIP8_ROM_START];
    max_rom[0] = 0x12;
    max_rom[CHIP8_MEM_SIZE - CHIP8_ROM_START - 1] = 0x34;
    int rc = memory_load_rom(&mem, max_rom, sizeof(max_rom));
    TEST_ASSERT_EQUAL_INT(0, rc);
    TEST_ASSERT_EQUAL_UINT8(0x12, mem.ram[CHIP8_ROM_START]);
    TEST_ASSERT_EQUAL_UINT8(0x34, mem.ram[CHIP8_MEM_SIZE - 1]);
}

// A ROM one byte larger than program space must be rejected with return value -1.
void test_load_rom_rejects_oversized_rom(void)
{
    static uint8_t oversized[CHIP8_MEM_SIZE - CHIP8_ROM_START + 1];
    int rc = memory_load_rom(&mem, oversized, sizeof(oversized));
    TEST_ASSERT_EQUAL_INT(-1, rc);
}

// A value written to an address must be readable back from the same address.
void test_read_write_byte_round_trip(void)
{
    memory_write_byte(&mem, 0x300, 0x42);
    TEST_ASSERT_EQUAL_UINT8(0x42, memory_read_byte(&mem, 0x300));
}

// Addresses wider than 12 bits must be silently masked; bits 12+ are ignored on both read and
// write.
void test_read_write_byte_masks_to_12_bits(void)
{
    memory_write_byte(&mem, 0x1234, 0x99);
    TEST_ASSERT_EQUAL_UINT8(0x99, memory_read_byte(&mem, 0x0234));
    TEST_ASSERT_EQUAL_UINT8(0x99, memory_read_byte(&mem, 0x1234));
    TEST_ASSERT_EQUAL_UINT8(0x99, memory_read_byte(&mem, 0xF234));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_zeroes_non_font_ram);
    RUN_TEST(test_font_bytes_match_spec);
    RUN_TEST(test_load_rom_places_bytes_at_rom_start);
    RUN_TEST(test_load_rom_accepts_maximum_size);
    RUN_TEST(test_load_rom_rejects_oversized_rom);
    RUN_TEST(test_read_write_byte_round_trip);
    RUN_TEST(test_read_write_byte_masks_to_12_bits);
    return UNITY_END();
}
