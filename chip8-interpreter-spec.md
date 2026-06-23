# RFC: CHIP-8 Interpreter Specification
 
| Field | Value |
| --- | --- |
| **Status** | Draft |
| **Version** | 1.0 |
| **Date** | 2026-06-23 |
| **Scope** | Implementation of a CHIP-8 interpreter ("the interpreter") |
| **Audience** | Engineers implementing the interpreter; reviewers decomposing this document into tasks |
 
---
 
## 1. Introduction
 
### 1.1 Purpose
 
This document specifies the behavior required of a CHIP-8 interpreter. CHIP-8 is an interpreted programming language originally developed by Joseph Weisbecker around 1977 for the COSMAC VIP and similar 8-bit microcomputers. A CHIP-8 program ("ROM") is a stream of 16-bit instructions that the interpreter fetches, decodes, and executes against a small virtual machine consisting of memory, registers, a stack, two timers, a monochrome display, and a 16-key keypad.
 
The goal of this specification is to define a CHIP-8 interpreter precisely enough that two independent implementations built from it would run the same library of ROMs identically. It is intended to be decomposed into discrete engineering tasks.
 
### 1.2 Goals and Non-Goals
 
**Goals**
 
- Define the complete virtual machine: memory layout, registers, stack, timers, display, and input.
- Define all 35 instructions of the standard CHIP-8 instruction set, including the four historically undocumented `8XYN` instructions.
- Specify a single, unambiguous behavior for each instruction, including the cases where historical implementations disagree.
- Define the fetch–decode–execute cycle and the timing model.
**Non-Goals**
 
- This document does **not** specify SUPER-CHIP (SCHIP), CHIP-48, XO-CHIP, or any other extension instructions. These are explicitly out of scope for v1.0 and may be addressed in a future revision.
- This document does not mandate a particular host language, rendering backend, audio backend, or UI.
- This document does not specify a ROM file format beyond "a flat sequence of bytes loaded into memory" (see §4).
### 1.3 Terminology
 
The key words **MUST**, **MUST NOT**, **REQUIRED**, **SHALL**, **SHOULD**, **SHOULD NOT**, **MAY**, and **OPTIONAL** are to be interpreted as described in RFC 2119.
 
| Term | Meaning |
| --- | --- |
| Nibble | A 4-bit value. |
| Byte | An 8-bit value. |
| Word | A 16-bit value. |
| Sprite | A run of 1–15 bytes interpreted as a bitmap, 8 pixels wide. |
| ROM / program | The byte sequence loaded into memory and executed. |
| Quirk | A behavior on which historical CHIP-8 implementations disagree; this document resolves each one explicitly (see §9). |
 
### 1.4 Notation
 
- Instructions are written as four hexadecimal nibbles, e.g. `DXYN`.
- Within an instruction pattern, the following placeholders are used:
  - `NNN` — a 12-bit address (lowest 12 bits of the instruction).
  - `NN` (also written `KK`) — an 8-bit constant (lowest 8 bits).
  - `N` — a 4-bit constant (lowest 4 bits).
  - `X` — a 4-bit register index from the lower nibble of the high byte.
  - `Y` — a 4-bit register index from the upper nibble of the low byte.
- Hexadecimal constants carry a `0x` prefix. Decimal constants are bare.
- `VX` denotes the general-purpose register selected by nibble `X`; likewise `VY`.
---
 
## 2. System Architecture Overview
 
The interpreter emulates a virtual machine with the following components:
 
- **Memory:** 4096 bytes (4 KB) of addressable RAM.
- **Registers:** sixteen 8-bit general-purpose registers (`V0`–`VF`), one 16-bit address register (`I`), and two 8-bit timer registers (delay and sound).
- **Pseudo-registers:** a program counter (`PC`) and a stack pointer (`SP`), not directly accessible to programs.
- **Stack:** an array of 16-bit return addresses supporting nested subroutine calls.
- **Display:** a 64 × 32 monochrome framebuffer.
- **Input:** a 16-key hexadecimal keypad.
- **Timers:** a delay timer and a sound timer, each decrementing at 60 Hz.
Each of these is specified in detail below.
 
---
 
## 3. Memory
 
### 3.1 Size and Addressing
 
