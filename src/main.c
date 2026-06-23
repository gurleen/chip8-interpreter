#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "cpu.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <rom-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *rom_path = argv[1];
    (void)rom_path;

    chip8_config cfg;
    (void)cfg;
    // TODO (§12): config_default(&cfg);

    chip8_cpu cpu;
    (void)cpu;
    // TODO (§4): cpu_init(&cpu, &cfg);

    // TODO (§4): open rom_path, read its bytes into a buffer, call
    //   memory_load_rom(&cpu.mem, buf, len); handle errors (§13).

    // TODO (§11): cpu_run(&cpu);

    return EXIT_SUCCESS;
}
