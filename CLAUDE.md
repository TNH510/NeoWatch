# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**NeoWatch** is an embedded smartwatch firmware project built on ESP32-C6 using ESP-IDF v5.5.2. The system combines a round OLED display (GC9A01), BLE connectivity, RTC, button inputs, and LVGL graphics library to deliver time display, customizable themes, and smartphone integration.

**Languages:** C  
**Build System:** CMake (ESP-IDF)  
**Target Platforms:** ESP32-S3 (primary), ESP32-C6, ESP32

## Prerequisites & Setup

### Development Environment

- **ESP-IDF v5.5.2**: [Download](https://dl.espressif.com/dl/esp-idf/)
- Set environment variables:
  - `IDF_PATH`: Path to ESP-IDF installation
  - `IDF_TOOLS_PATH`: Path to toolchain (e.g., `D:\program_files\Espressif`)
  - `IFX_TOOLBOX_UUID`: Toolbox UUID (shown when running ESP-IDF CMD)
- CMake 3.5+ and platform-specific toolchains
- VS Code with C/C++ extensions (optional but recommended)

### DevContainer Option

A `.devcontainer` is included for consistent environments using Docker + VS Code.

## Build & Flash Commands

```bash
# Initial setup
idf.py add-dependency "lvgl/lvgl"
idf.py update-dependencies

# Core build commands
idf.py build              # Build firmware (uses sdkconfig for current target)
idf.py flash              # Flash to connected device
idf.py monitor            # Serial monitor for logs
idf.py build monitor      # Build + monitor (common workflow)

# Multi-target development (see docs/sdkconfig_per_target.md)
idf.py -B build_esp32s3 build    # Build ESP32-S3 variant
idf.py -B build_esp32c6 build    # Build ESP32-C6 variant
idf.py -B build_esp32 build      # Build ESP32 variant
idf.py -B build_esp32s3 flash    # Flash ESP32-S3

# Cleanup
idf.py fullclean          # Remove all build artifacts
```

**Note:** Multi-target builds use separate build directories (e.g., `build_esp32c6/`) with their own `sdkconfig`. Copy `sdkconfig` to `sdkconfig.defaults` or use `idf.py build` in the target directory.

## Architecture Overview

### Layered Architecture

The firmware follows a strict layered design (bottom to top):

1. **BSP (Board Support Package)** — Hardware abstraction
   - Location: `source/src/bsp/`, `source/inc/bsp/`
   - Modules: I2C, LCD (GC9A01), RTC, GPIO, button, flash
   - Pure hardware initialization and low-level drivers

2. **Driver Layer** — Protocol and peripheral control
   - Location: `source/src/driver/`, `source/inc/driver/`
   - Modules: Button state machine, GPIO control
   - Builds on BSP; no BSP awareness of driver logic

3. **Middleware** — Utilities and optional features
   - Location: `source/src/middleware/`, `source/inc/middleware/`
   - Modules: Fonts, animations, games, graphics helpers
   - Self-contained; rarely depends on BSP/driver

4. **System Layer** — Business logic and inter-subsystem coordination
   - Location: `source/src/system/`, `source/inc/system/`
   - Modules: System Manager (central event router), Display Manager, Button Manager, UI Manager, Settings Manager, Network Manager (planned)
   - Communicates via unified event message queues (see System Manager architecture below)

5. **UI / Application** — LVGL-based graphical interface
   - Location: `ui/export/c/` (auto-generated from SquareLine Studio)
   - Widgets, screens, event callbacks

### System Manager (Event-Driven Architecture)

The **System Manager** is the central nervous system:

```
Button -> SM_EVENT (routing) -> Button Queue
Display -> SM_EVENT (routing) -> Display Queue  
UI -> SM_EVENT (routing) -> UI Queue
Settings -> SM_EVENT (routing) -> Settings Queue
Network -> SM_EVENT (routing) -> Network Queue
```

Each subsystem:

1. **Publishes** events via System Manager (`system_manager_post_event()`)
2. **Consumes** its own queue independently (event-driven, non-blocking)
3. **Responds** by publishing new events back (loose coupling, no direct calls)

**Key benefit:** Subsystems are decoupled and can be tested in isolation with mock queues.

#### Unified Message Structure

All inter-subsystem communication flows through `sm_event_msg_t`:

```c
typedef struct
{
    sm_event_source_t source;       /* Sender: BUTTON, DISPLAY, UI, NETWORK, SETTINGS */
    uint16_t event_id;              /* Event enum (e.g., BUTTON_CLICKED) */
    uint32_t timestamp_ms;
    union {
        struct { uint8_t button_id; } button;
        struct { uint8_t brightness; uint8_t mode; } display;
        struct { uint8_t arg0; uint8_t arg1; } ui;
        struct { /* settings payload */ } settings;
    } data;
} sm_event_msg_t;
```

See `docs/sdd/system_manager/SDD_System_Manager.md` for the full event catalog.

### Subsystem Responsibilities

| Subsystem | Files | Role |
|-----------|-------|------|
| **Button System** | `system_button.c` | Detects physical input, publishes click/double-click/hold events |
| **Display System** | `system_display.c` | Renders LVGL UI, manages brightness, sleep mode |
| **UI System** | LVGL + `ui/export/c/` | Manages screens, widget state, theme changes |
| **Settings System** | `system_settings.c` | Persists preferences (RTC, timezone, clock format) to flash |
| **Network System** | *(planned)* | BLE communication, app sync, theme downloads |

## Code Structure

```
source/
├── inc/              # Public headers (layer interfaces)
│   ├── bsp/          # BSP headers (I2C, LCD, RTC, GPIO)
│   ├── driver/       # Driver layer headers
│   ├── middleware/   # Middleware utilities
│   ├── system/       # System manager and subsystems
│   └── common/       # Shared types and constants
└── src/              # Implementation
    ├── bsp/          # BSP drivers (I2C, LCD, RTC, GPIO, flash)
    ├── driver/       # Driver implementations
    ├── middleware/   # Utilities, fonts, animations
    └── system/       # System manager, subsystem logic

main/
├── main.c            # ESP-IDF entry point (calls system_manager_init())
└── CMakeLists.txt    # Main component registration and includes

ui/
├── *.sll, *.spj      # SquareLine Studio project files
└── export/c/         # Auto-generated LVGL code from SquareLine

docs/
├── architecture/     # SAD, SRS, system diagrams
├── sdd/              # Detailed design documents per subsystem
├── style/            # Coding style guide (C conventions)
└── config/           # Configuration guides
```

## Key Files & Entry Points

- **`main/main.c`** — Firmware entry point; calls `system_manager_init()`
- **`source/src/system/system_manager.c`** — Event routing and queue management
- **`source/src/system/system_display.c`** — Display and LVGL rendering loop
- **`source/src/system/system_button.c`** — Button event detection and dispatch
- **`source/src/bsp/bsp.c`** — BSP initialization (I2C, LCD, RTC, GPIO)
- **`ui/export/c/ui.c`** — LVGL screen definitions and callbacks (auto-generated)

## Coding Style & Conventions

See `docs/style/coding_style_c.md` for comprehensive guidelines. Key points:

- **Naming:**
  - Constants: `UPPERCASE_WITH_UNDERSCORE`
  - Variables, functions: `lowercase_with_underscore`
  - Enums: `typedef enum { ENUM_VALUE } enum_name_t;`
  - Structs: `typedef struct { ... } struct_name_t;`

- **Formatting:**
  - `.clang-format` file included; run `clang-format -i <file>` to auto-format
  - Indentation: 4 spaces (never tabs)
  - Line length: aim for ≤100 chars where practical

- **Code Organization:**
  - Public headers in `inc/`; private headers in `src/` alongside `.c`
  - Functions grouped logically; static helpers at bottom
  - Always include guards: `#ifndef HEADER_H` / `#define HEADER_H` / `#endif`

## Build Configuration

**`sdkconfig.defaults`** sets up:

- BLE-only mode (NimBLE stack, not Bluedroid)
- ESP32-S3 as primary target (configurable to ESP32-C6 or ESP32)
- LVGL and ESP-IDF components

Multi-target builds keep separate `sdkconfig` files per build directory; see `docs/config/sdkconfig_per_target.md`.

## Dependencies

Managed via ESP-IDF component manager:

- **lvgl/lvgl** (v9.3.0) — Graphics library
- **espressif/button** (v4.1.3) — Button driver component
- **espressif/cmake_utilities** — Build helpers
- **espressif/touch_sensor_fsm** — Touch sensor state machine
- **ESP-IDF v5.5.2** — Core framework

## Design Documentation

Read these in order for deep understanding:

1. **`docs/architecture/SRS.md`** — Software Requirements Specification (features, use cases)
2. **`docs/architecture/SAD.md`** — Software Architecture Document (overview, diagrams)
3. **`docs/sdd/system_manager/SDD_System_Manager.md`** — Central event routing
4. **`docs/sdd/system_display/SDD_System_Display.md`** — Display manager and LVGL loop
5. **`docs/sdd/system_button/SDD_System_Button.md`** — Button event detection
6. **`docs/sdd/system_ui/SDD_System_UI.md`** — UI state and screen navigation
7. **`docs/sdd/system_settings/SDD_System_Settings.md`** — Settings persistence
8. **`docs/sdd/system_network/SDD_System_Network.md`** — BLE communication (planned)

## Common Development Tasks

### Adding a New System Event

1. Define event enum in subsystem header (e.g., `system_button.h`)
2. Add event routing rule in `system_manager.c` (which queues receive it)
3. Add handler in target subsystem's main loop
4. Document the event in the relevant SDD

### Adding a New Button or Input

1. Update BSP driver (`bsp_button.c` or `bsp_gpio.c`)
2. Register callback in Driver layer (`drv_button.c`)
3. Publish button event via System Manager in `system_button.c`
4. Handle event in target subsystem queue (usually UI or Display)

### Modifying UI Screens

1. Edit `ui/*.sll` or `ui/*.spj` (SquareLine Studio projects)
2. Export C code to `ui/export/c/`
3. LVGL callbacks are auto-generated; add business logic via System Manager events
4. Rebuild with `idf.py build`

### Debugging

- **Serial logs:** `idf.py monitor` (check `ESP_LOG*` macros in code)
- **Build logs:** Run `idf.py build -v` for verbose output
- **Flash issues:** Try `idf.py -B build fullclean && idf.py build`

## Git Workflow

The project uses feature branches (e.g., `feature/system-design`). Key conventions:

- Branch from `main` for new features
- Keep commits focused; include context in messages
- Check `BRANCH_SUMMARY.md` for recent changes and status
- Detailed design documents are tracked in `docs/sdd/`; update them before/during implementation

## Recent Architecture Changes

- **System Manager (latest):** Event-driven design with unified message queues (all subsystems decouple via events, no direct calls)
- **Detailed System Design Documents:** Per-subsystem architecture in `docs/sdd/` (manager pattern, state machines, event routing)
- **UI Generation:** LVGL screens auto-exported from SquareLine Studio; C callbacks wired to System Manager

## Quick Reference

- **Compile:** `idf.py build` (builds for ESP32-S3 by default)
- **Flash + Monitor:** `idf.py build monitor`
- **Multi-target (ESP32-C6):** `idf.py -B build_esp32c6 build`
- **Multi-target (ESP32-S3):** `idf.py -B build_esp32s3 build`
- **Check code style:** `clang-format -i source/src/system/*.c`
- **Read architecture:** Start with `docs/architecture/SAD.md`, then `docs/sdd/system_manager/`
- **Trace an event:** Follow the flow in System Manager `post_event()` → queue routing → subsystem handler → response event
