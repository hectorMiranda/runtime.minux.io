#include "minux_shell.h"
#include "minux_kernel.h"
#include "minux_display.h"
#include "minux_fs.h"

MinuxShell::MinuxShell() {
  bufferIndex = 0;
  shellActive = false;
  memset(commandBuffer, 0, MAX_CMD_LENGTH);
}

void MinuxShell::init() {
  printPrompt();
}

void MinuxShell::processInput(char c) {
  if (c == '\r' || c == '\n') {
    commandBuffer[bufferIndex] = '\0';
    ui.printChar('\n');
    executeCommand(commandBuffer);
    bufferIndex = 0;
    memset(commandBuffer, 0, MAX_CMD_LENGTH);
    printPrompt();
  } else if (c == '\b' || c == 127) { // Backspace
    if (bufferIndex > 0) {
      bufferIndex--;
      commandBuffer[bufferIndex] = '\0';
      // Simple backspace handling
      ui.printChar('\b');
    }
  } else if (bufferIndex < MAX_CMD_LENGTH - 1) {
    commandBuffer[bufferIndex++] = c;
    ui.printChar(c);
  }
}

void MinuxShell::executeCommand(const char* cmd) {
  if (strlen(cmd) == 0) return;
  
  // Parse command and arguments
  char cmdCopy[MAX_CMD_LENGTH];
  strcpy(cmdCopy, cmd);
  
  char* token = strtok(cmdCopy, " ");
  if (!token) return;
  
  // Built-in commands
  if (strcmp(token, "help") == 0) {
    cmd_help();
  } else if (strcmp(token, "ls") == 0) {
    cmd_ls();
  } else if (strcmp(token, "ps") == 0) {
    cmd_ps();
  } else if (strcmp(token, "clear") == 0) {
    cmd_clear();
  } else if (strcmp(token, "uptime") == 0) {
    cmd_uptime();
  } else if (strcmp(token, "mem") == 0) {
    cmd_mem();
  } else if (strcmp(token, "reboot") == 0) {
    cmd_reboot();
  } else if (strcmp(token, "version") == 0) {
    cmd_version();
  } else if (strcmp(token, "cat") == 0) {
    char* filename = strtok(nullptr, " ");
    if (filename) cmd_cat(filename);
    else ui.println("Usage: cat <filename>");
  } else if (strcmp(token, "echo") == 0) {
    char* text = strtok(nullptr, "");
    if (text) cmd_echo(text);
  } else {
    ui.print("Command not found: ");
    ui.println(cmd);
    ui.println("Type 'help' for available commands");
  }
}

void MinuxShell::printPrompt() {
  ui.print("minux:/ $ ");
}

void MinuxShell::cmd_help() {
  ui.println("Available commands:");
  ui.println("help    - Show this help");
  ui.println("ls      - List files");
  ui.println("ps      - List processes");
  ui.println("clear   - Clear screen");
  ui.println("uptime  - Show uptime");
  ui.println("mem     - Show memory info");
  ui.println("version - Show version");
  ui.println("cat     - Display file");
  ui.println("echo    - Print text");
  ui.println("reboot  - Restart system");
}

void MinuxShell::cmd_ls() {
  ui.println("Name\t\tSize\tType");
  ui.println("------------------------");
  
  for (int i = 0; i < filesystem.getFileCount(); i++) {
    FileEntry* file = filesystem.getFile(i);
    if (file) {
      ui.print(file->name);
      ui.print("\t\t");
      ui.print(file->size);
      ui.print("\t");
      ui.println(file->isDirectory ? "DIR" : "FILE");
    }
  }
}

void MinuxShell::cmd_ps() {
  ui.println("PID\tName\t\tState");
  ui.println("------------------------");
  
  for (int i = 0; i < scheduler.getProcessCount(); i++) {
    ProcessControlBlock* proc = scheduler.getProcess(i);
    if (proc && proc->active) {
      ui.print(i);
      ui.print("\t");
      ui.print(proc->name);
      ui.print("\t\t");
      switch(proc->state) {
        case PROC_READY: ui.println("READY"); break;
        case PROC_RUNNING: ui.println("RUNNING"); break;
        case PROC_BLOCKED: ui.println("BLOCKED"); break;
        case PROC_TERMINATED: ui.println("TERMINATED"); break;
      }
    }
  }
}

void MinuxShell::cmd_clear() {
  ui.clear();
  ui.setCursor(0, 0);
}

void MinuxShell::cmd_uptime() {
  unsigned long uptime = kernel.getUptime();
  ui.print("Uptime: ");
  ui.print(uptime / 1000);
  ui.println(" seconds");
}

void MinuxShell::cmd_mem() {
  MemInfo mem = kernel.getMemoryInfo();
  ui.print("Total: ");
  ui.print(mem.total);
  ui.println(" bytes");
  ui.print("Used: ");
  ui.print(mem.used);
  ui.println(" bytes");
  ui.print("Free: ");
  ui.print(mem.free);
  ui.println(" bytes");
  ui.print("Usage: ");
  ui.print(mem.fragmentation);
  ui.println("%");
}

void MinuxShell::cmd_version() {
  ui.print("Minux RTOS ");
  ui.println(kernel.getVersion());
  ui.print("Build: ");
  ui.print(__DATE__);
  ui.print(" ");
  ui.println(__TIME__);
}

void MinuxShell::cmd_cat(const char* filename) {
  FileEntry* file = filesystem.openFile(filename);
  if (file) {
    for (int i = 0; i < file->size; i++) {
      ui.printChar((char)file->data[i]);
    }
    ui.printChar('\n');
  } else {
    ui.print("File not found: ");
    ui.println(filename);
  }
}

void MinuxShell::cmd_echo(const char* text) {
  ui.println(text);
}

void MinuxShell::cmd_reboot() {
  ui.println("Rebooting...");
  delay(1000);
  // Software reset for Arduino
  asm volatile ("  jmp 0");
}
