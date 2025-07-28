#ifndef MINUX_FS_H
#define MINUX_FS_H

#include <Arduino.h>

// Simple in-memory filesystem
#define MAX_FILES 2     // Further reduced
#define MAX_FILENAME 8  
#define MAX_FILESIZE 32 // Further reduced

struct FileEntry {
  char name[MAX_FILENAME];
  uint8_t data[MAX_FILESIZE];
  uint16_t size;
  bool isDirectory;
  unsigned long created;
  unsigned long modified;
};

class MinuxFS {
private:
  FileEntry files[MAX_FILES];
  uint8_t fileCount;
  char currentPath[64];
  
public:
  MinuxFS();
  void init();
  
  // File operations
  bool createFile(const char* name, const uint8_t* data, uint16_t size);
  bool deleteFile(const char* name);
  FileEntry* openFile(const char* name);
  bool writeFile(const char* name, const uint8_t* data, uint16_t size);
  uint16_t readFile(const char* name, uint8_t* buffer, uint16_t maxSize);
  
  // Directory operations
  bool createDir(const char* name);
  void listFiles();
  uint8_t getFileCount() { return fileCount; }
  FileEntry* getFile(uint8_t index);
  
  // System files
  void createSystemFiles();
  void updateSystemInfo();
};

extern MinuxFS filesystem;

#endif