The interpreter **MUST** provide 4096 bytes of RAM, addressable from `0x000` to `0xFFF` inclusive. Each address holds one 8-bit byte.
 
### 3.2 Memory Map
 
| Range | Size | Purpose |
| --- | --- | --- |
| `0x000`–`0x1FF` | 512 bytes | Reserved for the interpreter. On original hardware the interpreter itself lived here. Programs **MUST NOT** be loaded here. The font data (§7.3) is stored somewhere in this region. |
| `0x200`–`0xFFF` | 3584 bytes | Program and data space. ROMs are loaded starting at `0x200`. |
 
The interpreter **MUST** load ROMs at `0x200` by default. The interpreter **MAY** support an alternative load address (some historical programs target `0x600` for the ETI-660), but this is **OPTIONAL** and `0x200` is the norm.
 
### 3.3 Font Storage
 
The interpreter **MUST** preload the built-in hexadecimal font (§7.3) into the reserved region (`0x000`–`0x1FF`) before execution begins. A conventional and **RECOMMENDED** location is `0x050`–`0x09F`. The exact address is an implementation detail, but the `FX29` instruction (§8) **MUST** return addresses consistent with wherever the font was stored.
 
### 3.4 Endianness
 
Instructions are 16 bits and stored **big-endian**: the most significant byte occupies the lower address. The interpreter **MUST** read the byte at `PC` as the high byte and the byte at `PC + 1` as the low byte.
 
### 3.5 Instruction Alignment
 
By convention the first byte of each instruction resides at an even address. The interpreter **MUST NOT** assume alignment, since `PC` can be set to any address by jump and call instructions; it **MUST** simply fetch two bytes from `PC` and `PC + 1` regardless of parity.
 
---
 
## 4. Program Loading
 
On reset, the interpreter **MUST**:
 
1. Zero all of RAM (or otherwise place RAM in a defined initial state), then load the font into the reserved region.
2. Copy the ROM byte-for-byte into RAM beginning at the load address (`0x200` by default).
3. Initialize `PC` to the load address.
4. Initialize `SP` to point at an empty stack, set `I` to `0`, set all `V0`–`VF` to `0`, set both timers to `0`, and clear the display.
The interpreter **SHOULD** reject ROMs that do not fit in the available program space (a ROM larger than 3584 bytes when loaded at `0x200`).
 
---
 
## 5. Registers
 
### 5.1 General-Purpose Registers
 
There **MUST** be sixteen 8-bit registers, `V0` through `VF`, each holding an unsigned value from `0x00` to `0xFF`.
 
`VF` is special: it is used as a flag (carry, borrow, collision, shifted-out bit) by several instructions. Programs are advised not to use `VF` for general storage. The interpreter **MUST** write the flag results into `VF` as specified per instruction (§8), even when `VF` is also the destination register `X` — see §9.7.
 
### 5.2 Address Register `I`
 
There **MUST** be one 16-bit address register, `I`, used to hold memory addresses for read/write and sprite operations. Although 16 bits wide, only the low 12 bits are meaningful for addressing the 4 KB memory.
 
### 5.3 Timer Registers
 
There **MUST** be two 8-bit timer registers: the delay timer (`DT`) and the sound timer (`ST`). Their behavior is specified in §10.
 
### 5.4 Pseudo-Registers
 
- **Program Counter (`PC`):** holds the address of the currently executing instruction. **MUST** be wide enough to address all of memory (≥ 12 bits; 16 bits **RECOMMENDED**).
- **Stack Pointer (`SP`):** identifies the top of the stack.
Neither pseudo-register is directly readable or writable by a CHIP-8 program.
 
---
 
## 6. Stack
 
The stack stores return addresses for subroutine calls. The interpreter **MUST** support at least 12 levels of nested subroutine calls; 16 levels is **RECOMMENDED** and is the conventional size.
 
- `CALL` (`2NNN`) pushes the current `PC` onto the stack and jumps.
- `RET` (`00EE`) pops the top of the stack into `PC`.
The interpreter's behavior on stack overflow (more than the supported nesting depth) and stack underflow (`RET` with an empty stack) is **undefined** by CHIP-8; the interpreter **SHOULD** detect these conditions and fail safely (e.g. halt with a diagnostic) rather than corrupt host memory. See §11.
 
