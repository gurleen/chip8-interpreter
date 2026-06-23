#ifndef CHIP8_CPU_H
#define CHIP8_CPU_H

#include "config.h"
#include "display.h"
#include "input.h"
#include "memory.h"
#include "registers.h"
#include "stack.h"

// Aggregates all VM state. The cpu module owns the fetch-decode-execute loop.
typedef struct
{
    chip8_memory mem;
    chip8_registers regs;
    chip8_stack stack;
    chip8_display display;
    chip8_input input;
    chip8_config config;
} chip8_cpu;

// TODO (§4, Appendix B): initialise every sub-component (memory_init,
//   registers_init(CHIP8_ROM_START), stack_init, display_init, input_init);
//   copy *cfg into cpu->config.
void cpu_init(chip8_cpu *cpu, const chip8_config *cfg);

// TODO (§11.1): execute one fetch-decode-execute cycle:
//   1. fetch big-endian 16-bit instruction from PC and PC+1  (§3.4, §11.1)
//   2. advance PC by 2                                        (§11.1 step 2)
//   3. decode by top nibble, dispatch to per-opcode handler   (§9)
//   4. on unknown opcode: print diagnostic (§9.9), return -1
int cpu_step(chip8_cpu *cpu);

// TODO (§11.2): run the main loop — execute config.instructions_per_frame
//   steps per 1/60 s frame, then call timers_tick, render, handle sound;
//   return when halted or an unrecoverable error occurs.
int cpu_run(chip8_cpu *cpu);

#endif /* CHIP8_CPU_H */
