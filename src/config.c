#include "config.h"

void config_default(chip8_config *cfg)
{
    (void)cfg;
    // TODO (§12): assign every field its normative default from the §12 table:
    //   cfg->shift_uses_vy            = true;
    //   cfg->logic_resets_vf          = true;
    //   cfg->jump_uses_vx             = false;
    //   cfg->index_increment_on_store = false;
    //   cfg->fx1e_sets_vf             = false;
    //   cfg->display_wait             = false;
    //   cfg->clip_sprites             = true;
    //   cfg->wait_key_on_release      = true;
    //   cfg->instructions_per_frame   = 10;
}