---
 
## 7. Display
 
### 7.1 Geometry
 
The display **MUST** be a monochrome framebuffer of 64 pixels wide by 32 pixels tall. The top-left pixel is `(0, 0)` and the bottom-right is `(63, 31)`. Each pixel is either on (`1`) or off (`0`). All pixels are off on reset and after `CLS` (`00E0`).
 
How on/off pixels are rendered (colors, scaling) is implementation-dependent. A common choice is white-on-black.
 
### 7.2 Sprites and the Draw Operation
 
All drawing is performed by the `DXYN` instruction (§8), which draws a sprite by XOR.
 
- A sprite is 8 pixels wide and `N` pixels tall, where `1 ≤ N ≤ 15`. It is read as `N` consecutive bytes from memory starting at the address in `I`; each byte is one row, most-significant bit on the left.
- Drawing XORs the sprite onto the framebuffer: each sprite bit toggles the corresponding framebuffer pixel.
- **Collision:** if any framebuffer pixel that was on is turned off by the draw, the interpreter **MUST** set `VF` to `1`; otherwise `VF` **MUST** be set to `0`. `VF` is always written (set to `0` when no collision occurs).
- **Coordinate wrapping of the starting position:** the starting coordinates **MUST** be taken modulo the display dimensions. The starting x is `VX mod 64` and the starting y is `VY mod 32`.
- **Clipping of sprite body:** once the starting position is established, pixels of the sprite that extend past the right or bottom edge of the display **MUST** be clipped (not wrapped). See §9.1 for the rationale and the alternative behavior that is explicitly rejected.
### 7.3 Built-in Font
 
The interpreter **MUST** provide sprite data for the sixteen hexadecimal digits `0`–`F`. Each font sprite is 8 pixels wide (only the high 4 bits per row are used, so glyphs are 4 pixels wide visually) and 5 pixels tall (5 bytes). The `FX29` instruction returns the address of the sprite for a given digit.
 
The font data **MUST** be byte-for-byte as follows:
 
| Digit | Bytes |
| --- | --- |
| 0 | `F0 90 90 90 F0` |
| 1 | `20 60 20 20 70` |
| 2 | `F0 10 F0 80 F0` |
| 3 | `F0 10 F0 10 F0` |
| 4 | `90 90 F0 10 10` |
| 5 | `F0 80 F0 10 F0` |
| 6 | `F0 80 F0 90 F0` |
| 7 | `F0 10 20 40 40` |
| 8 | `F0 90 F0 90 F0` |
| 9 | `F0 90 F0 10 F0` |
| A | `F0 90 F0 90 90` |
| B | `E0 90 E0 90 E0` |
| C | `F0 80 80 80 F0` |
| D | `E0 90 90 90 E0` |
| E | `F0 80 F0 80 F0` |
| F | `F0 80 F0 80 80` |
 
---
 
## 8. Input
 
### 8.1 Keypad Layout
 
The interpreter **MUST** accept input from a 16-key hexadecimal keypad. Each key maps to one hex digit `0`–`F`. The canonical physical layout is:
 
```
1 2 3 C
4 5 6 D
7 8 9 E
A 0 B F
```
 
The interpreter **MUST** map host keyboard keys to these 16 values. The mapping is implementation-defined; a widely used convention maps the left 4×4 block of a QWERTY keyboard (`1 2 3 4` / `Q W E R` / `A S D F` / `Z X C V`) onto the keypad, but this is **OPTIONAL**.
 
### 8.2 Key State Model
 
The interpreter **MUST** track, for each of the 16 keys, whether it is currently pressed or released. This state is queried by `EX9E`, `EXA1`, and `FX0A` (§9).
 
---
 
## 9. Instruction Set
 
### 9.1 Encoding and Operands
 
Every instruction is exactly 16 bits. The placeholders `NNN`, `NN`/`KK`, `N`, `X`, and `Y` are defined in §1.4.
 
### 9.2 Instruction Reference
 
For each instruction below: the pattern, a mnemonic, and the required behavior. Unless stated otherwise, after executing an instruction the interpreter advances `PC` by 2 (see §10.2 for the fetch/advance ordering).
 
