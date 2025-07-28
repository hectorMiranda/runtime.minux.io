#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Hardware configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// Try different I2C addresses
uint8_t addresses[] = {0x3C, 0x3D, 0x78, 0x7A};
const int numAddresses = 4;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== Minux RTOS Display Diagnostic ===");
  Serial.println();
  
  // Initialize I2C
  Wire.begin();
  
  // Scan I2C bus
  Serial.println("Scanning I2C bus...");
  int deviceCount = 0;
  
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
      deviceCount++;
    }
  }
  
  if (deviceCount == 0) {
    Serial.println("No I2C devices found!");
    Serial.println();
    Serial.println("TROUBLESHOOTING:");
    Serial.println("1. Check wiring:");
    Serial.println("   SDA -> A4 (Arduino Nano)");
    Serial.println("   SCL -> A5 (Arduino Nano)");
    Serial.println("   VCC -> 3.3V or 5V");
    Serial.println("   GND -> GND");
    Serial.println("2. Check power supply");
    Serial.println("3. Check display module");
  } else {
    Serial.print("Found ");
    Serial.print(deviceCount);
    Serial.println(" I2C device(s)");
  }
  
  Serial.println();
  Serial.println("Testing SSD1306 initialization...");
  
  // Test each possible address
  for (int i = 0; i < numAddresses; i++) {
    Serial.print("Trying address 0x");
    Serial.print(addresses[i], HEX);
    Serial.print("... ");
    
    Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
    
    if (display.begin(SSD1306_SWITCHCAPVCC, addresses[i])) {
      Serial.println("SUCCESS!");
      
      // Test display functionality
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("Minux RTOS");
      display.println("Display Test");
      display.println();
      display.print("Address: 0x");
      display.println(addresses[i], HEX);
      display.println("Status: OK");
      display.display();
      
      Serial.println("Display test pattern shown!");
      Serial.print("Use address 0x");
      Serial.print(addresses[i], HEX);
      Serial.println(" in your main code.");
      
      return; // Exit after first successful init
    } else {
      Serial.println("FAILED");
    }
  }
  
  Serial.println();
  Serial.println("All addresses failed!");
  Serial.println("Check hardware connections and power.");
}

void loop() {
  // Blink built-in LED to show Arduino is running
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  
  // Print diagnostic info every 10 seconds
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 10000) {
    Serial.println("Arduino running... Check display connections.");
    lastPrint = millis();
  }
}
