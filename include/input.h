#ifndef CHIP8_INPUT_H
#define CHIP8_INPUT_H

#include <stdbool.h>
#include <stdint.h>

#define CHIP8_NUM_KEYS 16 // §8.1 — keys 0x0..0xF

typedef struct
{
    bool pressed[CHIP8_NUM_KEYS]; // §8.2 — current key state (true = held)
} chip8_input;

// TODO (§8.2): set all keys to released (false).
void input_init(chip8_input *input);

// TODO (§8.2): set pressed[key_id] = down; key_id must be in 0..15.
void input_set_key(chip8_input *input, uint8_t key_id, bool down);

// TODO (§8.2, §9 EX9E/EXA1): return pressed[key_id]; bounds-check key_id.
bool input_is_pressed(const chip8_input *input, uint8_t key_id);

// TODO (§9.3, FX0A): block until a key event; store the key's value in *out.
//   Timers must continue to decrement during the wait (§9.3).
//   When on_release is true, wait for full press-then-release (§9.3 SHOULD).
void input_wait_for_key(chip8_input *input, uint8_t *out, bool on_release);

#endif /* CHIP8_INPUT_H */