#### Flow control and system
 
| Opcode | Mnemonic | Behavior |
| --- | --- | --- |
| `0NNN` | `SYS NNN` | Call a machine-code routine at `NNN`. The interpreter **MUST** treat this as a no-op (ignored) except for the two specific patterns below. It is present only for historical hardware. |
| `00E0` | `CLS` | Clear the display (all pixels to `0`). |
| `00EE` | `RET` | Return from subroutine: set `PC` to the address popped from the top of the stack and decrement `SP`. |
| `1NNN` | `JP NNN` | Jump: set `PC` to `NNN`. Does not touch the stack. |
| `2NNN` | `CALL NNN` | Call subroutine: push the current `PC` onto the stack, increment `SP`, then set `PC` to `NNN`. |
| `BNNN` | `JP V0, NNN` | Jump to `NNN + V0`. See §9.6 (`BNNN` quirk). |
 
> **Note on `0NNN` vs `00E0`/`00EE`:** `00E0` and `00EE` share the `0` group with `SYS`. The interpreter **MUST** match `00E0` and `00EE` exactly and treat all other `0NNN` patterns as an ignored `SYS`.
 
#### Conditional skips
 
| Opcode | Mnemonic | Behavior |
| --- | --- | --- |
| `3XNN` | `SE VX, NN` | Skip the next instruction if `VX == NN`. |
| `4XNN` | `SNE VX, NN` | Skip the next instruction if `VX != NN`. |
| `5XY0` | `SE VX, VY` | Skip the next instruction if `VX == VY`. |
| `9XY0` | `SNE VX, VY` | Skip the next instruction if `VX != VY`. |
| `EX9E` | `SKP VX` | Skip the next instruction if the key whose value is in `VX` is currently pressed. |
| `EXA1` | `SKNP VX` | Skip the next instruction if the key whose value is in `VX` is currently **not** pressed. |
 
> "Skip the next instruction" means advance `PC` by an additional 2 (4 total for this step), so the following 2-byte instruction is not executed.
 
#### Loads and arithmetic with constants
 
| Opcode | Mnemonic | Behavior |
| --- | --- | --- |
| `6XNN` | `LD VX, NN` | Set `VX = NN`. |
| `7XNN` | `ADD VX, NN` | Set `VX = VX + NN` (mod 256). **Does not** affect `VF`. |
 
#### Register-to-register ALU (`8XYN`)
 
| Opcode | Mnemonic | Behavior |
| --- | --- | --- |
| `8XY0` | `LD VX, VY` | Set `VX = VY`. |
| `8XY1` | `OR VX, VY` | Set `VX = VX OR VY`. See §9.5 (`VF` reset quirk). |
| `8XY2` | `AND VX, VY` | Set `VX = VX AND VY`. See §9.5. |
| `8XY3` | `XOR VX, VY` | Set `VX = VX XOR VY`. See §9.5. |
| `8XY4` | `ADD VX, VY` | Set `VX = VX + VY`. Set `VF = 1` if the sum exceeds 255 (carry), else `VF = 0`. Store only the low 8 bits in `VX`. |
| `8XY5` | `SUB VX, VY` | Set `VX = VX - VY`. Set `VF = 1` if `VX >= VY` (i.e. NOT borrow), else `VF = 0`. Store the low 8 bits in `VX`. |
| `8XY6` | `SHR VX {, VY}` | Shift right by 1. See §9.4 — this instruction has a quirk and is fully specified there. |
| `8XY7` | `SUBN VX, VY` | Set `VX = VY - VX`. Set `VF = 1` if `VY >= VX` (NOT borrow), else `VF = 0`. Store the low 8 bits in `VX`. |
| `8XYE` | `SHL VX {, VY}` | Shift left by 1. See §9.4 — quirk, fully specified there. |
 
> **Flag-write ordering (`8XY4`–`8XYE`):** The arithmetic result is written to `VX`, and the flag is written to `VF`. When `X == F`, the flag write wins — see §9.7.
 
#### Address register and random
 
| Opcode | Mnemonic | Behavior |
| --- | --- | --- |
| `ANNN` | `LD I, NNN` | Set `I = NNN`. |
| `CXNN` | `RND VX, NN` | Set `VX = (random byte 0–255) AND NN`. |
 
