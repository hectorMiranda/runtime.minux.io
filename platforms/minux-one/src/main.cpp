#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "minux_config.h"
#include "minux_kernel.h"
#include "minux_display.h"
#include "minux_input.h"
#include "minux_scheduler.h"
#include "minux_fs.h"
#include "minux_shell.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define BUZZER_PIN 9
#define BTN_A 2 
#define BTN_UP 10 
#define BTN_DOWN 8
#define BTN_RIGHT 7

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Comment out all Minux objects - Arduino Nano has insufficient SRAM
// MinuxKernel kernel;
// MinuxDisplay ui(&display);
// MinuxInput input;
// MinuxScheduler scheduler;
// MinuxFS filesystem;
// MinuxShell shell;

enum AppState {
  STATE_DESKTOP,
  STATE_TERMINAL,
  STATE_SYSINFO,
  STATE_FILES
};

AppState currentState = STATE_DESKTOP;

// Function declarations
int getFreeMemory();
void runTasks();
void updateDisplay();
void checkButtons();
void processSerial();
void updateStatus();
void showMainScreen();
void scanI2C();

// Lightweight RTOS variables
unsigned long lastTaskSwitch = 0;
unsigned long taskSwitchInterval = 50; // 50ms task switching
uint8_t currentTask = 0;
uint8_t numTasks = 4;
bool displayWorking = false;

// Memory management utility
int getFreeMemory() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

// Lightweight task scheduler
void runTasks() {
  if (millis() - lastTaskSwitch > taskSwitchInterval) {
    switch(currentTask) {
      case 0:
        updateDisplay();
        break;
      case 1:
        checkButtons();
        break;
      case 2:
        processSerial();
        break;
      case 3:
        updateStatus();
        break;
    }
    currentTask = (currentTask + 1) % numTasks;
    lastTaskSwitch = millis();
  }
}
unsigned long lastInput = 0;
bool terminalMode = false;

void handleDesktopInput(InputEvent event);
void handleTerminalInput(InputEvent event);
void handleSysInfoInput(InputEvent event);
void handleFilesInput(InputEvent event);
void handleGlobalSerialCommand(String cmd);
void enterTerminal();
void showSystemInfo();
void showFiles();
void returnToDesktop();

void idle_task();
void ui_task();
void input_task();
void fs_task();

void setup() {
  // Initialize hardware
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== MINUX LITE RTOS ===");
  Serial.println("Memory-optimized for Arduino Nano");
  Serial.flush();
  
  // Initialize display with more debugging
  Serial.println("Initializing I2C...");
  Wire.begin();
  delay(500);  // Longer delay for I2C stabilization
  
  Serial.println("Scanning I2C bus...");
  bool deviceFound = false;
  for(byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if(Wire.endTransmission() == 0) {
      Serial.print("I2C device found at address 0x");
      if(address < 16) Serial.print("0");
      Serial.println(address, HEX);
      deviceFound = true;
    }
  }
  
  if(!deviceFound) {
    Serial.println("No I2C devices found!");
    Serial.println("Check wiring: SDA->A4, SCL->A5, VCC->5V, GND->GND");
  }
  
  Serial.println("Attempting SSD1306 initialization...");
  bool displayWorking = false;
  if(display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 found at address 0x3C");
    displayWorking = true;
  } else {
    Serial.println("SSD1306 allocation failed - continuing without display");
    Serial.println("System will work via serial commands only");
  }
  
  // Initialize input pins
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  
  Serial.println("Input pins initialized");
  
  // Show boot screen only if display is working
  if(displayWorking) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("MINUX LITE v0.1");
    display.setCursor(0, 10);
    display.println("Low-Memory RTOS");
    display.setCursor(0, 20);
    display.println("Booting...");
    display.display();
    delay(2000);
    
    // Show main screen
    showMainScreen();
  } else {
    Serial.println("Display not available - using serial mode");
  }
  
  Serial.println("=== MINUX LITE READY ===");
  Serial.println("Commands: help, mem, tasks, clear, i2c");
  Serial.print("Free Memory: ");
  Serial.print(getFreeMemory());
  Serial.println(" bytes");
  Serial.println("Display Status: " + String(displayWorking ? "Working" : "Failed"));
}

void loop() {
  // Run lightweight task scheduler
  runTasks();
  
  // Small delay to prevent overwhelming the system
  delay(1);
}

// Lightweight task implementations
void updateDisplay() {
  if(!displayWorking) return;
  
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 1000) { // Update every second
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(100, 0);
    display.print(millis()/1000);
    display.display();
    lastUpdate = millis();
  }
}

