#include <unity.h>

#include "stack.h"

static chip8_stack stk;

void setUp(void)
{
    stack_init(&stk);
}

void tearDown(void)
{
}

// All data slots must be zero after init.
void test_init_zeroes_data(void)
{
    for (int i = 0; i < CHIP8_STACK_DEPTH; i++)
    {
        TEST_ASSERT_EQUAL_UINT16(0, stk.data[i]);
    }
}

// top must be 0 (empty) after init.
void test_init_sets_top_to_zero(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, stk.top);
}

// A single push onto an empty stack must return true.
void test_push_returns_true(void)
{
    TEST_ASSERT_TRUE(stack_push(&stk, 0x200));
}

// After one push, top must advance to 1.
void test_push_increments_top(void)
{
    stack_push(&stk, 0x200);
    TEST_ASSERT_EQUAL_UINT8(1, stk.top);
}

// The pushed address must be retrievable via pop.
void test_pop_recovers_pushed_addr(void)
{
    uint16_t out = 0;
    stack_push(&stk, 0x3AC);
    bool ok = stack_pop(&stk, &out);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_UINT16(0x3AC, out);
}

// After a push/pop round-trip, top must return to 0.
void test_pop_decrements_top(void)
{
    uint16_t out;
    stack_push(&stk, 0x200);
    stack_pop(&stk, &out);
    TEST_ASSERT_EQUAL_UINT8(0, stk.top);
}

// Multiple pushes must be returned LIFO: last in, first out.
void test_lifo_order(void)
{
    uint16_t a;
    uint16_t b;
    uint16_t c;
    stack_push(&stk, 0x100);
    stack_push(&stk, 0x200);
    stack_push(&stk, 0x300);
    stack_pop(&stk, &c);
    stack_pop(&stk, &b);
    stack_pop(&stk, &a);
    TEST_ASSERT_EQUAL_UINT16(0x300, c);
    TEST_ASSERT_EQUAL_UINT16(0x200, b);
    TEST_ASSERT_EQUAL_UINT16(0x100, a);
}

// Filling the stack to exactly CHIP8_STACK_DEPTH entries must succeed.
void test_push_to_full_succeeds(void)
{
    for (int i = 0; i < CHIP8_STACK_DEPTH; i++)
    {
        TEST_ASSERT_TRUE(stack_push(&stk, (uint16_t)(0x200 + i)));
    }
    TEST_ASSERT_EQUAL_UINT8(CHIP8_STACK_DEPTH, stk.top);
}

// One push beyond CHIP8_STACK_DEPTH must return false (overflow).
void test_overflow_returns_false(void)
{
    for (int i = 0; i < CHIP8_STACK_DEPTH; i++)
    {
        stack_push(&stk, 0x200);
    }
    TEST_ASSERT_FALSE(stack_push(&stk, 0x300));
}

// top must not change when a push overflows.
void test_overflow_leaves_top_unchanged(void)
{
    for (int i = 0; i < CHIP8_STACK_DEPTH; i++)
    {
        stack_push(&stk, 0x200);
    }
    stack_push(&stk, 0x300); // overflow
    TEST_ASSERT_EQUAL_UINT8(CHIP8_STACK_DEPTH, stk.top);
}

// Popping from an empty stack must return false (underflow).
void test_underflow_returns_false(void)
{
    uint16_t out = 0;
    TEST_ASSERT_FALSE(stack_pop(&stk, &out));
}

// The out pointer must not be written on underflow.
void test_underflow_leaves_out_unchanged(void)
{
    uint16_t out = 0xBEEF;
    stack_pop(&stk, &out);
    TEST_ASSERT_EQUAL_UINT16(0xBEEF, out);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_zeroes_data);
    RUN_TEST(test_init_sets_top_to_zero);
    RUN_TEST(test_push_returns_true);
    RUN_TEST(test_push_increments_top);
    RUN_TEST(test_pop_recovers_pushed_addr);
    RUN_TEST(test_pop_decrements_top);
    RUN_TEST(test_lifo_order);
    RUN_TEST(test_push_to_full_succeeds);
    RUN_TEST(test_overflow_returns_false);
    RUN_TEST(test_overflow_leaves_top_unchanged);
    RUN_TEST(test_underflow_returns_false);
    RUN_TEST(test_underflow_leaves_out_unchanged);
    return UNITY_END();
}
