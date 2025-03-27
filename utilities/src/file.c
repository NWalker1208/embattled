#include "utilities/file.h"
#include <stdio.h>
#include <stdlib.h>

char* readAllText(const char* filePath) {
  // Open the file
  FILE* file = fopen(filePath, "rb");
  if (filePath == NULL) {
    return NULL;
  }

  // Measure the length of the file
  fseek(file, 0, SEEK_END);
  size_t fileLength = ftell(file);
  rewind(file);

  // Allocate the buffer for storing the file contents
  char* buffer = (char*)malloc(sizeof(char) * (fileLength + 1));
  if (buffer == NULL) {
    fclose(file);
    return NULL;
  }

  // Read the contents of the file
  size_t bytesRead = fread(buffer, 1, fileLength, file);
  if (bytesRead != fileLength) {
    free(buffer);
    fclose(file);
    return NULL;
  }
  
  buffer[bytesRead] = '\0';
  fclose(file);

  return buffer;
}
