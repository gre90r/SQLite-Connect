#include <cstdio>
#include <iostream>
#include <fstream>
#include <thread>

#define println(s) { std::cout << s << std::endl; }
#define printlnError(s) { std::cerr << s << std::endl; }

/**
 * deletes a file
 * @param filename the file to delete
 */
void deleteFile(const char* filename) {
  if (remove(filename) != 0) {
    printlnError("[WARNING] failed to delete file. maybe file does not exist or permission denied.");
  } else {
    println("[INFO] deleted file: " << filename);
  }
}

/**
 * create a file
 * @param filename the filename to create the file with
 */
void createFile(const char* filename) {
  std::ofstream file(filename);
  file.close();
  println("[INFO] created file: " << filename);
}

/**
 * check if a file exists
 * @param filename the file to check its existence
 * @return true: the file exists. false: does not exist
 */
bool fileExists (const char* filename) {
  std::ifstream file(filename);
  return file.good();
}

/**
 * this thread sleeps for a specified amount of time.
 * @param milliseconds the amount of millseconds to sleep
 */
void sleepMilliseconds(const unsigned int& milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}