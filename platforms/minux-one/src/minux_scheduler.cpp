#include "minux_scheduler.h"

MinuxScheduler::MinuxScheduler() {
  processCount = 0;
  currentProcess = 0;
  lastSchedule = 0;
  scheduleInterval = 10; // 10ms time slice
}

void MinuxScheduler::init() {
  for(int i = 0; i < MAX_PROCESSES; i++) {
    processes[i].active = false;
    processes[i].state = PROC_TERMINATED;
  }
}

bool MinuxScheduler::startProcess(const char* name, void (*func)(), unsigned long interval, uint8_t priority) {
  if (processCount >= MAX_PROCESSES) return false;
  
  ProcessControlBlock* pcb = &processes[processCount];
  strncpy(pcb->name, name, 7);
  pcb->name[7] = '\0';
  pcb->function = func;
  pcb->interval = interval;
  pcb->lastRun = 0;
  pcb->active = true;
  pcb->priority = priority;
  pcb->state = PROC_READY;
  
  processCount++;
  return true;
}

void MinuxScheduler::stopProcess(const char* name) {
  for(int i = 0; i < processCount; i++) {
    if (strcmp(processes[i].name, name) == 0) {
      processes[i].active = false;
      processes[i].state = PROC_TERMINATED;
      break;
    }
  }
}

void MinuxScheduler::tick() {
  unsigned long now = millis();
  
  // Round-robin scheduling with time-based intervals
  for(int i = 0; i < processCount; i++) {
    ProcessControlBlock* pcb = &processes[i];
    
    if (pcb->active && pcb->state == PROC_READY) {
      if (now - pcb->lastRun >= pcb->interval) {
        pcb->state = PROC_RUNNING;
        pcb->function();
        pcb->lastRun = now;
        pcb->state = PROC_READY;
      }
    }
  }
}

void MinuxScheduler::yield() {
  // Simple yield - just delay a bit
  delay(1);
}

ProcessControlBlock* MinuxScheduler::getProcess(uint8_t index) {
  if (index < processCount) {
    return &processes[index];
  }
  return nullptr;
}

void MinuxScheduler::suspendProcess(const char* name) {
  for(int i = 0; i < processCount; i++) {
    if (strcmp(processes[i].name, name) == 0) {
      processes[i].state = PROC_BLOCKED;
      break;
    }
  }
}

void MinuxScheduler::resumeProcess(const char* name) {
  for(int i = 0; i < processCount; i++) {
    if (strcmp(processes[i].name, name) == 0) {
      processes[i].state = PROC_READY;
      break;
    }
  }
}
