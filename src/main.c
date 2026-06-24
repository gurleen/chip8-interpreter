#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "cpu.h"
#include "memory.h"

void fsize(FILE *file, int64_t *size)
{
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <rom-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *rom_path = argv[1];

    chip8_config cfg;
    config_default(&cfg);

    chip8_cpu cpu;
    cpu_init(&cpu, &cfg);

    FILE *f = fopen(rom_path, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "ERROR: could not open rom file\n");
        return EXIT_FAILURE;
    }

    int64_t rom_size;
    fsize(f, &rom_size);
    if (rom_size == -1)
    {
        fprintf(stderr, "ERROR: could not get size of rom file\n");
        fclose(f);
        return EXIT_FAILURE;
    }

    if (rom_size > CHIP8_MAX_ROM_SIZE)
    {
        fprintf(stderr, "ERROR: rom is too large to fit in memory\n");
        fclose(f);
        return EXIT_FAILURE;
    }

    uint8_t *rom_buffer;
    rom_buffer = (uint8_t *)calloc((size_t)rom_size, sizeof(uint8_t));

    if (rom_buffer == NULL)
    {
        fprintf(stderr, "ERROR: could not allocate memory for rom buffer\n");
        fclose(f);
        return EXIT_FAILURE;
    }

    size_t items_read = fread(rom_buffer, sizeof(uint8_t), (size_t)rom_size, f);
    fclose(f);

    if (items_read != (size_t)rom_size)
    {
        fprintf(stderr, "ERROR: expected %lld bytes but read %zu; did file change?\n",
                (long long)rom_size, items_read);
        free(rom_buffer);
        return EXIT_FAILURE;
    }

    int memory_load_status = memory_load_rom(&cpu.mem, rom_buffer, (size_t)rom_size);
    if (memory_load_status == -1)
    {
        fprintf(stderr, "ERROR: memory_load_rom returned -1; this should not happen\n");
        free(rom_buffer);
        return EXIT_FAILURE;
    }

    printf("Loaded rom: %s\n", rom_path);
    printf("Rom size: %lld bytes\n", rom_size);

    cpu_run(&cpu);

    free(rom_buffer);

    return EXIT_SUCCESS;
}
