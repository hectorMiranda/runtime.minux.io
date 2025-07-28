#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "minux_kernel.h"
#include "minux_display.h"
#include "minux_input.h"
#include "minux_scheduler.h"
#include "minux_fs.h"
#include "minux_shell.h"

// Hardware configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define BUZZER_PIN 9
#define BTN_A 2 
#define BTN_UP 10 
#define BTN_DOWN 8
#define BTN_RIGHT 7

// Global system objects
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MinuxKernel kernel;
MinuxDisplay ui(&display);
MinuxInput input;
MinuxScheduler scheduler;
MinuxFS filesystem;
MinuxShell shell;

// Application state
enum AppState {
  STATE_DESKTOP,
  STATE_TERMINAL,
  STATE_SYSINFO,
  STATE_FILES
};

AppState currentState = STATE_DESKTOP;
unsigned long lastInput = 0;
bool terminalMode = false;

// Function declarations
void handleDesktopInput(InputEvent event);
void handleTerminalInput(InputEvent event);
void handleSysInfoInput(InputEvent event);
void handleFilesInput(InputEvent event);
void enterTerminal();
void showSystemInfo();
void showFiles();
void returnToDesktop();

void setup() {
  // Initialize hardware
  Serial.begin(115200);
  
  // Initialize display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  // Initialize system components
  kernel.init();
  ui.init();
  input.init(BTN_A, BTN_UP, BTN_DOWN, BTN_RIGHT, BUZZER_PIN);
  scheduler.init();
  filesystem.init();
  shell.init();
  
  // Boot sequence
  ui.showBootScreen();
  delay(1500);
  
  // Show loading progress
  for(int i = 0; i <= 100; i += 10) {
    ui.showLoadingBar(i);
    delay(100);
  }
  
  delay(500);
  
  // Start system processes
  scheduler.startProcess("idle", idle_task, 100);
  scheduler.startProcess("ui", ui_task, 50);
  scheduler.startProcess("input", input_task, 10);
  scheduler.startProcess("fs", fs_task, 1000);
  
  // Show desktop
  ui.showDesktop();
  ui.updateStatusBar("System Ready");
  
  Serial.println("Minux RTOS Started");
}

void loop() {
  // Handle input events
  InputEvent event = input.poll();
  
  switch(currentState) {
    case STATE_DESKTOP:
      handleDesktopInput(event);
      break;
      
    case STATE_TERMINAL:
      handleTerminalInput(event);
      break;
      
    case STATE_SYSINFO:
      handleSysInfoInput(event);
      break;
      
    case STATE_FILES:
      handleFilesInput(event);
      break;
  }
  
  // Run scheduler
  scheduler.tick();
  
  // Auto-return to desktop after inactivity
  if (currentState != STATE_DESKTOP && millis() - lastInput > 30000) {
    returnToDesktop();
  }
  
  delay(1);
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
      // Show running processes
      ui.showProcessList();
      delay(2000);
      ui.showDesktop();
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
  // In terminal mode, we'd normally handle serial input
  // For now, just handle button to exit
  if (event == EVENT_BTN_A) {
    returnToDesktop();
  }
  
  // Handle serial input if available
  if (Serial.available()) {
    char c = Serial.read();
    shell.processInput(c);
    lastInput = millis();
  }
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
  ui.setTerminalMode();
  ui.updateStatusBar("Terminal Mode");
  shell.activate();
  lastInput = millis();
}

void showSystemInfo() {
  currentState = STATE_SYSINFO;
  ui.showSystemInfo();
  ui.updateStatusBar("System Info");
  lastInput = millis();
}

void showFiles() {
  currentState = STATE_FILES;
  ui.clear();
  ui.setCursor(0, 0);
  
  display.println("=== FILE SYSTEM ===");
  display.println();
  
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
  
  display.println();
  display.println("Press any button to return");
  ui.update();
  ui.updateStatusBar("File Browser");
  lastInput = millis();
}

void returnToDesktop() {
  currentState = STATE_DESKTOP;
  terminalMode = false;
  shell.deactivate();
  ui.showDesktop();
  ui.updateStatusBar("Desktop");
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