#ifndef MINUX_SYSTEM_H
#define MINUX_SYSTEM_H

#include "minux_config.h"
#include "minux_kernel.h"
#include "minux_display.h"
#include "minux_input.h"
#include "minux_scheduler.h"
#include "minux_fs.h"
#include "minux_shell.h"

// System state management
enum SystemMode {
  MODE_BOOT,
  MODE_DESKTOP,
  MODE_TERMINAL,
  MODE_SYSINFO,
  MODE_FILES,
  MODE_ERROR
};

class MinuxSystem {
private:
  SystemMode currentMode;
  bool systemReady;
  unsigned long bootTime;
  
public:
  MinuxSystem();
  
  // System lifecycle
  bool init();
  void run();
  void shutdown();
  void reboot();
  
  // Mode management
  void setMode(SystemMode mode);
  SystemMode getMode() { return currentMode; }
  
  // System status
  bool isReady() { return systemReady; }
  unsigned long getBootTime() { return bootTime; }
  
  // Error handling
  void handleError(const char* message);
  void systemPanic(const char* reason);
};

// Global system instance
extern MinuxSystem minuxSystem;

// System event handlers
void handleSystemInput();
void updateSystemStatus();
void performSystemMaintenance();

#endif // MINUX_SYSTEM_H
