#ifndef CHIP8_REGISTERS_H
#define CHIP8_REGISTERS_H

#include <stdint.h>

#define CHIP8_NUM_REGS 16 // §5.1 — V0..VF

typedef struct
{
    uint8_t v[CHIP8_NUM_REGS]; // §5.1 — V0..VF; VF is the flag register
    uint16_t i; // §5.2 — address register (only low 12 bits meaningful)
    uint16_t pc; // §5.4 — program counter
    uint8_t sp; // §5.4 — stack pointer (index into chip8_stack)
    uint8_t dt; // §5.3 — delay timer (60 Hz countdown)
    uint8_t st; // §5.3 — sound timer (60 Hz countdown; tone while > 0)
} chip8_registers;

void registers_init(chip8_registers *regs, uint16_t load_addr);

#endif /* CHIP8_REGISTERS_H */
