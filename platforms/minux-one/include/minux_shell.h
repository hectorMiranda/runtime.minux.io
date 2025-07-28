#ifndef MINUX_SHELL_H
#define MINUX_SHELL_H

#include <Arduino.h>
#include "minux_config.h"

// Forward declarations
class MinuxDisplay;
class MinuxFS;

#define MAX_ARGS 2         // Reduced from 4

class MinuxShell {
private:
  char commandBuffer[MAX_CMD_LENGTH];
  uint8_t bufferIndex;
  bool shellActive;
  
public:
  MinuxShell();
  void init();
  void processInput(char c);
  void executeCommand(const char* cmd);
  void printPrompt();
  void printHelp();
  
  // Built-in commands
  void cmd_help();
  void cmd_ls();
  void cmd_ps();
  void cmd_clear();
  void cmd_uptime();
  void cmd_mem();
  void cmd_reboot();
  void cmd_version();
  void cmd_cat(const char* filename);
  void cmd_echo(const char* text);
  
  bool isActive() { return shellActive; }
  void activate() { shellActive = true; }
  void deactivate() { shellActive = false; }
};

extern MinuxShell shell;

#endif
