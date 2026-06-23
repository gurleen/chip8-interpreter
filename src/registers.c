#include "registers.h"

#include <string.h>

void registers_init(chip8_registers *regs, uint16_t load_addr)
{
    memset(regs->v, 0, CHIP8_NUM_REGS);
    regs->i = 0;
    regs->sp = 0;
    regs->dt = 0;
    regs->st = 0;
    regs->pc = load_addr;
}
