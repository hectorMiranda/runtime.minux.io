#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=== Minux Display Test ===");
  
  // Initialize display exactly like the working code
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    while(1); // Don't proceed, loop forever
  }
  
  Serial.println("Display initialized successfully!");
  
  // Test display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Minux RTOS Test");
  display.setCursor(0, 10);
  display.println("Display Working!");
  display.setCursor(0, 20);
  display.println("Address: 0x3C");
  display.setCursor(0, 30);
  display.println("If you see this,");
  display.setCursor(0, 40);
  display.println("hardware is OK!");
  display.display();
  
  Serial.println("Test pattern displayed.");
}

void loop() {
  // Simple animation to show it's working
  static unsigned long lastUpdate = 0;
  static int counter = 0;
  
  if (millis() - lastUpdate >= 1000) {
    counter++;
    
    // Clear just the counter area
    display.fillRect(0, 50, 128, 14, SSD1306_BLACK);
    display.setCursor(0, 50);
    display.print("Counter: ");
    display.print(counter);
    display.display();
    
    lastUpdate = millis();
    
    Serial.print("Counter: ");
    Serial.println(counter);
  }
  
  delay(10);
}