#### Display
 
| Opcode | Mnemonic | Behavior |
| --- | --- | --- |
| `DXYN` | `DRW VX, VY, N` | Draw an `N`-byte sprite read from memory at `I` at screen position `(VX mod 64, VY mod 32)`, XOR mode, with collision flag and clipping as specified in §7.2. `I` is **not** modified. |
 
#### Timers and input (`FXNN`)
 
| Opcode | Mnemonic | Behavior |
| --- | --- | --- |
| `FX07` | `LD VX, DT` | Set `VX = DT` (current delay timer value). |
| `FX0A` | `LD VX, K` | Block until a key is pressed, then store that key's value in `VX`. See §9.3 for the precise blocking semantics. |
| `FX15` | `LD DT, VX` | Set `DT = VX`. |
| `FX18` | `LD ST, VX` | Set `ST = VX`. |
| `FX1E` | `ADD I, VX` | Set `I = I + VX`. See §9.8 for `VF`/overflow behavior. |
| `FX29` | `LD F, VX` | Set `I` to the address of the font sprite for the hex digit in the low nibble of `VX`. |
| `FX33` | `LD B, VX` | Store the BCD of `VX`: hundreds digit at `I`, tens at `I+1`, ones at `I+2`. |
| `FX55` | `LD [I], VX` | Store `V0`..`VX` into memory starting at `I`. See §9.2 (`I` increment quirk). |
| `FX65` | `LD VX, [I]` | Load `V0`..`VX` from memory starting at `I`. See §9.2 (`I` increment quirk). |
 
### 9.2.1 `FX55` / `FX65` memory range
 
For `FX55` and `FX65`, the registers `V0` through `VX` **inclusive** are written/read — that is `X + 1` registers. When `X == 0`, exactly one register (`V0`) is transferred. The increment of `I` is governed by the quirk in §9.2 below.
 
> **`I` increment quirk (`FX55` / `FX65`):** On the original COSMAC VIP, these instructions incremented `I` by `X + 1` as a side effect (leaving `I = I + X + 1` afterward). CHIP-48/SCHIP changed this so `I` is left unchanged. **This interpreter MUST leave `I` unchanged** after `FX55`/`FX65`. This is configurable in §11 for compatibility but unchanged is the default.
 
### 9.3 `FX0A` Blocking Semantics
 
`FX0A` **MUST** halt instruction execution — `PC` does not advance past this instruction — until a key press is registered. Timers (§10) **MUST** continue to decrement while blocked. Once a key is pressed, its hex value is stored in `VX` and execution resumes at the following instruction.
 
The interpreter **SHOULD** register the key on **release** rather than on initial press (i.e. wait for a full press-and-release), matching original hardware; registering on press is an acceptable simplification but **SHOULD** be noted as a configuration option (§11).
 
### 9.4 Shift Quirk (`8XY6` / `8XYE`)
 
Historical implementations disagree on the source operand:
 
- **Original COSMAC VIP / CHIP-8:** the value in `VY` is shifted and the result stored in `VX`.
- **CHIP-48 / SCHIP:** `VX` is shifted in place and `VY` is ignored.
**This interpreter MUST implement the original (VIP) behavior by default:**
 
- `8XY6` (`SHR`): set `VF` to the least-significant bit of `VY`, then set `VX = VY >> 1`.
- `8XYE` (`SHL`): set `VF` to the most-significant bit of `VY`, then set `VX = VY << 1` (low 8 bits).
The interpreter **MUST** expose a configuration flag (§11) to select the CHIP-48/SCHIP behavior (shift `VX` in place, ignore `VY`), because a large fraction of modern ROMs assume it. The flag's default is the VIP behavior; consumers decomposing this spec should treat the configuration toggle as a required feature.
 
### 9.5 Logical-Op `VF` Reset Quirk (`8XY1` / `8XY2` / `8XY3`)
 
On the original COSMAC VIP, the `OR`, `AND`, and `XOR` instructions reset `VF` to `0` as a side effect. CHIP-48/SCHIP do not. **This interpreter MUST reset `VF` to `0`** after `8XY1`, `8XY2`, and `8XY3` by default, and **MUST** provide a configuration flag (§11) to disable this side effect.
 
