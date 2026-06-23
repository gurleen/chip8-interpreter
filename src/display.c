#include "display.h"

void display_init(chip8_display *disp)
{
    (void)disp;
    // TODO (§7.1): zero all of disp->pixels[][].
}

void display_clear(chip8_display *disp)
{
    (void)disp;
    // TODO (§7.1, §9 CLS/00E0): zero all of disp->pixels[][].
}

bool display_draw_sprite(chip8_display *disp, uint8_t x, uint8_t y, const uint8_t *sprite_data,
                         uint8_t n, bool clip_sprites)
{
    (void)disp;
    (void)x;
    (void)y;
    (void)sprite_data;
    (void)n;
    (void)clip_sprites;
    // TODO (§7.2, §9 DRW/DXYN):
    //   sx = x % CHIP8_DISPLAY_W, sy = y % CHIP8_DISPLAY_H  (starting coords)
    //   for row in 0..n-1:
    //     byte = sprite_data[row]
    //     for bit in 0..7:
    //       px = sx + bit, py = sy + row
    //       if clip_sprites: skip if px >= W or py >= H
    //       else: wrap px and py
    //       if sprite bit is set: XOR pixel, track collision if pixel was on
    //   return collision flag (VF = 1 if any on pixel turned off, else 0)
    return false;
}

uint8_t display_get_pixel(const chip8_display *disp, uint8_t x, uint8_t y)
{
    (void)disp;
    (void)x;
    (void)y;
    // TODO (§7.1): return disp->pixels[y % CHIP8_DISPLAY_H][x % CHIP8_DISPLAY_W].
    return 0;
}
