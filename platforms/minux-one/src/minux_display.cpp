#include "minux_display.h"
#include "minux_kernel.h"
#include "minux_scheduler.h"

// External references
extern MinuxKernel kernel;
extern MinuxScheduler scheduler;

// Minimal bitmap icons (8x8)
const uint8_t icons[][8] PROGMEM = {
  // Terminal icon
  {0x7E, 0x42, 0x42, 0x5A, 0x42, 0x42, 0x42, 0x7E},
  // Settings icon  
  {0x18, 0x24, 0x7E, 0xFF, 0xFF, 0x7E, 0x24, 0x18},
  // Files icon
  {0x1E, 0x21, 0x21, 0x21, 0x7F, 0x41, 0x41, 0x7F},
  // Monitor icon
  {0xFF, 0x81, 0x81, 0x81, 0x81, 0xFF, 0x18, 0x3C}
};

MinuxDisplay::MinuxDisplay(Adafruit_SSD1306* disp) {
  display = disp;
  currentMode = MODE_BOOT;
  cursor_x = 0;
  cursor_y = 0;
  inverted = false;
  strcpy(statusBar, "Minux RTOS v0.1");
}

void MinuxDisplay::init() {
  display->clearDisplay();
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);
  display->setCursor(0, 0);
}

void MinuxDisplay::clear() {
  display->clearDisplay();
}

void MinuxDisplay::update() {
  display->display();
}

void MinuxDisplay::showBootScreen() {
  clear();
  display->setTextSize(1);
  display->setCursor(30, 10);
  display->println("MINUX RTOS");
  display->setCursor(35, 25);
  display->println("v0.1.0");
  display->setCursor(15, 40);
  display->println("Initializing...");
  update();
}

void MinuxDisplay::showLoadingBar(uint8_t progress) {
  // Draw loading bar at bottom
  display->drawRect(10, 55, 108, 8, SSD1306_WHITE);
  uint8_t barWidth = (progress * 106) / 100;
  display->fillRect(11, 56, barWidth, 6, SSD1306_WHITE);
  update();
}

void MinuxDisplay::showDesktop() {
  clear();
  currentMode = MODE_DESKTOP;
  
  // Desktop background pattern
  for(int i = 0; i < 128; i += 16) {
    for(int j = 0; j < 64; j += 16) {
      display->drawPixel(i, j, SSD1306_WHITE);
    }
  }
  
  // Draw icons
  drawIcon(10, 10, ICON_TERMINAL);
  drawIcon(30, 10, ICON_SETTINGS);
  drawIcon(50, 10, ICON_FILES);
  drawIcon(70, 10, ICON_MONITOR);
  
  // Icon labels
  display->setTextSize(1);
  display->setCursor(8, 20);
  display->print("T");
  display->setCursor(28, 20);
  display->print("S");
  display->setCursor(48, 20);
  display->print("F");
  display->setCursor(68, 20);
  display->print("M");
  
  drawTaskbar();
  update();
}

void MinuxDisplay::drawTaskbar() {
  // Bottom taskbar
  display->fillRect(0, 56, 128, 8, SSD1306_WHITE);
  display->setTextColor(SSD1306_BLACK);
  display->setCursor(2, 57);
  display->print(statusBar);
  display->setTextColor(SSD1306_WHITE);
}

void MinuxDisplay::drawWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char* title) {
  // Window frame
  display->drawRect(x, y, w, h, SSD1306_WHITE);
  // Title bar
  display->fillRect(x, y, w, 10, SSD1306_WHITE);
  display->setTextColor(SSD1306_BLACK);
  display->setCursor(x + 2, y + 2);
  display->print(title);
  display->setTextColor(SSD1306_WHITE);
}

void MinuxDisplay::setTerminalMode() {
  clear();
  currentMode = MODE_TERMINAL;
  cursor_x = 0;
  cursor_y = 0;
  display->setCursor(0, 0);
  display->print("minux:/ $ ");
  update();
}