### 9.6 `BNNN` Jump Quirk
 
- **Original CHIP-8:** `BNNN` jumps to `NNN + V0`.
- **SCHIP:** the instruction was reinterpreted as `BXNN`, jumping to `XNN + VX`.
**This interpreter MUST implement the original `NNN + V0` behavior** and **MAY** provide a configuration flag for the SCHIP interpretation.
 
### 9.7 `VF` as Destination Register
 
When an instruction both produces a numeric result in `VX` and a flag in `VF`, and `X == F`, the **flag value is the final value of `VF`**. The interpreter **MUST** write the numeric result first and the flag second, so that the flag is what remains. This applies to `8XY4`, `8XY5`, `8XY6`, `8XY7`, and `8XYE`.
 
### 9.8 `FX1E` Overflow
 
`FX1E` sets `I = I + VX`. The interpreter **MUST** keep `I` within its 16-bit range. On the original CHIP-8, `VF` is **not** affected by this instruction, and **this interpreter MUST NOT modify `VF`** in `FX1E` by default. (Some Amiga-era interpreters set `VF` on overflow past `0x0FFF`; this is **OPTIONAL** and off by default.)
 
### 9.9 Undocumented and Unknown Opcodes
 
- The instructions `8XY3`, `8XY6`, `8XY7`, and `8XYE` were undocumented historically but are part of the required set here.
- Any 16-bit value that does not match a defined pattern is an **unknown opcode**. The interpreter **SHOULD** halt with a diagnostic identifying the opcode and the `PC`, rather than silently ignoring it. It **MUST NOT** crash the host process or corrupt host memory.
---
 
## 10. Timers and Sound
 
### 10.1 Decrement Rate
 
Both `DT` and `ST` **MUST** decrement at 60 Hz while non-zero, stopping at `0`. This is independent of the instruction execution rate (§10.3).
 
### 10.2 Delay Timer
 
`DT` is a general-purpose countdown used by programs to time events. It is set by `FX15` and read by `FX07`. It has no other observable effect.
 
### 10.3 Sound Timer
 
While `ST` is non-zero, the interpreter **MUST** emit a tone; when `ST` reaches `0`, the tone stops. `ST` is set by `FX18`; it cannot be read. The tone's frequency and waveform are unspecified (implementation-defined). Per original hardware, a value of `0x01` produces no audible tone (the minimum effective value is `0x02`); replicating this edge case is **OPTIONAL**.
 
---
 
## 11. Execution Model and Timing
 
### 11.1 The Cycle
 
The interpreter executes a fetch–decode–execute loop:
 
1. **Fetch** the two bytes at `PC` and `PC + 1`, forming a big-endian 16-bit instruction.
2. **Advance** `PC` by 2 *before* executing, so that instructions which modify `PC` (jumps, calls, skips) operate relative to the already-advanced value. (Equivalently, an implementation may advance after execution as long as jumps/skips are made consistent; the pre-increment model is **RECOMMENDED** for clarity.)
3. **Decode** the instruction by its pattern (§9).
4. **Execute** the corresponding behavior.
### 11.2 Clock Speeds
 
There are two independent rates:
 
- **Timer rate:** exactly 60 Hz (§10.1). **REQUIRED**.
- **Instruction rate (CPU speed):** the number of instructions executed per second is not defined by CHIP-8. A common, **RECOMMENDED** default is roughly 500–700 instructions per second (often expressed as a number of instructions per 60 Hz frame, e.g. ~8–11 instructions per frame). The interpreter **MUST** make this rate configurable.
The interpreter **MUST** keep the 60 Hz timer cadence correct regardless of the chosen instruction rate. A typical structure runs a fixed number of instructions per 1/60 s frame, then decrements both timers once and renders/produces sound for that frame.
 
### 11.3 Display Refresh and the `DXYN` Wait Quirk
 
On original hardware, `DXYN` waited for the vertical blanking interval, effectively limiting draws to 60 Hz. Some programs rely on this for timing. The interpreter **MAY** implement a "display wait" (at most one `DXYN` per frame) as a configuration option (§12); it is **OPTIONAL** and off by default.
 
