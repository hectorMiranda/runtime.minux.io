#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("=== BASIC ARDUINO TEST ===");
  Serial.println("If you see this, Arduino is working");
  
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  static int counter = 0;
  
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("Blink ");
  Serial.println(counter++);
  delay(1000);
  
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
