#ifndef MINUX_SCHEDULER_H
#define MINUX_SCHEDULER_H

#include <Arduino.h>
#include "minux_kernel.h"

// Process control block
struct ProcessControlBlock {
  char name[8];     // Reduced from 16
  void (*function)();
  unsigned long interval;
  unsigned long lastRun;
  bool active;
  uint8_t priority;
  ProcessState state;
};;

class MinuxScheduler {
private:
  ProcessControlBlock processes[MAX_PROCESSES];
  uint8_t processCount;
  uint8_t currentProcess;
  unsigned long lastSchedule;
  uint16_t scheduleInterval;
  
public:
  MinuxScheduler();
  void init();
  bool startProcess(const char* name, void (*func)(), unsigned long interval, uint8_t priority = 1);
  void stopProcess(const char* name);
  void tick();
  void yield();
  uint8_t getProcessCount() { return processCount; }
  ProcessControlBlock* getProcess(uint8_t index);
  void listProcesses();
  void suspendProcess(const char* name);
  void resumeProcess(const char* name);
};

extern MinuxScheduler scheduler;

#endif
