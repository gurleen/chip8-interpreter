#include "timers.h"

void timers_tick(uint8_t *dt, uint8_t *st)
{
    (void)dt;
    (void)st;
    // TODO (§10.1): if (*dt > 0) (*dt)--;
    //               if (*st > 0) (*st)--;
}

bool timers_sound_active(const uint8_t *st)
{
    (void)st;
    // TODO (§10.3): return *st > 0.
    return false;
}
