@echo off
setlocal EnableDelayedExpansion

cd /d "%~dp0"

where idf.py >nul 2>&1
if errorlevel 1 (
    echo [ERROR] idf.py not found in PATH.
    echo Open an ESP-IDF terminal in VS Code, or run export.bat first.
    exit /b 1
)

where python >nul 2>&1
if errorlevel 1 (
    echo [ERROR] python not found in PATH.
    echo ESP-IDF environment Python is required for auto chip detection.
    exit /b 1
)

set MODE=%~1
if "%MODE%"=="" goto :usage

if /I "%MODE%"=="esp32" (
    set TARGET=esp32
    shift
    goto :run_explicit
)

if /I "%MODE%"=="esp32c6" (
    set TARGET=esp32c6
    shift
    goto :run_explicit
)

if /I "%MODE%"=="auto" (
    set PORT=%~2
    if /I "%PORT:~0,3%"=="COM" (
        shift
        shift
    ) else (
        set PORT=
        shift
    )
    goto :detect_and_run
)

echo [ERROR] Unknown mode: %MODE%
goto :usage

:run_explicit
if "%~1"=="" (
    set ACTION=build flash monitor
) else (
    set ACTION=%*
)
goto :select_target

:detect_and_run
if "%~1"=="" (
    set ACTION=build flash monitor
) else (
    set ACTION=%*
)

set TMPFILE=%TEMP%\esptool_chip_%RANDOM%%RANDOM%.log
if defined PORT (
    python -m esptool --port %PORT% chip_id > "%TMPFILE%" 2>&1
) else (
    python -m esptool chip_id > "%TMPFILE%" 2>&1
)
if errorlevel 1 (
    if defined PORT (
        echo [ERROR] Failed to detect chip on %PORT%.
    ) else (
        echo [ERROR] Failed to auto-detect chip on available serial ports.
    )
    type "%TMPFILE%"
    del /q "%TMPFILE%" >nul 2>&1
    exit /b 1
)

findstr /I /C:"ESP32-C6" "%TMPFILE%" >nul && set TARGET=esp32c6
if not defined TARGET (
    findstr /I /R /C:"Detecting chip type... ESP32$" /C:"Chip is ESP32" "%TMPFILE%" >nul && set TARGET=esp32
)

del /q "%TMPFILE%" >nul 2>&1

if not defined TARGET (
    if defined PORT (
        echo [ERROR] Unsupported or unknown chip on %PORT%.
    ) else (
        echo [ERROR] Unsupported or unknown chip from auto-detected serial port.
    )
    echo Supported by this script: ESP32, ESP32-C6.
    exit /b 1
)

:select_target
if /I "%TARGET%"=="esp32" (
    set BUILD_DIR=build
    set SDKCFG=sdkconfig.esp32
)

if /I "%TARGET%"=="esp32c6" (
    set BUILD_DIR=build
    set SDKCFG=sdkconfig.esp32c6
)

if "%BUILD_DIR%"=="" (
    echo [ERROR] Internal target mapping failed for target %TARGET%.
    exit /b 1
)

if not exist "%SDKCFG%" (
    echo [ERROR] Missing %SDKCFG%.
    echo Create it first using idf.py for target %TARGET%.
    exit /b 1
)

set CACHE_FILE=%BUILD_DIR%\CMakeCache.txt
if exist "%CACHE_FILE%" (
    set PREV_TARGET=
    for /f "tokens=2 delims==" %%A in ('findstr /B /C:"IDF_TARGET:STRING=" "%CACHE_FILE%"') do set PREV_TARGET=%%A
    if defined PREV_TARGET (
        if /I not "!PREV_TARGET!"=="%TARGET%" (
            echo [INFO] Target changed from !PREV_TARGET! to %TARGET%.
            echo [INFO] Running fullclean in %BUILD_DIR% to avoid CMake target mismatch.
            idf.py -B %BUILD_DIR% fullclean
            if errorlevel 1 exit /b %ERRORLEVEL%
        )
    )
)

echo [INFO] Target: %TARGET%
echo [INFO] Build dir: %BUILD_DIR%
echo [INFO] Config: %SDKCFG%

if /I "%MODE%"=="auto" (
    if defined PORT (
        echo [RUN] idf.py -B %BUILD_DIR% -DIDF_TARGET=%TARGET% -DSDKCONFIG=%SDKCFG% -p %PORT% %ACTION%
        idf.py -B %BUILD_DIR% -DIDF_TARGET=%TARGET% -DSDKCONFIG=%SDKCFG% -p %PORT% %ACTION%
    ) else (
        echo [RUN] idf.py -B %BUILD_DIR% -DIDF_TARGET=%TARGET% -DSDKCONFIG=%SDKCFG% %ACTION%
        idf.py -B %BUILD_DIR% -DIDF_TARGET=%TARGET% -DSDKCONFIG=%SDKCFG% %ACTION%
    )
) else (
    echo [RUN] idf.py -B %BUILD_DIR% -DIDF_TARGET=%TARGET% -DSDKCONFIG=%SDKCFG% %ACTION%
    idf.py -B %BUILD_DIR% -DIDF_TARGET=%TARGET% -DSDKCONFIG=%SDKCFG% %ACTION%
)

exit /b %ERRORLEVEL%

:usage
echo.
echo Usage:
echo   flash.bat esp32 [idf_actions...]
echo   flash.bat esp32c6 [idf_actions...]
echo   flash.bat auto [idf_actions...]
echo   flash.bat auto COMx [idf_actions...]
echo.
echo Examples:
echo   flash.bat esp32
echo   flash.bat esp32 build
echo   flash.bat esp32c6 -p COM9 flash monitor
echo   flash.bat auto
echo   flash.bat auto COM8
echo   flash.bat auto COM9 flash monitor
echo.
exit /b 1