---
 
## 12. Configuration (Quirk Flags)
 
Because CHIP-8 was never formally standardized, ROMs were written against different interpreter behaviors. The interpreter **MUST** expose the following configuration flags so that both the original-hardware behavior and the common modern behavior can be selected. Defaults are as stated; defaults reflect original COSMAC VIP behavior except where noted.
 
| Flag | Default | Effect when enabled |
| --- | --- | --- |
| `shift_uses_vy` | **on** | `8XY6`/`8XYE` read `VY` (VIP). When off, shift `VX` in place (CHIP-48/SCHIP). (§9.4) |
| `logic_resets_vf` | **on** | `8XY1`/`8XY2`/`8XY3` reset `VF` to `0` (VIP). (§9.5) |
| `jump_uses_vx` | **off** | `BNNN` behaves as SCHIP `BXNN`. When off, uses `NNN + V0` (CHIP-8). (§9.6) |
| `index_increment_on_store` | **off** | `FX55`/`FX65` leave `I = I + X + 1` (VIP). When off, `I` is unchanged. (§9.2) Note: default off means modern (unchanged) behavior. |
| `fx1e_sets_vf` | **off** | `FX1E` sets `VF` on overflow past `0x0FFF`. (§9.8) |
| `display_wait` | **off** | Limit `DXYN` to one draw per 60 Hz frame. (§11.3) |
| `clip_sprites` | **on** | Clip sprites at screen edges (do not wrap the body). When off, wrap. (§7.2, §9.1) |
| `wait_key_on_release` | **on** | `FX0A` registers a key on release. When off, on press. (§9.3) |
| `instructions_per_frame` | 10 | Number of instructions executed per 60 Hz frame (tunes CPU speed). (§11.2) |
 
> Reviewers decomposing this spec: each flag is an independent, testable unit of behavior and is a natural task boundary. The default column is normative.
 
---
 
## 13. Error Handling
 
The interpreter **MUST** behave safely on malformed input and illegal states. Specifically it:
 
- **MUST NOT** read or write outside the 4 KB memory; memory accesses **SHOULD** be masked to 12 bits or bounds-checked.
- **MUST NOT** crash the host process on an unknown opcode (§9.9); it **SHOULD** halt with a diagnostic.
- **SHOULD** detect stack overflow and underflow (§6) and halt with a diagnostic.
- **SHOULD** detect a ROM too large to fit in program space at load time (§4).
- **SHOULD** treat any digit `0x0`–`0xF` as valid for `FX29`; behavior for `VX > 0x0F` (i.e. when the upper nibble is non-zero) is **undefined** but **SHOULD** be handled defensively (mask to the low nibble).
---
 
## 14. Conformance
 
An implementation conforms to this specification if and only if:
 
1. It implements the full virtual machine of §§3–8 with the required sizes and ranges.
2. It implements every instruction in §9 with the specified default behavior.
3. It implements the timer and sound semantics of §10 at 60 Hz.
4. It implements the execution model of §11, keeping timers at 60 Hz independent of instruction rate.
5. It exposes the configuration flags of §12 with the stated defaults.
6. It satisfies the error-handling requirements of §13.
Conformance **SHOULD** be demonstrated against a recognized CHIP-8 test-ROM suite covering opcode behavior, flag results, quirks, and timing.
 
---
 
## 15. Open Questions / Future Work
 
- **SUPER-CHIP / XO-CHIP support.** This v1.0 deliberately excludes extension instructions (high-res 128×64 mode, scrolling, `EXIT`, extended sprites, `RPL` flag storage, etc.). A future revision should specify them, including how the `DXY0` 16×16 sprite and the half-pixel SCHIP scroll quirks interact with the flags in §12.
- **Authoritative default for `instructions_per_frame`.** The 10/frame default is a pragmatic middle value; it should be validated against a representative ROM library and revised if a better default emerges.
- **Audio specification.** The tone is currently fully implementation-defined. A future revision may recommend a frequency/waveform for consistency.
---
 
## Appendix A: Complete Opcode Table
 
