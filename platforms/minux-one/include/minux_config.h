#ifndef MINUX_CONFIG_H
#define MINUX_CONFIG_H

// System Configuration for Minux RTOS

// Version Information
#define MINUX_VERSION_MAJOR 0
#define MINUX_VERSION_MINOR 1
#define MINUX_VERSION_PATCH 0
#define MINUX_VERSION_STRING "0.1.0"

// Hardware Configuration
#define ARDUINO_NANO        1
#define SCREEN_WIDTH        128
#define SCREEN_HEIGHT       64
#define OLED_RESET          -1
#define SCREEN_ADDRESS      0x3C

// Pin Definitions
#define PIN_BUZZER          9
#define PIN_BUTTON_A        2
#define PIN_BUTTON_UP       10
#define PIN_BUTTON_DOWN     8
#define PIN_BUTTON_RIGHT    7

// System Limits
#define MAX_PROCESSES       8
#define MAX_PROCESS_NAME    16
#define MAX_FILES           16
#define MAX_FILENAME        12
#define MAX_FILESIZE        256
#define MAX_CMD_LENGTH      32

// Memory Configuration
#define TOTAL_MEMORY        2048    // Arduino Nano SRAM
#define STACK_SIZE          512
#define HEAP_SIZE           768

// Timing Configuration
#define SCHEDULER_TICK_MS   10      // Scheduler time slice
#define INPUT_DEBOUNCE_MS   50      // Button debounce time
#define UI_UPDATE_MS        100     // UI refresh rate
#define FS_MAINTENANCE_MS   1000    // Filesystem maintenance

// Feature Flags
#define ENABLE_SHELL        1
#define ENABLE_FILESYSTEM   1
#define ENABLE_SOUND        1
#define ENABLE_SERIAL       1
#define ENABLE_DEBUG        1

// Debug Configuration
#if ENABLE_DEBUG
#define DEBUG_SERIAL_SPEED  115200
#define DEBUG_PRINT(x)      Serial.print(x)
#define DEBUG_PRINTLN(x)    Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

// System Defaults
#define DEFAULT_SHELL_PROMPT    "minux:/ $ "
#define DEFAULT_STATUS_TEXT     "Minux RTOS v0.1"
#define BOOT_DELAY_MS           1500
#define LOADING_STEPS           10

// Audio Configuration
#define NOTE_C4     262
#define NOTE_D4     294
#define NOTE_E4     330
#define NOTE_F4     349
#define NOTE_G4     392
#define NOTE_A4     440
#define NOTE_B4     494
#define NOTE_C5     523

// Icon Definitions
#define ICON_TERMINAL   0
#define ICON_SETTINGS   1
#define ICON_FILES      2
#define ICON_MONITOR    3

#endif // MINUX_CONFIG_H
