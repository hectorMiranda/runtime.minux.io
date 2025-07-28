#include "minux_kernel.h"
#include "minux_input.h"
#include "minux_scheduler.h"

// External references
extern MinuxInput input;
extern MinuxScheduler scheduler;

MinuxKernel::MinuxKernel() {
  currentState = SYS_BOOT;
  bootTime = 0;
  uptime = 0;
}

void MinuxKernel::init() {
  bootTime = millis();
  currentState = SYS_RUNNING;
  updateMemoryInfo();
}

void MinuxKernel::panic(const char* message) {
  currentState = SYS_ERROR;
  // Display panic message and halt
  Serial.print("KERNEL PANIC: ");
  Serial.println(message);
  while(1) {
    // Blink LED or make sound to indicate panic
    delay(500);
  }
}

unsigned long MinuxKernel::getUptime() {
  return millis() - bootTime;
}

MemInfo MinuxKernel::getMemoryInfo() {
  updateMemoryInfo();
  return memory;
}

void MinuxKernel::updateMemoryInfo() {
  // Simplified memory calculation for Arduino
  extern int __heap_start, *__brkval;
  int v;
  memory.total = 2048; // Arduino Nano has 2KB SRAM
  memory.used = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  memory.free = memory.total - memory.used;
  memory.fragmentation = (memory.used * 100) / memory.total;
}
