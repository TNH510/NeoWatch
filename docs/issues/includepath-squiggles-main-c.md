# Include Errors In `main/main.c` (IntelliSense)

## Issue
VS Code reported:

`#include errors detected. Please update your includePath. Squiggles are disabled for this translation unit (main/main.c).`

The source code in `main/main.c` was valid, but IntelliSense could not resolve includes.

## Root Cause
1. ESP-IDF target configuration mismatch after sync/pull.
2. C/C++ extension was not explicitly using the generated compile database.
3. Build metadata required by IntelliSense was not available or not wired:
   - `build/compile_commands.json`
   - `build/config/sdkconfig.h`

## Solution Applied
1. Set ESP-IDF target to `esp32c6` and regenerated build configuration.
2. Verified generated metadata files exist:
   - `build/compile_commands.json`
   - `build/config/sdkconfig.h`
3. Updated workspace settings in `.vscode/settings.json`:

```json
"C_Cpp.default.compileCommands": "${workspaceFolder}/build/compile_commands.json"
```

## Result
- IntelliSense include resolution works for `main/main.c`.
- Include squiggles are removed.

## If Warning Still Appears In VS Code
1. Run `C/C++: Reset IntelliSense Database`.
2. Run `Developer: Reload Window`.
