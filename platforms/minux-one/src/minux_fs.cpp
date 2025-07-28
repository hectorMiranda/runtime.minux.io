#include "minux_fs.h"
#include "minux_scheduler.h"

// External references
extern MinuxScheduler scheduler;

MinuxFS::MinuxFS() {
  fileCount = 0;
  strcpy(currentPath, "/");
}

void MinuxFS::init() {
  createSystemFiles();
}

bool MinuxFS::createFile(const char* name, const uint8_t* data, uint16_t size) {
  if (fileCount >= MAX_FILES || size > MAX_FILESIZE) return false;
  
  FileEntry* file = &files[fileCount];
  strncpy(file->name, name, MAX_FILENAME - 1);
  file->name[MAX_FILENAME - 1] = '\0';
  
  memcpy(file->data, data, size);
  file->size = size;
  file->isDirectory = false;
  file->created = millis();
  file->modified = millis();
  
  fileCount++;
  return true;
}

bool MinuxFS::deleteFile(const char* name) {
  for (int i = 0; i < fileCount; i++) {
    if (strcmp(files[i].name, name) == 0) {
      // Shift remaining files
      for (int j = i; j < fileCount - 1; j++) {
        files[j] = files[j + 1];
      }
      fileCount--;
      return true;
    }
  }
  return false;
}

FileEntry* MinuxFS::openFile(const char* name) {
  for (int i = 0; i < fileCount; i++) {
    if (strcmp(files[i].name, name) == 0) {
      return &files[i];
    }
  }
  return nullptr;
}

bool MinuxFS::writeFile(const char* name, const uint8_t* data, uint16_t size) {
  FileEntry* file = openFile(name);
  if (file && size <= MAX_FILESIZE) {
    memcpy(file->data, data, size);
    file->size = size;
    file->modified = millis();
    return true;
  }
  return false;
}

uint16_t MinuxFS::readFile(const char* name, uint8_t* buffer, uint16_t maxSize) {
  FileEntry* file = openFile(name);
  if (file) {
    uint16_t copySize = min(file->size, maxSize);
    memcpy(buffer, file->data, copySize);
    return copySize;
  }
  return 0;
}

bool MinuxFS::createDir(const char* name) {
  if (fileCount >= MAX_FILES) return false;
  
  FileEntry* dir = &files[fileCount];
  strncpy(dir->name, name, MAX_FILENAME - 1);
  dir->name[MAX_FILENAME - 1] = '\0';
  dir->size = 0;
  dir->isDirectory = true;
  dir->created = millis();
  dir->modified = millis();
  
  fileCount++;
  return true;
}

FileEntry* MinuxFS::getFile(uint8_t index) {
  if (index < fileCount) {
    return &files[index];
  }
  return nullptr;
}

void MinuxFS::createSystemFiles() {
  // Create minimal system files
  const char* version = "Minux v0.1";
  createFile("version", (const uint8_t*)version, strlen(version));
  
  updateSystemInfo();
}

void MinuxFS::updateSystemInfo() {
  char sysinfo[32];
  sprintf(sysinfo, "Up:%lus Mem:%db", millis()/1000, 1024);
  
  // Update or create system info file
  if (openFile("sysinfo")) {
    writeFile("sysinfo", (const uint8_t*)sysinfo, strlen(sysinfo));
  } else {
    createFile("sysinfo", (const uint8_t*)sysinfo, strlen(sysinfo));
  }
}