void MinuxDisplay::printChar(char c) {
  if (c == '\n') {
    cursor_y += 8;
    cursor_x = 0;
    if (cursor_y >= 56) {
      // Scroll up
      cursor_y = 48;
      // Simple scroll simulation
      clear();
      cursor_y = 0;
    }
  } else {
    display->setCursor(cursor_x, cursor_y);
    display->print(c);
    cursor_x += 6;
    if (cursor_x >= 122) {
      cursor_x = 0;
      cursor_y += 8;
    }
  }
  update();
}

void MinuxDisplay::print(const char* text) {
  display->setCursor(cursor_x, cursor_y);
  display->print(text);
  cursor_x += strlen(text) * 6;
  if (cursor_x >= 122) {
    cursor_x = 0;
    cursor_y += 8;
  }
  update();
}

void MinuxDisplay::print(int value) {
  display->setCursor(cursor_x, cursor_y);
  display->print(value);
  cursor_x += 6 * 5; // Approximate width
  if (cursor_x >= 122) {
    cursor_x = 0;
    cursor_y += 8;
  }
  update();
}

void MinuxDisplay::print(long value) {
  display->setCursor(cursor_x, cursor_y);
  display->print(value);
  cursor_x += 6 * 8; // Approximate width
  if (cursor_x >= 122) {
    cursor_x = 0;
    cursor_y += 8;
  }
  update();
}

void MinuxDisplay::print(unsigned long value) {
  display->setCursor(cursor_x, cursor_y);
  display->print(value);
  cursor_x += 6 * 8; // Approximate width
  if (cursor_x >= 122) {
    cursor_x = 0;
    cursor_y += 8;
  }
  update();
}

void MinuxDisplay::print(uint16_t value) {
  display->setCursor(cursor_x, cursor_y);
  display->print(value);
  cursor_x += 6 * 5; // Approximate width
  if (cursor_x >= 122) {
    cursor_x = 0;
    cursor_y += 8;
  }
  update();
}

void MinuxDisplay::print(uint8_t value) {
  display->setCursor(cursor_x, cursor_y);
  display->print(value);
  cursor_x += 6 * 3; // Approximate width
  if (cursor_x >= 122) {
    cursor_x = 0;
    cursor_y += 8;
  }
  update();
}

void MinuxDisplay::println(const char* text) {
  display->setCursor(cursor_x, cursor_y);
  display->println(text);
  cursor_y += 8;
  if (cursor_y >= 56) cursor_y = 0;
  update();
}

void MinuxDisplay::setCursor(uint8_t x, uint8_t y) {
  cursor_x = x;
  cursor_y = y;
  display->setCursor(x, y);
}

void MinuxDisplay::showSystemInfo() {
  clear();
  display->setCursor(0, 0);
  display->println("=== SYSTEM INFO ===");
  display->print("Kernel: ");
  display->println(kernel.getVersion());
  display->print("Uptime: ");
  display->print(kernel.getUptime() / 1000);
  display->println("s");
  
  MemInfo mem = kernel.getMemoryInfo();
  display->print("Memory: ");
  display->print(mem.used);
  display->print("/");
  display->print(mem.total);
  display->println(" bytes");
  
  display->print("Free: ");
  display->print(mem.free);
  display->println(" bytes");
  
  update();
}

void MinuxDisplay::showProcessList() {
  clear();
  display->setCursor(0, 0);
  display->println("=== PROCESSES ===");
  display->println("PID Name     State");
  display->println("-------------------");
  
  for (int i = 0; i < scheduler.getProcessCount(); i++) {
    ProcessControlBlock* proc = scheduler.getProcess(i);
    if (proc && proc->active) {
      display->print(i);
      display->print("   ");
      display->print(proc->name);
      display->print("  ");
      switch(proc->state) {
        case PROC_READY: display->println("RDY"); break;
        case PROC_RUNNING: display->println("RUN"); break;
        case PROC_BLOCKED: display->println("BLK"); break;
        case PROC_TERMINATED: display->println("END"); break;
      }
    }
  }
  
  update();
}

void MinuxDisplay::updateStatusBar(const char* text) {
  strncpy(statusBar, text, 16);
  statusBar[16] = '\0';
}

void MinuxDisplay::drawIcon(uint8_t x, uint8_t y, uint8_t icon) {
  if (icon < 4) {
    display->drawBitmap(x, y, icons[icon], 8, 8, SSD1306_WHITE);
  }
}
