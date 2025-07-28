#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Hardware configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Global objects
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Function declarations
void showDesktop();
void updateDesktop();
void handleSerialCommand(String cmd);
void showTestPattern();
int freeMemory();

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Minux RTOS Starting...");
  
  // Initialize I2C
  Wire.begin();
  
  // Initialize display with fixed address like the working code
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("ERROR: SSD1306 allocation failed!");
    Serial.println("Check wiring: SDA->A4, SCL->A5, VCC->5V, GND->GND");
    Serial.println("Also try VCC->3.3V if 5V doesn't work");
    while (1) {
      delay(1000);
    }
  }
  
  Serial.println("Display initialized successfully!");
  
  // Clear display and show boot message
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Minux v1.0");
  display.setCursor(0, 10);
  display.println("Booting...");
  display.display();
  
  delay(2000);
  
  // Show desktop
  showDesktop();
  
  Serial.println("Minux RTOS Ready!");
}

void loop() {
  static unsigned long lastUpdate = 0;
  unsigned long currentTime = millis();
  
  // Check for Serial commands
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    handleSerialCommand(command);
  }
  
  // Update display every 1000ms
  if (currentTime - lastUpdate >= 1000) {
    updateDesktop();
    lastUpdate = currentTime;
  }
  
  delay(10);
}

void handleSerialCommand(String cmd) {
  cmd.toLowerCase();
  
  if (cmd == "help") {
    Serial.println("=== Minux RTOS Commands ===");
    Serial.println("help     - Show this help");
    Serial.println("status   - Show system status");
    Serial.println("clear    - Clear display");
    Serial.println("test     - Show test pattern");
    Serial.println("desktop  - Show desktop");
    Serial.println("reboot   - Restart system");
    Serial.println("uptime   - Show uptime");
  } 
  else if (cmd == "status") {
    Serial.println("=== System Status ===");
    Serial.print("Uptime: ");
    Serial.print(millis() / 1000);
    Serial.println(" seconds");
    Serial.print("Free RAM: ");
    Serial.print(freeMemory());
    Serial.println(" bytes");
    Serial.println("Display: OK");
    Serial.println("System: Running");
  }
  else if (cmd == "clear") {
    display.clearDisplay();
    display.display();
    Serial.println("Display cleared");
  }
  else if (cmd == "test") {
    showTestPattern();
    Serial.println("Test pattern displayed");
  }
  else if (cmd == "desktop") {
    showDesktop();
    Serial.println("Desktop shown");
  }
  else if (cmd == "reboot") {
    Serial.println("Rebooting system...");
    delay(1000);
    asm volatile ("jmp 0"); // Reset Arduino
  }
  else if (cmd == "uptime") {
    Serial.print("System uptime: ");
    Serial.print(millis() / 1000);
    Serial.println(" seconds");
  }
  else if (cmd != "") {
    Serial.print("Unknown command: ");
    Serial.println(cmd);
    Serial.println("Type 'help' for available commands");
  }
}

void showTestPattern() {
  display.clearDisplay();
  
  // Draw test patterns
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("TEST PATTERN");
  
  // Lines
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
  display.drawLine(0, 20, 127, 20, SSD1306_WHITE);
  
  // Rectangles
  display.drawRect(10, 25, 30, 15, SSD1306_WHITE);
  display.fillRect(50, 25, 30, 15, SSD1306_WHITE);
  
  // Circles
  display.drawCircle(100, 32, 8, SSD1306_WHITE);
  
  // Text
  display.setCursor(0, 45);
  display.println("All pixels OK!");
  display.setCursor(0, 55);
  display.print("Time: ");
  display.print(millis() / 1000);
  display.println("s");
  
  display.display();
}

int freeMemory() {
  char top;
  extern char *__heap_start, *__brkval;
  return __brkval ? &top - __brkval : &top - __heap_start;
}

void showDesktop() {
  display.clearDisplay();
  
  // Title bar
  display.drawRect(0, 0, SCREEN_WIDTH, 12, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 2);
  display.println("Minux Desktop");
  
  // Draw icons
  display.drawRect(10, 20, 44, 16, SSD1306_WHITE);
  display.setCursor(12, 24);
  display.println("Files");
  
  display.drawRect(70, 20, 44, 16, SSD1306_WHITE);
  display.setCursor(72, 24);
  display.println("Term");
  
  display.drawRect(10, 45, 44, 16, SSD1306_WHITE);
  display.setCursor(12, 49);
  display.println("Info");
  
  display.drawRect(70, 45, 44, 16, SSD1306_WHITE);
  display.setCursor(72, 49);
  display.println("Exit");
  
  // Status bar
  display.setCursor(0, 56);
  display.print("Uptime: ");
  display.print(millis() / 1000);
  display.println("s");
  
  display.display();
}

void updateDesktop() {
  // Update status bar only
  display.fillRect(0, 56, SCREEN_WIDTH, 8, SSD1306_BLACK);
  display.setCursor(0, 56);
  display.setTextColor(SSD1306_WHITE);
  display.print("Uptime: ");
  display.print(millis() / 1000);
  display.println("s");
  display.display();
}