void checkButtons() {
  static bool lastBtnA = HIGH;
  bool btnA = digitalRead(BTN_A);
  
  if (lastBtnA == HIGH && btnA == LOW) {
    // Button A pressed - show menu
    if(displayWorking) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("=== MENU ===");
      display.setCursor(0, 15);
      display.println("1. System Info");
      display.setCursor(0, 25);
      display.println("2. Memory");
      display.setCursor(0, 35);
      display.println("3. Tasks");
      display.setCursor(0, 45);
      display.println("4. Back");
      display.display();
      
      Serial.println("Menu displayed");
      delay(2000); // Show menu for 2 seconds
      
      // Return to main screen
      showMainScreen();
    } else {
      Serial.println("=== BUTTON MENU ===");
      Serial.println("Display not available - use serial commands:");
      Serial.println("mem, tasks, i2c, help");
    }
  }
  lastBtnA = btnA;
}

void processSerial() {
  static String inputBuffer = "";
  
  while (Serial.available()) {
    char c = Serial.read();
    
    if (c == '\n' || c == '\r') {
      // Process complete command
      inputBuffer.trim();
      
      if (inputBuffer.length() > 0) {
        Serial.print("Command received: ");
        Serial.println(inputBuffer);
        
        if (inputBuffer == "help") {
          Serial.println("=== MINUX LITE COMMANDS ===");
          Serial.println("help - Show this help");
          Serial.println("mem - Show memory info");
          Serial.println("tasks - Show task info");
          Serial.println("clear - Clear screen");
          Serial.println("i2c - Scan I2C bus");
          Serial.println("reboot - Restart system");
        }
        else if (inputBuffer == "mem") {
          Serial.print("Free Memory: ");
          Serial.print(getFreeMemory());
          Serial.println(" bytes");
          Serial.println("Total SRAM: 2048 bytes");
          Serial.print("Used: ");
          Serial.print(2048 - getFreeMemory());
          Serial.println(" bytes");
          Serial.print("Usage: ");
          Serial.print(((2048 - getFreeMemory()) * 100) / 2048);
          Serial.println("%");
        }
        else if (inputBuffer == "tasks") {
          Serial.println("=== TASK INFO ===");
          Serial.println("Task 0: Display Update");
          Serial.println("Task 1: Button Handler");
          Serial.println("Task 2: Serial Handler");
          Serial.println("Task 3: Status Update");
          Serial.print("Current Task: ");
          Serial.println(currentTask);
          Serial.print("Task Switch Interval: ");
          Serial.print(taskSwitchInterval);
          Serial.println("ms");
        }
        else if (inputBuffer == "clear") {
          if(displayWorking) {
            showMainScreen();
            Serial.println("Screen cleared");
          } else {
            Serial.println("Display not available");
          }
        }
        else if (inputBuffer == "i2c") {
          scanI2C();
        }
        else if (inputBuffer == "reboot") {
          Serial.println("Rebooting...");
          delay(1000);
          asm volatile ("  jmp 0");
        }
        else {
          Serial.print("Unknown command: '");
          Serial.print(inputBuffer);
          Serial.println("'");
          Serial.println("Type 'help' for commands");
        }
      }
      
      inputBuffer = "";
    }
    else if (c >= 32 && c <= 126) {
      // Printable character
      inputBuffer += c;
      Serial.print(c); // Echo character
    }
    else if (c == 8 || c == 127) {
      // Backspace
      if (inputBuffer.length() > 0) {
        inputBuffer.remove(inputBuffer.length() - 1);
        Serial.print("\b \b");
      }
    }
  }
}

void updateStatus() {
  // Update status info periodically
  static unsigned long lastStatusUpdate = 0;
  if (millis() - lastStatusUpdate > 5000) { // Every 5 seconds
    Serial.print("System Status - Uptime: ");
    Serial.print(millis()/1000);
    Serial.print("s, Free RAM: ");
    Serial.print(getFreeMemory());
    Serial.println(" bytes");
    lastStatusUpdate = millis();
  }
}

void showMainScreen() {
  if(!displayWorking) return;
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("MINUX LITE v0.1");
  display.setCursor(0, 10);
  display.println("Low-Memory RTOS");
  display.setCursor(0, 20);
  display.println("----------------");
  display.setCursor(0, 30);
  display.println("BTN_A: Menu");
  display.setCursor(0, 40);
  display.println("Serial: Commands");
  display.setCursor(0, 50);
  display.print("RAM: ");
  display.print(getFreeMemory());
  display.println(" bytes");
  display.display();
}

