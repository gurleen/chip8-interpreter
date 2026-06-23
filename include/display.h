#ifndef CHIP8_DISPLAY_HDR
#define CHIP8_DISPLAY_HDR

#include <stdbool.h>
#include <stdint.h>

#define CHIP8_DISPLAY_W 64 // §7.1
#define CHIP8_DISPLAY_H 32 // §7.1

// Row-major: pixels[y][x], each element 0 (off) or 1 (on).
typedef struct
{
    uint8_t pixels[CHIP8_DISPLAY_H][CHIP8_DISPLAY_W];
} chip8_display;

// TODO (§7.1): set all pixels to 0.
void display_init(chip8_display *disp);

// TODO (§7.1, §9 CLS/00E0): set all pixels to 0.
void display_clear(chip8_display *disp);

// TODO (§7.2, §9 DRW/DXYN): XOR an n-row sprite from sprite_data at
//   (x mod 64, y mod 32); clip pixels past the right/bottom edge when
//   clip_sprites is true; return true on collision (any lit pixel turned off).
//   I is not modified here — the caller passes sprite_data = &ram[I].
bool display_draw_sprite(chip8_display *disp, uint8_t x, uint8_t y, const uint8_t *sprite_data,
                         uint8_t n, bool clip_sprites);

// TODO (§7.1): return the pixel state at (x, y); bounds-check or wrap.
uint8_t display_get_pixel(const chip8_display *disp, uint8_t x, uint8_t y);

#endif /* CHIP8_DISPLAY_HDR */
