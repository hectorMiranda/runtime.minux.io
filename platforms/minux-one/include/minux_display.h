#ifndef MINUX_DISPLAY_H
#define MINUX_DISPLAY_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// Display modes
enum DisplayMode {
  MODE_BOOT,
  MODE_DESKTOP,
  MODE_TERMINAL,
  MODE_MENU,
  MODE_APP
};

// Window types
enum WindowType {
  WIN_DESKTOP,
  WIN_DIALOG,
  WIN_TERMINAL,
  WIN_APP
};

class MinuxDisplay {
private:
  Adafruit_SSD1306* display;
  DisplayMode currentMode;
  uint8_t cursor_x, cursor_y;
  bool inverted;
  char statusBar[17];  // Reduced from 21 (16 chars + null)
  
public:
  MinuxDisplay(Adafruit_SSD1306* disp);
  void init();
  void clear();
  void update();
  
  // Boot sequence
  void showBootScreen();
  void showLoadingBar(uint8_t progress);
  
  // Desktop environment
  void showDesktop();
  void drawTaskbar();
  void drawWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char* title);
  
  // Terminal
  void setTerminalMode();
  void printChar(char c);
  void print(const char* text);
  void print(int value);
  void print(long value);
  void print(unsigned long value);
  void print(uint16_t value);
  void print(uint8_t value);
  void println(const char* text);
  void setCursor(uint8_t x, uint8_t y);
  
  // System info
  void showSystemInfo();
  void showProcessList();
  void updateStatusBar(const char* text);
  
  // Utilities
  void setMode(DisplayMode mode) { currentMode = mode; }
  DisplayMode getMode() { return currentMode; }
  void invertDisplay(bool invert);
  void drawIcon(uint8_t x, uint8_t y, uint8_t icon);
};

// Desktop icons
#define ICON_TERMINAL 0
#define ICON_SETTINGS 1
#define ICON_FILES 2
#define ICON_MONITOR 3

extern MinuxDisplay ui;

#endif
