# Building main.c for ESP-IDF – NeoWatch Guide

## Table of Contents

1. [Overview](#overview)
2. [ESP-IDF Project Structure](#esp-idf-project-structure)
3. [The app_main() Entry Point](#the-app_main-entry-point)
4. [File Header Convention](#file-header-convention)
5. [main.c Structure](#mainc-structure)
6. [CMakeLists.txt – Main Component](#cmakeliststxt--main-component)
7. [CMakeLists.txt – Project Root](#cmakeliststxt--project-root)
8. [Adding Dependencies](#adding-dependencies)
9. [Build Commands](#build-commands)
10. [Environment Setup](#environment-setup)
11. [Troubleshooting](#troubleshooting)

---

## Overview

The ESP-IDF (Espressif IoT Development Framework) uses a component-based build system built on top of **CMake**. The entry point of every ESP-IDF application is the `app_main()` function, which resides in `main/main.c`. Understanding how to correctly create, register, and build this file is the foundation of any ESP32 project.

This document covers the structure and conventions used in the **NeoWatch** project.

---

## ESP-IDF Project Structure

A minimal ESP-IDF project that matches the NeoWatch layout looks like this:

```
project-root/
├── CMakeLists.txt          # Top-level project CMakeLists
├── sdkconfig               # Generated configuration (do not edit manually)
├── sdkconfig.defaults      # Default Kconfig values committed to the repo
├── dependencies.lock       # Locked dependency versions
├── main/                   # Main application component (required by ESP-IDF)
│   ├── CMakeLists.txt      # Component registration
│   ├── main.c              # Application entry point
│   └── idf_component.yml   # IDF Component Manager manifest
└── source/                 # Additional application source code
    ├── inc/                # Public header files
    └── src/                # Implementation files
```

> **Rule:** ESP-IDF always looks for a component named `main`. Its `CMakeLists.txt` must call `idf_component_register()`.

---

## The app_main() Entry Point

In standard C the entry point is `main()`. In ESP-IDF it is **`app_main()`**. The framework creates the necessary FreeRTOS infrastructure before calling this function, so `app_main()` runs in its own FreeRTOS task.

```c
void app_main(void)
{
    // Initialization code here

    while (1)
    {
        vTaskDelay(portMAX_DELAY); // Prevent the task from returning
    }
}
```

Key points:
- `app_main()` must **not return**. If it returns, the application will restart.
- Use `vTaskDelay(portMAX_DELAY)` in the main loop when all work is handed off to other FreeRTOS tasks.
- `#include "freertos/FreeRTOS.h"` and `#include "freertos/task.h"` are available automatically via the ESP-IDF build system.

---

## File Header Convention

All `.c` files in NeoWatch follow this Doxygen-style header (see `docs/style/coding_style_c.md`):

```c
/**
 * @file       main.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     <Author Name>
 * @brief      Main file
 * @note       None
 */
```

Section dividers use the following style:

```c
/* Includes ----------------------------------------------------------- */
/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */
/* End of file -------------------------------------------------------- */
```

---

## main.c Structure

Below is the complete `main/main.c` for the NeoWatch project:

```c
/**
 * @file       main.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     Hieu Tran Ngoc
 * @brief      Main file
 * @note       None
 */
/* Includes ----------------------------------------------------------- */
#include "system_manager.h"

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */
void app_main(void)
{
    // Initialize system manager (BSP + all subsystems)
    system_manager_init();

    while (1)
    {
        // All work is performed by FreeRTOS tasks spawned inside
        // system_manager_init(). This loop simply prevents app_main
        // from returning.
        vTaskDelay(portMAX_DELAY);
    }
}
/* End of file -------------------------------------------------------- */
```

### What `system_manager_init()` does

`system_manager_init()` is the single initialisation call that brings up every subsystem in the correct order:

| Order | Subsystem            | Purpose                                          |
|-------|----------------------|--------------------------------------------------|
| 1     | BSP I2C              | Initialises the I2C master bus                   |
| 2     | BSP LCD              | Initialises the SSD1306 display and LVGL         |
| 3     | BSP RTC              | Initialises the real-time clock                  |
| 4     | System Display task  | Starts the FreeRTOS task that drives the display |

---

## CMakeLists.txt – Main Component

The file `main/CMakeLists.txt` registers the `main` component with the ESP-IDF build system using `idf_component_register()`:

```cmake
idf_component_register(
    SRCS
        "main.c"
        ${CMAKE_SOURCE_DIR}/source/src/bsp/bsp_i2c.c
        ${CMAKE_SOURCE_DIR}/source/src/bsp/bsp_lcd.c
        ${CMAKE_SOURCE_DIR}/source/src/bsp/bsp_rtc.c
        ${CMAKE_SOURCE_DIR}/source/src/system/system_display.c
        ${CMAKE_SOURCE_DIR}/source/src/system/system_manager.c
    REQUIRES
        main
    INCLUDE_DIRS
        "."
)

# Application headers
include_directories(${CMAKE_SOURCE_DIR}/source/inc/bsp)
include_directories(${CMAKE_SOURCE_DIR}/source/inc/common)
include_directories(${CMAKE_SOURCE_DIR}/source/inc/driver)
include_directories(${CMAKE_SOURCE_DIR}/source/inc/middleware)
include_directories(${CMAKE_SOURCE_DIR}/source/inc/system)
```

### Key parameters of `idf_component_register()`

| Parameter      | Description                                                        |
|----------------|--------------------------------------------------------------------|
| `SRCS`         | List of `.c` source files to compile into this component           |
| `REQUIRES`     | Other ESP-IDF components or custom components this component needs |
| `INCLUDE_DIRS` | Public header directories exposed to other components              |

> **Note:** `include_directories()` after `idf_component_register()` adds *private* include paths that are not exported to other components.

---

## CMakeLists.txt – Project Root

The root `CMakeLists.txt` sets the minimum CMake version, pulls in the ESP-IDF toolchain, and names the project:

```cmake
# For more information about the build system see:
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(neo-watch)
```

The `include($ENV{IDF_PATH}/tools/cmake/project.cmake)` line must come **before** `project()`. It imports all ESP-IDF CMake logic and must be present in every project root.

---

## Adding Dependencies

### Using idf_component.yml (recommended)

Place an `idf_component.yml` manifest inside the `main/` folder to declare component dependencies:

```yaml
## IDF Component Manager Manifest File
dependencies:
  idf:
    version: '>=4.1.0'
  espressif/button: '*'
  lvgl/lvgl: '*'
  masuidrive/nimble-nordic-uart: ^1.0.3
```

Then run:

```bash
idf.py update-dependencies
```

This downloads dependencies to `managed_components/` and updates `dependencies.lock`.

### Adding a single dependency from the CLI

```bash
idf.py add-dependency "lvgl/lvgl"
idf.py add-dependency "masuidrive/nimble-nordic-uart^1.0.3"
```

---

## Build Commands

All commands are run from the **project root** after activating the ESP-IDF environment.

| Command                        | Description                                              |
|-------------------------------|----------------------------------------------------------|
| `idf.py set-target esp32`     | Set the target chip (only needed once)                   |
| `idf.py menuconfig`           | Open interactive Kconfig menu to configure the project   |
| `idf.py build`                | Compile the entire project                               |
| `idf.py flash`                | Flash the compiled firmware to the connected device      |
| `idf.py monitor`              | Open serial monitor to see ESP32 log output              |
| `idf.py flash monitor`        | Flash and immediately open the serial monitor            |
| `idf.py fullclean`            | Delete all build artefacts (use before a clean rebuild)  |

### Typical first-time build workflow

```bash
# 1. Activate ESP-IDF environment (once per terminal session)
. $IDF_PATH/export.sh          # Linux/macOS
# or: export.bat               # Windows CMD
# or: export.ps1               # Windows PowerShell

# 2. Set the target (only needed once per project)
idf.py set-target esp32

# 3. (Optional) Customise Kconfig options
idf.py menuconfig

# 4. Build
idf.py build

# 5. Flash and monitor
idf.py flash monitor
```

---

## Environment Setup

### Option A – Dev Container (recommended for NeoWatch)

The repository ships with a `.devcontainer/` folder. Open the project in VS Code and choose **"Reopen in Container"**. The container is pre-configured with:

- ESP-IDF toolchain
- All required environment variables (`IDF_PATH`, `PATH`)
- VS Code extensions: `espressif.esp-idf-extension`

See `.devcontainer/Dockerfile` and `.devcontainer/devcontainer.json` for details.

### Option B – Local installation

1. Install prerequisites for your OS following the [official ESP-IDF Getting Started guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/).
2. Clone ESP-IDF and run the install script:

   ```bash
   git clone --recursive https://github.com/espressif/esp-idf.git
   cd esp-idf
   ./install.sh esp32
   ```

3. Activate the environment before every build session:

   ```bash
   . ./export.sh
   ```

### Required ESP-IDF version

The NeoWatch project targets **ESP-IDF v5.4.0** (see `dependencies.lock`). Using a different version may cause compatibility issues with managed components.

---

## Troubleshooting

### `IDF_PATH is not set`
Activate the ESP-IDF environment: `. $IDF_PATH/export.sh`

### `No such file or directory: idf.py`
Ensure `$IDF_PATH/tools` is on your `PATH`. Re-run `. $IDF_PATH/export.sh`.

### `CMake Error: idf_component_register called without SRCS`
Add at least one source file to the `SRCS` list in `main/CMakeLists.txt`.

### `app_main returned` (restart loop)
The `app_main()` function returned. Add `vTaskDelay(portMAX_DELAY)` at the end of the function to prevent this.

### Build fails with missing header
Ensure the header's directory is listed in `include_directories()` or in the `INCLUDE_DIRS` argument of `idf_component_register()`.

### Component dependency not found
Run `idf.py update-dependencies` to download all declared components from the IDF Component Registry.

---

## References

- [ESP-IDF Build System](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html)
- [ESP-IDF Getting Started](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/)
- [IDF Component Manager](https://docs.espressif.com/projects/idf-component-manager/en/latest/)
- [FreeRTOS with ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/freertos.html)
- NeoWatch `docs/style/coding_style_c.md`
- NeoWatch `docs/BSP_LCD_Documentation.md`
