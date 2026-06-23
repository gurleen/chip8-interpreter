#ifndef CHIP8_TIMERS_H
#define CHIP8_TIMERS_H

#include <stdbool.h>
#include <stdint.h>

// §10 — DT and ST live in chip8_registers; this module provides the 60 Hz
// decrement logic and the sound-active predicate used by the main loop.

// TODO (§10.1): decrement *dt and *st each by 1 if non-zero.
//   Called exactly once per 60 Hz frame by the run loop (§11.2).
void timers_tick(uint8_t *dt, uint8_t *st);

// TODO (§10.3): return true while *st > 0 (tone should be playing).
bool timers_sound_active(const uint8_t *st);

#endif /* CHIP8_TIMERS_H */
