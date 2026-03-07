# Per-Target `sdkconfig` Setup (Any ESP Device)

This project can support multiple ESP devices (for example: `esp32`, `esp32c3`, `esp32s3`, `esp32c6`).

To avoid target mismatch errors, keep one `sdkconfig` file per target.
The build output can stay in a single local `build/` folder.

## Why This Is Needed

A common error is:

`Target '...' in sdkconfig ... does not match currently selected IDF_TARGET ...`

This happens when switching targets without cleaning old CMake cache/build metadata.

## Recommended Layout Pattern

Use this naming pattern for every target you support:

- `sdkconfig.<target>`

Examples:

- `sdkconfig.esp32`
- `sdkconfig.esp32c3`
- `sdkconfig.esp32s3`
- `sdkconfig.esp32c6`

## Create `sdkconfig` For Any Target

1. Open an ESP-IDF terminal in project root.
2. Clean the shared `build/` folder for target change.
3. Run `menuconfig` with explicit target.
4. Save/exit menuconfig.
5. Save the generated `sdkconfig` using the target-specific filename.

Command template:

```bash
idf.py -B build fullclean
idf.py -B build -DIDF_TARGET=<target> menuconfig
copy sdkconfig sdkconfig.<target>
```

Examples:

```bash
idf.py -B build fullclean
idf.py -B build -DIDF_TARGET=esp32c3 menuconfig
copy sdkconfig sdkconfig.esp32c3

idf.py -B build fullclean
idf.py -B build -DIDF_TARGET=esp32s3 menuconfig
copy sdkconfig sdkconfig.esp32s3
```

## Build/Flash For Any Target

Command template:

```bash
idf.py -B build -DIDF_TARGET=<target> -DSDKCONFIG=sdkconfig.<target> build flash monitor
```

Examples:

```bash
idf.py -B build -DIDF_TARGET=esp32 -DSDKCONFIG=sdkconfig.esp32 build flash monitor
idf.py -B build -DIDF_TARGET=esp32c3 -DSDKCONFIG=sdkconfig.esp32c3 build flash monitor
idf.py -B build -DIDF_TARGET=esp32s3 -DSDKCONFIG=sdkconfig.esp32s3 build flash monitor
idf.py -B build -DIDF_TARGET=esp32c6 -DSDKCONFIG=sdkconfig.esp32c6 build flash monitor
```

If multiple boards are connected, set port explicitly:

```bash
idf.py -p COM8 ...
```

## About `flash.bat`

Current `flash.bat` mappings in this repository support:

- `esp32`
- `esp32c6`
- `auto` detection (currently routed to supported targets above)

The script uses one local `build/` folder and runs `fullclean` automatically when switching target.

To support additional targets like `esp32c3` or `esp32s3`, add new target mappings in `flash.bat`:

- `TARGET=esp32c3` -> `BUILD_DIR=build`, `SDKCFG=sdkconfig.esp32c3`
- `TARGET=esp32s3` -> `BUILD_DIR=build`, `SDKCFG=sdkconfig.esp32s3`

## Notes

- `idf.py` defaults to `sdkconfig` unless `-DSDKCONFIG=...` is provided.
- Always keep `IDF_TARGET`, build directory, and `sdkconfig` aligned.
- One shared local `build/` folder is OK if you clean or auto-clean on target switch.
