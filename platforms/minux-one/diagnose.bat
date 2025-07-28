@echo off
REM Display Diagnostic Script for Minux RTOS

echo === Minux RTOS Display Diagnostic ===
echo.
echo This will test your SSD1306 OLED display connection
echo.

REM Backup main.cpp
echo Backing up main.cpp...
copy src\main.cpp src\main.cpp.backup

REM Rename files for diagnostic
echo Setting up diagnostic mode...
ren src\main.cpp main.cpp.temp
ren src\diagnostic.cpp main.cpp

echo Building diagnostic firmware...
pio run -t upload

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Upload successful! Starting serial monitor...
    echo Check the output for I2C scan results and display test.
    echo.
    echo Expected wiring:
    echo   SSD1306 VCC  -> Arduino 5V or 3.3V
    echo   SSD1306 GND  -> Arduino GND  
    echo   SSD1306 SDA  -> Arduino A4
    echo   SSD1306 SCL  -> Arduino A5
    echo.
    timeout /t 3 /nobreak > nul
    pio device monitor --baud 115200
) else (
    echo Build failed! Check your connections and try again.
)

REM Restore original files
echo.
echo Restoring original files...
ren src\main.cpp diagnostic.cpp
ren src\main.cpp.temp main.cpp

echo Diagnostic complete!
