#include "stack.h"

#include <string.h>

void stack_init(chip8_stack *stack)
{
    memset(stack->data, 0, CHIP8_STACK_DEPTH);
    stack->top = 0;
}

bool stack_push(chip8_stack *stack, uint16_t addr)
{
    if (stack->top == CHIP8_STACK_DEPTH)
    {
        return false;
    }

    stack->data[stack->top++] = addr;
    return true;
}

bool stack_pop(chip8_stack *stack, uint16_t *out)
{
    if (stack->top == 0)
    {
        return false;
    }

    *out = stack->data[--stack->top];
    return true;
}
