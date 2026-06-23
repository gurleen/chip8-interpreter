#ifndef CHIP8_STACK_H
#define CHIP8_STACK_H

#include <stdbool.h>
#include <stdint.h>

#define CHIP8_STACK_DEPTH 16 // §6 — 16 levels recommended

typedef struct
{
    uint16_t data[CHIP8_STACK_DEPTH];
    uint8_t top; // index of the next free slot; 0 = empty
} chip8_stack;

// TODO (§6): zero data[], set top = 0.
void stack_init(chip8_stack *stack);

// TODO (§6, §9 CALL/2NNN): push addr; return false on overflow (§13).
bool stack_push(chip8_stack *stack, uint16_t addr);

// TODO (§6, §9 RET/00EE): pop top into *out; return false on underflow (§13).
bool stack_pop(chip8_stack *stack, uint16_t *out);

#endif /* CHIP8_STACK_H */
