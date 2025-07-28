#ifndef MINUX_KERNEL_H
#define MINUX_KERNEL_H

#include <Arduino.h>

// Forward declarations
class MinuxScheduler;
class ProcessControlBlock;

// System constants
#define MAX_PROCESSES 4     // Reduced from 8
#define MAX_PROCESS_NAME 8  // Reduced from 16
#define KERNEL_VERSION "0.1.0"

// System states
enum SystemState {
  SYS_BOOT,
  SYS_RUNNING,
  SYS_SLEEP,
  SYS_ERROR,
  SYS_SHUTDOWN
};

// Process states
enum ProcessState {
  PROC_READY,
  PROC_RUNNING,
  PROC_BLOCKED,
  PROC_TERMINATED
};

// Memory management
struct MemInfo {
  uint16_t total;
  uint16_t free;
  uint16_t used;
  uint8_t fragmentation;
};

class MinuxKernel {
private:
  SystemState currentState;
  unsigned long bootTime;
  unsigned long uptime;
  MemInfo memory;
  
public:
  MinuxKernel();
  void init();
  void panic(const char* message);
  SystemState getState() { return currentState; }
  void setState(SystemState state) { currentState = state; }
  unsigned long getUptime();
  MemInfo getMemoryInfo();
  void updateMemoryInfo();
  const char* getVersion() { return KERNEL_VERSION; }
};

// Global system calls
void idle_task();
void ui_task();
void input_task();
void fs_task();

#endif
