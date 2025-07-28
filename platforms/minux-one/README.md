# Minux RTOS - Minimal Real-Time Operating System for Arduino

## Overview

Minux RTOS is a minimal real-time operating system designed for Arduino platforms, featuring a microkernel architecture with essential OS components.

## Features

### Core Components
- **Kernel**: Process management, memory management, system calls
- **Scheduler**: Round-robin cooperative multitasking
- **Display Manager**: GUI with desktop environment and terminal
- **Input System**: Button handling with debouncing and sound feedback
- **File System**: In-memory filesystem with directories and files
- **Shell**: Command-line interface with built-in commands

### Hardware Support
- Arduino Nano (ATmega328P)
- SSD1306 OLED Display (128x64)
- 4 Buttons (A, UP, DOWN, RIGHT)
- Buzzer for audio feedback

## System Architecture

```
┌─────────────────────────────────────────┐
│                Applications            │
├─────────────────────────────────────────┤
│            Shell & GUI                  │
├─────────────────────────────────────────┤
│     Filesystem    │      Display        │
├─────────────────────────────────────────┤
│      Input        │     Scheduler       │
├─────────────────────────────────────────┤
│              Kernel                     │
├─────────────────────────────────────────┤
│          Hardware Abstraction          │
└─────────────────────────────────────────┘
```

## File Structure

```
platforms/minux-one/
├── include/
│   ├── minux_kernel.h      # Kernel interface
│   ├── minux_display.h     # Display management
│   ├── minux_input.h       # Input handling
│   ├── minux_scheduler.h   # Process scheduler
│   ├── minux_fs.h          # Filesystem
│   └── minux_shell.h       # Command shell
├── lib/
│   ├── MinuxKernel/        # Kernel implementation
│   ├── MinuxDisplay/       # Display driver
│   ├── MinuxInput/         # Input driver
│   ├── MinuxScheduler/     # Scheduler implementation
│   ├── MinuxFS/            # Filesystem implementation
│   └── MinuxShell/         # Shell implementation
├── src/
│   └── main.cpp            # Main application
└── platformio.ini          # Build configuration
```

## Getting Started

### Prerequisites
- PlatformIO IDE or CLI
- Arduino Nano or compatible board
- SSD1306 OLED display
- Push buttons and buzzer

### Wiring
```
Arduino Nano → SSD1306 OLED
GND         → GND
VCC         → 3.3V/5V
SCL         → A5
SDA         → A4

Arduino Nano → Buttons
D2          → Button A
D10         → Button UP
D8          → Button DOWN
D7          → Button RIGHT

Arduino Nano → Buzzer
D9          → Buzzer +
```

### Building and Uploading
```bash
# Clone the repository
git clone <repository-url>
cd platforms/minux-one

# Build and upload
pio run -t upload

# Monitor serial output
pio device monitor
```

## Usage

### Desktop Mode
- **Button A**: Enter terminal mode
- **Button UP**: Show system information
- **Button DOWN**: Browse filesystem
- **Button RIGHT**: Show running processes

### Terminal Mode
- Connect via serial monitor (115200 baud)
- Available commands:
  - `help` - Show available commands
  - `ls` - List files and directories
  - `ps` - Show running processes
  - `mem` - Display memory usage
  - `uptime` - Show system uptime
  - `version` - Display version information
  - `cat <file>` - Display file contents
  - `clear` - Clear screen
  - `reboot` - Restart system

### System Information
The system provides real-time information about:
- Memory usage (total, used, free)
- Running processes
- System uptime
- Filesystem contents

## System Calls and API

### Kernel API
```cpp
kernel.getUptime()        // Get system uptime
kernel.getMemoryInfo()    // Get memory statistics
kernel.panic(message)     // Trigger kernel panic
```

### Scheduler API
```cpp
scheduler.startProcess(name, function, interval)  // Start new process
scheduler.stopProcess(name)                       // Stop process
scheduler.tick()                                  // Run scheduler
```

### Filesystem API
```cpp
filesystem.createFile(name, data, size)  // Create file
filesystem.openFile(name)                // Open file
filesystem.createDir(name)               // Create directory
filesystem.listFiles()                   // List all files
```

## Memory Layout

```
Arduino Nano (2KB SRAM)
├── Stack (growing down)     ~512 bytes
├── Heap (dynamic allocation) ~768 bytes
├── Global variables         ~512 bytes
└── System structures        ~256 bytes
```

## Process Management

Minux RTOS uses cooperative multitasking with the following system processes:
- **idle**: Background idle process
- **ui**: Display updates and GUI management  
- **input**: Button polling and event handling
- **fs**: Filesystem maintenance

## Development

### Adding New Features
1. Create header file in `include/`
2. Implement in `lib/YourModule/`
3. Include in main.cpp
4. Register with scheduler if needed

### Debugging
- Use Serial output for debugging
- Monitor memory usage with `mem` command
- Check process states with `ps` command

## Limitations

- Single-threaded cooperative multitasking
- Limited to 2KB RAM (Arduino Nano)
- No persistent storage (in-memory filesystem)
- Basic GUI with monochrome display
- Maximum 8 concurrent processes

## Future Enhancements

- [ ] Preemptive multitasking
- [ ] EEPROM-based persistent storage
- [ ] Network stack
- [ ] More hardware drivers
- [ ] Advanced GUI widgets
- [ ] Inter-process communication
- [ ] Power management

## License

MIT License - see LICENSE file for details

## Contributing

1. Fork the repository
2. Create feature branch
3. Make changes
4. Test thoroughly
5. Submit pull request

## Version History

- v0.1.0 - Initial release with basic OS components
  - Kernel with memory management
  - Cooperative scheduler
  - Basic filesystem
  - GUI desktop environment
  - Command shell
  - Hardware drivers for display, input, sound
