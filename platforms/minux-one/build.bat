@echo off
REM Minux RTOS Build Script for Windows
REM Usage: build.bat [clean|build|upload|monitor|all]

cd /d "%~dp0"

if "%1"=="clean" (
    echo Cleaning build files...
    pio run -t clean
    goto :eof
)

if "%1"=="build" (
    echo Building Minux RTOS...
    pio run
    goto :eof
)

if "%1"=="upload" (
    echo Uploading to device...
    pio run -t upload
    goto :eof
)

if "%1"=="monitor" (
    echo Starting serial monitor...
    pio device monitor --baud 115200
    goto :eof
)

if "%1"=="all" (
    echo Building and uploading Minux RTOS...
    pio run -t upload
    echo Starting monitor...
    timeout /t 2 /nobreak > nul
    pio device monitor --baud 115200
    goto :eof
)

echo Minux RTOS Build Script
echo Usage: %0 [clean^|build^|upload^|monitor^|all]
echo.
echo Commands:
echo   clean   - Clean build files
echo   build   - Build the project
echo   upload  - Upload to Arduino
echo   monitor - Start serial monitor
echo   all     - Build, upload, and monitor
