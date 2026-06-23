add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

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

task("setup")
    set_category("plugin")
    on_run(function()
        os.execv("git", {"config", "core.hooksPath", ".githooks"})
        print("git hooks installed")
    end)
    set_menu({
        usage = "xmake setup",
        description = "Wire up .githooks (run once after cloning)",
    })

task("format")
    set_category("plugin")
    on_run(function()
        import("core.project.project")
        local files = os.files("src/*.c")
        table.join2(files, os.files("include/*.h"))
        os.execv("clang-format", table.join({"-i"}, files))
    end)
    set_menu({
        usage = "xmake format",
        description = "Format all source files with clang-format",
    })

-- Placeholder test — verifies the Unity harness is wired up before any
-- interpreter code exists. Does not depend on chip8_lib.
-- Unity is vendored under vendor/unity/ (no package manager required).
target("test_placeholder")
    set_kind("binary")
    set_default(false)
    add_includedirs("vendor/unity")
    add_files("tests/test_placeholder.c", "vendor/unity/unity.c")
    add_tests("placeholder")

target("test_memory")
    set_kind("binary")
    set_default(false)
    add_deps("chip8_lib")
    add_includedirs("vendor/unity")
    add_files("tests/test_memory.c", "vendor/unity/unity.c")
    add_tests("memory")