void scanI2C() {
  Serial.println("=== I2C SCANNER ===");
  Serial.println("Scanning...");
  
  int nDevices = 0;
  for(byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
      nDevices++;
    }
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  
  if (nDevices == 0) {
    Serial.println("No I2C devices found");
    Serial.println("Check connections:");
    Serial.println("SDA -> A4 (Pin 18)");
    Serial.println("SCL -> A5 (Pin 19)");
    Serial.println("VCC -> 5V");
    Serial.println("GND -> GND");
  } else {
    Serial.print("Found ");
    Serial.print(nDevices);
    Serial.println(" device(s)");
  }
  Serial.println("Done");
}

void handleDesktopInput(InputEvent event) {
  switch(event) {
    case EVENT_BTN_A:
      enterTerminal();
      break;
      
    case EVENT_BTN_UP:
      showSystemInfo();
      break;
      
    case EVENT_BTN_DOWN:
      showFiles();
      break;
      
    case EVENT_BTN_RIGHT:
      // Show simple process info on display
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("=== PROCESSES ===");
      display.setCursor(0, 15);
      display.print("Active: ");
      display.println(scheduler.getProcessCount());
      display.setCursor(0, 35);
      display.println("Showing for 2 seconds...");
      display.display();
      delay(2000);
      returnToDesktop();
      break;
      
    case EVENT_NONE:
    case EVENT_BTN_COMBO:
      // Do nothing for these events
      break;
  }
  
  if (event != EVENT_NONE) {
    lastInput = millis();
  }
}

void handleTerminalInput(InputEvent event) {
  // In terminal mode, handle button to exit
  if (event == EVENT_BTN_A) {
    returnToDesktop();
  }
  
  // Note: Serial input is handled globally now
  // Character-by-character shell input would need special handling
}

void handleSysInfoInput(InputEvent event) {
  if (event != EVENT_NONE) {
    returnToDesktop();
  }
}

void handleFilesInput(InputEvent event) {
  if (event != EVENT_NONE) {
    returnToDesktop();
  }
}

void enterTerminal() {
  currentState = STATE_TERMINAL;
  terminalMode = true;
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("=== TERMINAL MODE ===");
  display.setCursor(0, 15);
  display.println("Use serial monitor");
  display.setCursor(0, 25);
  display.println("for commands");
  display.setCursor(0, 45);
  display.println("Button A: Exit");
  display.display();
  
  shell.activate();
  lastInput = millis();
}

void showSystemInfo() {
  currentState = STATE_SYSINFO;
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("=== SYSTEM INFO ===");
  
  MemInfo mem = kernel.getMemoryInfo();
  display.setCursor(0, 15);
  display.print("Uptime: ");
  display.print(kernel.getUptime() / 1000);
  display.println("s");
  
  display.setCursor(0, 25);
  display.print("Free RAM: ");
  display.print(mem.free);
  display.println("b");
  
  display.setCursor(0, 35);
  display.print("Processes: ");
  display.println(scheduler.getProcessCount());
  
  display.setCursor(0, 45);
  display.print("Files: ");
  display.println(filesystem.getFileCount());
  
  display.setCursor(0, 55);
  display.println("Press any button to return");
  display.display();
  lastInput = millis();
}

void showFiles() {
  currentState = STATE_FILES;
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("=== FILE SYSTEM ===");
  display.setCursor(0, 15);
  
  for (int i = 0; i < filesystem.getFileCount(); i++) {
    FileEntry* file = filesystem.getFile(i);
    if (file) {
      if (file->isDirectory) {
        display.print("[DIR]  ");
      } else {
        display.print("[FILE] ");
      }
      display.print(file->name);
      display.print(" (");
      display.print(file->size);
      display.println("b)");
    }
  }
  
  display.setCursor(0, 55);
  display.println("Press any button to return");
  display.display();
  lastInput = millis();
}

void returnToDesktop() {
  currentState = STATE_DESKTOP;
  terminalMode = false;
  shell.deactivate();
  
  // Show simple desktop using direct display calls
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("=== MINUX DESKTOP ===");
  display.setCursor(0, 15);
  display.println("BTN_A: Terminal");
  display.setCursor(0, 25);
  display.println("BTN_UP: System Info");
  display.setCursor(0, 35);
  display.println("BTN_DOWN: Files");
  display.setCursor(0, 45);
  display.println("BTN_RIGHT: Processes");
  display.setCursor(0, 55);
  display.println("Serial: help");
  display.display();
}