| Opcode | Mnemonic | Summary |
| --- | --- | --- |
| `0NNN` | `SYS NNN` | Ignored (legacy machine-code call). |
| `00E0` | `CLS` | Clear display. |
| `00EE` | `RET` | Return from subroutine. |
| `1NNN` | `JP NNN` | Jump to `NNN`. |
| `2NNN` | `CALL NNN` | Call subroutine at `NNN`. |
| `3XNN` | `SE VX, NN` | Skip if `VX == NN`. |
| `4XNN` | `SNE VX, NN` | Skip if `VX != NN`. |
| `5XY0` | `SE VX, VY` | Skip if `VX == VY`. |
| `6XNN` | `LD VX, NN` | `VX = NN`. |
| `7XNN` | `ADD VX, NN` | `VX = VX + NN` (no carry). |
| `8XY0` | `LD VX, VY` | `VX = VY`. |
| `8XY1` | `OR VX, VY` | `VX = VX OR VY` (resets `VF`). |
| `8XY2` | `AND VX, VY` | `VX = VX AND VY` (resets `VF`). |
| `8XY3` | `XOR VX, VY` | `VX = VX XOR VY` (resets `VF`). |
| `8XY4` | `ADD VX, VY` | `VX = VX + VY`, `VF = carry`. |
| `8XY5` | `SUB VX, VY` | `VX = VX - VY`, `VF = NOT borrow`. |
| `8XY6` | `SHR VX {, VY}` | Shift right (VIP: from `VY`), `VF = shifted-out bit`. |
| `8XY7` | `SUBN VX, VY` | `VX = VY - VX`, `VF = NOT borrow`. |
| `8XYE` | `SHL VX {, VY}` | Shift left (VIP: from `VY`), `VF = shifted-out bit`. |
| `9XY0` | `SNE VX, VY` | Skip if `VX != VY`. |
| `ANNN` | `LD I, NNN` | `I = NNN`. |
| `BNNN` | `JP V0, NNN` | Jump to `NNN + V0`. |
| `CXNN` | `RND VX, NN` | `VX = rand() AND NN`. |
| `DXYN` | `DRW VX, VY, N` | Draw `N`-row sprite at `(VX, VY)`, XOR, `VF = collision`. |
| `EX9E` | `SKP VX` | Skip if key `VX` pressed. |
| `EXA1` | `SKNP VX` | Skip if key `VX` not pressed. |
| `FX07` | `LD VX, DT` | `VX = DT`. |
| `FX0A` | `LD VX, K` | Block for key, store in `VX`. |
| `FX15` | `LD DT, VX` | `DT = VX`. |
| `FX18` | `LD ST, VX` | `ST = VX`. |
| `FX1E` | `ADD I, VX` | `I = I + VX`. |
| `FX29` | `LD F, VX` | `I = font address for digit VX`. |
| `FX33` | `LD B, VX` | Store BCD of `VX` at `I`, `I+1`, `I+2`. |
| `FX55` | `LD [I], VX` | Store `V0`..`VX` to memory at `I`. |
| `FX65` | `LD VX, [I]` | Load `V0`..`VX` from memory at `I`. |
 
## Appendix B: Reset State Summary
 
| Component | Initial value |
| --- | --- |
| RAM | Defined/zeroed, with font loaded into reserved region |
| `V0`–`VF` | `0x00` |
| `I` | `0x000` |
| `DT`, `ST` | `0x00` |
| `PC` | Load address (`0x200` default) |
| `SP` / stack | Empty |
| Display | All pixels off |
| Keypad | All keys released |
 
## Appendix C: Source References
 
- Cowgod's Chip-8 Technical Reference v1.0 (Thomas P. Greene, 1997) — canonical opcode descriptions and memory/register/display model.
- CHIP-8 Technical Reference (Matthew Mikolay) — COSMAC VIP-faithful behavior, undocumented-instruction history, timer/keypad/font details, sprite wrapping-vs-clipping.
- *Programming in CHIP-8* (Harry Kleinberg, RCA, 1978) and the *RCA COSMAC VIP CDP18S711 Instruction Manual* (RCA, 1978) — primary-source basis for original behavior and the 12-level subroutine guarantee.
- Wikipedia, "CHIP-8" — historical context and a summary of the quirk landscape across CHIP-8 / CHIP-48 / SCHIP.
 
