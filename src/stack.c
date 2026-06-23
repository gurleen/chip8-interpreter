#include "stack.h"

void stack_init(chip8_stack *stack)
{
    (void)stack;
    // TODO (§6): zero stack->data[], set stack->top = 0.
}

bool stack_push(chip8_stack *stack, uint16_t addr)
{
    (void)stack;
    (void)addr;
    // TODO (§6, §13): if stack->top == CHIP8_STACK_DEPTH return false (overflow);
    //   otherwise stack->data[stack->top++] = addr, return true.
    return false;
}

bool stack_pop(chip8_stack *stack, uint16_t *out)
{
    (void)stack;
    (void)out;
    // TODO (§6, §13): if stack->top == 0 return false (underflow);
    //   otherwise *out = stack->data[--stack->top], return true.
    return false;
}