void handleGlobalSerialCommand(String cmd) {
  cmd.toLowerCase();
  
  if (cmd == "help") {
    Serial.println("=== Minux RTOS Commands ===");
    Serial.println("help      - Show this help");
    Serial.println("status    - Show system status");
    Serial.println("ps        - Show running processes");
    Serial.println("mem       - Show memory information");
    Serial.println("uptime    - Show system uptime");
    Serial.println("desktop   - Show desktop");
    Serial.println("terminal  - Enter terminal mode");
    Serial.println("sysinfo   - Show system info screen");
    Serial.println("files     - Show file browser");
    Serial.println("reboot    - Restart system");
    Serial.println("version   - Show version info");
    Serial.println("clear     - Clear display");
  } 
  else if (cmd == "status") {
    MemInfo mem = kernel.getMemoryInfo();
    Serial.println("=== System Status ===");
    Serial.print("Version: Minux RTOS v");
    Serial.println(MINUX_VERSION_STRING);
    Serial.print("Uptime: ");
    Serial.print(kernel.getUptime() / 1000);
    Serial.println(" seconds");
    Serial.print("Free RAM: ");
    Serial.print(mem.free);
    Serial.print("/");
    Serial.print(mem.total);
    Serial.println(" bytes");
    Serial.print("Memory fragmentation: ");
    Serial.print(mem.fragmentation);
    Serial.println("%");
    Serial.print("Current state: ");
    switch(currentState) {
      case STATE_DESKTOP: Serial.println("Desktop"); break;
      case STATE_TERMINAL: Serial.println("Terminal"); break;
      case STATE_SYSINFO: Serial.println("System Info"); break;
      case STATE_FILES: Serial.println("File Browser"); break;
    }
  }
  else if (cmd == "ps") {
    Serial.println("=== Running Processes ===");
    Serial.println("PID\tName\t\tState\tPriority");
    Serial.println("------------------------------------");
    for (int i = 0; i < MAX_PROCESSES; i++) {
      ProcessControlBlock* pcb = scheduler.getProcess(i);
      if (pcb && pcb->active) {
        Serial.print(i);
        Serial.print("\t");
        Serial.print(pcb->name);
        Serial.print("\t\t");
        switch (pcb->state) {
          case PROC_READY: Serial.print("READY"); break;
          case PROC_RUNNING: Serial.print("RUNNING"); break;
          case PROC_BLOCKED: Serial.print("BLOCKED"); break;
          case PROC_TERMINATED: Serial.print("TERMINATED"); break;
        }
        Serial.print("\t");
        Serial.println(pcb->priority);
      }
    }
  }
  else if (cmd == "mem") {
    MemInfo mem = kernel.getMemoryInfo();
    Serial.println("=== Memory Information ===");
    Serial.print("Total: ");
    Serial.print(mem.total);
    Serial.println(" bytes");
    Serial.print("Used: ");
    Serial.print(mem.used);
    Serial.println(" bytes");
    Serial.print("Free: ");
    Serial.print(mem.free);
    Serial.println(" bytes");
    Serial.print("Fragmentation: ");
    Serial.print(mem.fragmentation);
    Serial.println("%");
  }
  else if (cmd == "uptime") {
    Serial.print("System uptime: ");
    Serial.print(kernel.getUptime() / 1000);
    Serial.println(" seconds");
  }
  else if (cmd == "desktop") {
    returnToDesktop();
    Serial.println("Switched to desktop");
  }
  else if (cmd == "terminal") {
    enterTerminal();
    Serial.println("Switched to terminal mode");
  }
  else if (cmd == "sysinfo") {
    showSystemInfo();
    Serial.println("Showing system info");
  }
  else if (cmd == "files") {
    showFiles();
    Serial.println("Showing file browser");
  }
  else if (cmd == "clear") {
    display.clearDisplay();
    display.display();
    Serial.println("Display cleared");
  }
  else if (cmd == "reboot") {
    Serial.println("Rebooting system...");
    delay(1000);
    asm volatile ("jmp 0"); // Reset Arduino
  }
  else if (cmd == "version") {
    Serial.println("=== Version Information ===");
    Serial.print("Minux RTOS v");
    Serial.println(MINUX_VERSION_STRING);
    Serial.println("Built for Arduino Nano");
    Serial.print("Compiled: ");
    Serial.print(__DATE__);
    Serial.print(" ");
    Serial.println(__TIME__);
  }
  else if (cmd != "") {
    Serial.print("Unknown command: '");
    Serial.print(cmd);
    Serial.println("'");
    Serial.println("Type 'help' for available commands");
  }
}

// System tasks
void idle_task() {
  // Update system status
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 5000) {
    filesystem.updateSystemInfo();
    lastUpdate = millis();
  }
  delay(50);
}

void ui_task() {
  // Periodic UI updates
  static unsigned long lastBlink = 0;
  
  // Blink cursor in terminal mode
  if (terminalMode && millis() - lastBlink > 500) {
    // Simple cursor blink logic could go here
    lastBlink = millis();
  }
  
  delay(100);
}

void input_task() {
  // Input handling is done in main loop
  delay(10);
}

void fs_task() {
  // Periodic filesystem maintenance
  filesystem.updateSystemInfo();
  delay(1000);
}