#include "registers.h"

void registers_init(chip8_registers *regs, uint16_t load_addr)
{
    (void)regs;
    (void)load_addr;
    // TODO (§4, Appendix B): zero regs->v[0..15], regs->i, regs->dt,
    //   regs->st, regs->sp; set regs->pc = load_addr.
}
