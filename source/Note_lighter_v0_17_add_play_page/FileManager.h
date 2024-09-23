
#include "iferror.h"

#define NB_FILES_MAX 30
#define NB_CHAR_FILENAME_MAX 50

class FileManager {
  char fileNames_[NB_FILES_MAX][NB_CHAR_FILENAME_MAX];
  int nbFiles_ = 0;

private:
  void getRightSubstring_(const char* text, char* result) {

      int len = strlen(text);

      if (len >= 4) {
          // Copy the last 4 characters from the input string
          strncpy(result, text + len - 4, 4);
          result[4] = '\0'; // Null-terminate the result

      } else  // Handle cases where input length is less than 4
          result = ""; // 

  }

public:

  void loadFiles(char * directory) {

    DEBUGVAL("loadFiles directory",directory)
    File root = SD.open(directory);

    nbFiles_ = 0;
    for (int i = 0; i < NB_FILES_MAX; i++) {

      File entry = root.openNextFile();

      if (!entry)
        break;

      char name[NB_CHAR_FILENAME_MAX]="";
      char ext[5] = "";
      strcpy(name, entry.name());
      getRightSubstring_(name, ext);

      if (!entry.isDirectory()) 
        if (strcmp(ext,".mid") == 0 || strcmp(ext,".MID") == 0) {

          strcpy(fileNames_[nbFiles_], name);
          DEBUGVAL("loading",name)

          nbFiles_++;
        }

      entry.close();
    }

    root.close();
  }

  void SerialPrintLoadedFiles() {
    for (int i = 0; i < nbFiles_; i++)
      Serial.println(fileNames_[i]);
  }

  int getNbFiles() {
    return nbFiles_;
  }

  // get the stored filename at index i, zero based
  char* getFileName(int i) {
    IFERROR(i >= nbFiles_, "FileManager.h getFileName(): invalid input index is >= number of files")
    IFERROR(i < 0, "FileManager.h getFileName(): invalid input index is < 0")
    return fileNames_[i];
  }
};