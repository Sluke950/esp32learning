#include <SPI.h>
#include <SD.h>

#define SD_CS_PIN 5  // Change this to the correct chip select pin for your setup

void setup() {
  Serial.begin(115200);
  
  // Initialize the SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD Card initialization failed!");
    return;
  }
  Serial.println("SD Card initialized.");
/*
  // Example usage of functions
  createFileAndWrite("/test.txt", "Hello, world!");
  appendToFile("/test.txt", "Appended data.");
  readFile("/test.txt");
  deleteFile("/test.txt");
  
  createDirectory("/testDir");
  listFiles("/");
  deleteDirectory("/testDir");
  */
}

void loop() {
  createDirectory("/testDir");
  createDirectory("/testDir");
  createDirectory("/testDir1");
  listFiles("/");
  while(1);
}

// Function to create a file and write data to it
void createFileAndWrite(const char *filename, const char *data) {
  File file = SD.open(filename, FILE_WRITE);
  
  if (file) {
    file.println(data);  // Write data to file
    file.close();  // Close the file after writing
    Serial.println("File created and data written.");
  } else {
    Serial.println("Error opening the file.");
  }
}

// Function to append data to an existing file
void appendToFile(const char *filename, const char *data) {
  File file = SD.open(filename, FILE_APPEND);
  
  if (file) {
    file.println(data);  // Append data to file
    file.close();
    Serial.println("Data appended to file.");
  } else {
    Serial.println("Error opening the file.");
  }
}

// Function to delete a file
void deleteFile(const char *filename) {
  if (SD.remove(filename)) {
    Serial.println("File deleted.");
  } else {
    Serial.println("Error deleting file.");
  }
}

// Function to rename a file
void renameFile(const char *oldName, const char *newName) {
  if (SD.rename(oldName, newName)) {
    Serial.println("File renamed.");
  } else {
    Serial.println("Error renaming file.");
  }
}

// Function to create a directory
void createDirectory(const char *dirName) {
  if (SD.mkdir(dirName)) {
    Serial.println("Directory created.");
  } else {
    Serial.println("Error creating directory.");
  }
}

// Function to delete a directory
void deleteDirectory(const char *dirName) {
  if (SD.rmdir(dirName)) {
    Serial.println("Directory deleted.");
  } else {
    Serial.println("Error deleting directory.");
  }
}

// Function to read the content of a file
void readFile(const char *filename) {
  File file = SD.open(filename);
  
  if (file) {
    Serial.println("Reading file...");
    while (file.available()) {
      Serial.write(file.read());  // Read and output file content
    }
    file.close();
  } else {
    Serial.println("Error opening file.");
  }
}

// Function to list all files in a directory
void listFiles(const char *dirName) {
  Serial.println("Listing files...");
  
  File dir = SD.open(dirName);
  if (dir) {
    while (true) {
      File entry = dir.openNextFile();
      if (!entry) {
        break;
      }
      Serial.print(entry.name());
      if (entry.isDirectory()) {
        Serial.println(" [DIR]");
      } else {
        Serial.println(" [FILE]");
      }
      entry.close();
    }
    dir.close();
  } else {
    Serial.println("Error opening directory.");
  }
}

// Function to check if a file exists
bool fileExists(const char *filename) {
  return SD.exists(filename);
}
