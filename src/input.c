#include "input.h"

void input_init(chip8_input *input)
{
    (void)input;
    // TODO (§8.2): zero input->pressed[] — all keys start released.
}

void input_set_key(chip8_input *input, uint8_t key_id, bool down)
{
    (void)input;
    (void)key_id;
    (void)down;
    // TODO (§8.2): bounds-check key_id (must be 0..15);
    //   input->pressed[key_id] = down.
}

bool input_is_pressed(const chip8_input *input, uint8_t key_id)
{
    (void)input;
    (void)key_id;
    // TODO (§8.2, §9 EX9E/EXA1): bounds-check key_id;
    //   return input->pressed[key_id].
    return false;
}

void input_wait_for_key(chip8_input *input, uint8_t *out, bool on_release)
{
    (void)input;
    (void)out;
    (void)on_release;
    // TODO (§9.3, FX0A): halt instruction execution (do not advance PC) until
    //   a key event is detected; timers must keep ticking (§9.3);
    //   store the key value in *out;
    //   if on_release: wait for the full press-then-release cycle.
}
