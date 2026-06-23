#include "cpu.h"

void cpu_init(chip8_cpu *cpu, const chip8_config *cfg)
{
    (void)cpu;
    (void)cfg;
    // TODO (§4, Appendix B): call memory_init, registers_init(CHIP8_ROM_START),
    //   stack_init, display_init, input_init on the corresponding sub-fields;
    //   copy *cfg into cpu->config.
}

int cpu_step(chip8_cpu *cpu)
{
    (void)cpu;
    // TODO (§11.1): fetch-decode-execute one instruction.
    //
    // 1. FETCH (§3.4, §11.1):
    //      hi  = memory_read_byte(&cpu->mem, cpu->regs.pc)
    //      lo  = memory_read_byte(&cpu->mem, cpu->regs.pc + 1)
    //      op  = (hi << 8) | lo
    //
    // 2. ADVANCE (§11.1 step 2): cpu->regs.pc += 2
    //
    // 3. DECODE & 4. EXECUTE (§9) — dispatch on top nibble (op >> 12):
    //      0x0 → CLS (00E0), RET (00EE), or ignored SYS (0NNN)
    //      0x1 → JP NNN
    //      0x2 → CALL NNN
    //      0x3 → SE VX, NN
    //      0x4 → SNE VX, NN
    //      0x5 → SE VX, VY  (low nibble must be 0)
    //      0x6 → LD VX, NN
    //      0x7 → ADD VX, NN  (no VF change)
    //      0x8 → ALU: LD/OR/AND/XOR/ADD/SUB/SHR/SUBN/SHL  (§9, §9.4, §9.5, §9.7)
    //      0x9 → SNE VX, VY  (low nibble must be 0)
    //      0xA → LD I, NNN
    //      0xB → JP V0, NNN  (or BXNN+VX per §9.6 quirk)
    //      0xC → RND VX, NN
    //      0xD → DRW VX, VY, N  (§7.2)
    //      0xE → SKP (EX9E) / SKNP (EXA1)
    //      0xF → timer/input/memory group (FX07..FX65)
    //      unknown → fprintf(stderr, ...) and return -1  (§9.9)
    return 0;
}

int cpu_run(chip8_cpu *cpu)
{
    (void)cpu;
    // TODO (§11.2): main loop at 60 Hz cadence.
    //   Each frame:
    //     for (int i = 0; i < cpu->config.instructions_per_frame; i++)
    //         if (cpu_step(cpu) != 0) return -1;
    //     timers_tick(&cpu->regs.dt, &cpu->regs.st);          (§10.1)
    //     render display                                        (§7, platform)
    //     if (timers_sound_active(&cpu->regs.st)) emit tone;   (§10.3)
    //     poll host for input events → input_set_key(...)      (§8)
    //     sleep remainder of 1/60 s                            (§11.2)
    return 0;
}
