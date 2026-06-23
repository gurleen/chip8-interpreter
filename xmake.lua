add_rules("mode.debug", "mode.release")

set_defaultmode("debug")
set_languages("c17")

if is_mode("debug") then
    add_cflags("-fsanitize=address,undefined", "-Wall", "-Wextra", "-Wpedantic")
    add_ldflags("-fsanitize=address,undefined")
elseif is_mode("release") then
    set_optimize("faster") -- -O2 (mode.release defaults to -O3 / fastest)
end

-- Static library of all interpreter modules; linked by the binary and
-- (eventually) by test targets that exercise interpreter code.
target("chip8_lib")
    set_kind("static")
    add_includedirs("include", {public = true})
    add_files(
        "src/config.c",
        "src/cpu.c",
        "src/display.c",
        "src/input.c",
        "src/memory.c",
        "src/registers.c",
        "src/stack.c",
        "src/timers.c"
    )

-- Main executable
target("chip8")
    set_kind("binary")
    add_deps("chip8_lib")
    add_files("src/main.c")

-- Placeholder test — verifies the Unity harness is wired up before any
-- interpreter code exists. Does not depend on chip8_lib.
-- Unity is vendored under vendor/unity/ (no package manager required).
target("test_placeholder")
    set_kind("binary")
    set_default(false)
    add_includedirs("vendor/unity")
    add_files("tests/test_placeholder.c", "vendor/unity/unity.c")
    add_tests("placeholder")
