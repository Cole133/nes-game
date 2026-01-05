# NES 1v1 Arena Shooter

A local multiplayer platform shooter developed for the Nintendo Entertainment System (NES) using C and the **cc65** compiler suite. This project demonstrates 2D physics implementation, entity management, and low-level memory optimization on the 6502 architecture.

## Overview

This is a two-player competitive shooter where players navigate platforming arenas to eliminate their opponent. The engine features custom sub-pixel physics, collision detection, a class-based loadout system, and dynamic map selection.

### Key Features
* **Local Multiplayer:** Simultaneous 1v1 gameplay.
* **Class System:**
    * **Scout:** High agility, standard projectile velocity.
    * **Sniper:** Standard agility, high projectile velocity (2x speed).
* **Physics Engine:** Custom implementation of momentum, friction, and gravity (acceleration/velocity-based movement).
* **Dynamic Power-ups:** Randomized spawns providing buffs to speed, jump height, or bullet velocity.
* **Map Selection:** Three distinct platform layouts selectable from the title screen.

## Technical Implementation

The project is written in C, utilizing `neslib` for hardware abstraction. The codebase is optimized specifically for the hardware constraints of the NES.

### Memory Management & Optimization
Due to the limited stack size and addressing modes of the MOS 6502 processor, this project adheres to specific low-level optimization strategies:
* **Zero Page Storage:** Critical variables (player coordinates, velocity vectors, game state) are allocated to the Zero Page (`#pragma bss-name(push, "ZEROPAGE")`). This significantly reduces CPU cycle cost for read/write operations compared to standard RAM addressing.
* **Global Scope:** Variables are declared globally to avoid the overhead associated with the CC65 software stack handling of local variables.
* **VRAM Updates:** Screen updates (score counters) are event-driven to minimize v-blank cycles.

### Data Structures
Level data is stored efficiently using a constant array of structs, minimizing ROM usage:

```c
struct Platform {
    unsigned char x;
    unsigned char y;
    unsigned char w;
}; ```

Collision detection uses Axis-Aligned Bounding Boxes (AABB) for both entity-vs-environment and entity-vs-projectile interactions.

## Controls

The game requires two controllers connected to the NES (or emulator).

| Button | Action |
| :--- | :--- |
| **D-Pad Left/Right** | Move Character |
| **A** | Jump |
| **B** | Shoot Projectile |
| **Select** | Return to Title Screen |
| **Start** | Begin Match (Title Screen) |

### Title Screen Navigation
* **D-Pad:** Select Map (1, 2, or 3).
* **Hold A:** Select **Sniper** Class (Display: `P1 1` / `P2 1`).
* **Hold B:** Select **Scout** Class (Display: `P1 2` / `P2 2`).

## Build Instructions

This project targets the **cc65** compiler suite and includes a batch script for automated building.

### Prerequisites
* **cc65:** The 6502 C compiler must be installed and added to your system PATH.
* **NES Libs:** Ensure the `LIB/` directory containing `neslib.h` and `nesdoug.h` is present in the root directory.

### Compilation
To compile the source code into an `.nes` ROM file, simply run the provided build script from the project root:

```cmd
compile.bat```

This script handles the compiling, assembling, and linking steps automatically.

### Running the Game
Load the resulting .nes file into any standard NES emulator (e.g., FCEUX, Mesen, or Nestopia)..