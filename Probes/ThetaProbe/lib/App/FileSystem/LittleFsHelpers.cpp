#include "CommandLine/ComLineConfig.h"
#include "CommandLine/CommandLine.h"
#include "LittleFsHelpers.h"
#include "Sensors/Measurement.h"
#include <Config.h>

namespace nvm {

void LittleFsHelpers::init(void) { new (&instance()) LittleFsHelpers(); }

LittleFsHelpers &LittleFsHelpers::instance(void) {
  static LittleFsHelpers littleFsHelpers;
  return littleFsHelpers;
}

LittleFsHelpers::LittleFsHelpers() : _LittleFS{LittleFS} {}

void LittleFsHelpers::initHardware() {
  if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
    Serial.println("LittleFS Mount Failed");
  }
}

void LittleFsHelpers::readConfigFile(const char *filename) {
  // Read file line by line
  File file = LittleFS.open(filename);
  if (!file || file.isDirectory()) {
    Serial.println("Failed to open idTableFile.");
    return;
  }
  while (file.available()) {
    String line = file.readStringUntil('\n');

    for (uint_fast8_t i = 0; i < line.length(); i++) {
      uint8_t chr = (uint8_t)line.c_str()[i];
      xQueueSendToBack(keyBufferQueue, &chr, 50);
    }
    if (line.c_str()[line.length() - 1] != cLine::_KEY_ENTER) {
      uint8_t chr = cLine::_KEY_ENTER;
      xQueueSendToBack(keyBufferQueue, &chr, 50);
    }

    delay(20); // CommandLine-task must process the buffer
  }
  file.close();
}

bool LittleFsHelpers::saveSensIdTable(
    msmnt::MeasurementPivot *measurementPivot) {

  File file = LittleFS.open(idTableFile, FILE_APPEND);
  if (!file || file.isDirectory()) {
    Serial.println("Failed to open idTableFile.");
    return false;
  }

  measurementPivot->ResetIter();
  msmnt::Measurement *actMsmnt = measurementPivot->GetNextMeasurement();
  while (actMsmnt != nullptr) {
    if (actMsmnt->configChanged) {

      char msgBuff[cLine::COMMAND_BUFFER_LEN];
      actMsmnt->GetConfigAsCmd(msgBuff);

      String message = String(msgBuff);

      if (file.print(message)) {
        actMsmnt->configChanged = false;
      } else {
        Serial.println("- append failed");
        return false;
      }
    }
    actMsmnt = measurementPivot->GetNextMeasurement();
  }
  file.close();
  return true;
}

void LittleFsHelpers::listDir(const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = _LittleFS.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void LittleFsHelpers::createDir(const char *path) {
  Serial.printf("Creating Dir: %s\n", path);
  if (!_LittleFS.mkdir(path)) {   
    Serial.println("mkdir failed");
  }
}

void LittleFsHelpers::removeDir(const char *path) {
  Serial.printf("Removing Dir: %s\n", path);
  if (!_LittleFS.rmdir(path)) {    
    Serial.println("rmdir failed");
  }
}

void LittleFsHelpers::readFile(const char *path) {
  File file = _LittleFS.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return;
  }

  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void LittleFsHelpers::writeFile(const char *path, const char *message) {
  File file = _LittleFS.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (!file.print(message)) {   
    Serial.println("- write failed");
  }
  file.close();
}

void LittleFsHelpers::appendFile(const char *path, const char *message) {
  File file = _LittleFS.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("- failed to open file for appending");
    return;
  }
  if (!file.print(message)) {   
    Serial.println("- append failed");
  }
  file.close();
}

void LittleFsHelpers::renameFile(const char *path1, const char *path2) {
  Serial.printf("Renaming file %s to %s\r\n", path1, path2);
  if (!_LittleFS.rename(path1, path2)) {  
    Serial.println("- rename failed");
  }
}

void LittleFsHelpers::deleteFile(const char *path) {
  Serial.printf("Deleting file: %s\r\n", path);
  if (!_LittleFS.remove(path)) {   
    Serial.println("- delete failed");
  }
}
} // namespace nvm