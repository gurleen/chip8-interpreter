#include <unity.h>

#include "memory.h"
#include "registers.h"

static chip8_registers regs;

void setUp(void)
{
    registers_init(&regs, CHIP8_ROM_START);
}

void tearDown(void)
{
}

// All sixteen V registers must be zeroed after init.
void test_init_zeroes_v_registers(void)
{
    for (int i = 0; i < CHIP8_NUM_REGS; i++)
    {
        TEST_ASSERT_EQUAL_UINT8(0, regs.v[i]);
    }
}

// I, SP, DT, and ST must all be zeroed after init.
void test_init_zeroes_i_sp_dt_st(void)
{
    TEST_ASSERT_EQUAL_UINT16(0, regs.i);
    TEST_ASSERT_EQUAL_UINT8(0, regs.sp);
    TEST_ASSERT_EQUAL_UINT8(0, regs.dt);
    TEST_ASSERT_EQUAL_UINT8(0, regs.st);
}

// PC must be set to the load_addr argument, not zero.
void test_init_sets_pc_to_load_addr(void)
{
    TEST_ASSERT_EQUAL_UINT16(CHIP8_ROM_START, regs.pc);
}

// A non-default load_addr must also be honoured.
void test_init_sets_pc_to_custom_addr(void)
{
    chip8_registers r2;
    registers_init(&r2, 0x300);
    TEST_ASSERT_EQUAL_UINT16(0x300, r2.pc);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_zeroes_v_registers);
    RUN_TEST(test_init_zeroes_i_sp_dt_st);
    RUN_TEST(test_init_sets_pc_to_load_addr);
    RUN_TEST(test_init_sets_pc_to_custom_addr);
    return UNITY_END();
}
