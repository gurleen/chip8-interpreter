#ifndef CHIP8_CONFIG_H
#define CHIP8_CONFIG_H

#include <stdbool.h>

// §12 — Quirk flags and runtime configuration.
// Each field corresponds to one named flag in the §12 table; see §9 for
// the instruction each governs. Defaults are the normative COSMAC VIP
// values unless the table says otherwise.
typedef struct
{
    bool shift_uses_vy;             // §9.4  — SHR/SHL read VY (on = VIP default)
    bool logic_resets_vf;           // §9.5  — OR/AND/XOR reset VF (on = VIP)
    bool jump_uses_vx;              // §9.6  — BNNN as BXNN+VX (off = CHIP-8 default)
    bool index_increment_on_store;  // §9.2  — FX55/FX65 increment I (off = modern default)
    bool fx1e_sets_vf;              // §9.8  — FX1E sets VF on overflow (off = default)
    bool display_wait;              // §11.3 — limit DXYN to one draw/frame (off = default)
    bool clip_sprites;              // §7.2  — clip sprites at screen edges (on = default)
    bool wait_key_on_release;       // §9.3  — FX0A fires on release (on = VIP default)
    int  instructions_per_frame;    // §11.2 — CPU speed tuning (default 10)
} chip8_config;

// TODO (§12): fill *cfg with every normative default from the §12 table.
void config_default(chip8_config *cfg);

#endif /* CHIP8_CONFIG_H */
