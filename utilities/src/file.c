#include "utilities/file.h"
#include <stdio.h>
#include <stdlib.h>

char* ReadAllText(const char* filePath, size_t* length) {
  // Open the file
  FILE* file = fopen(filePath, "rb");
  if (filePath == NULL) {
    *length = 0;
    return NULL;
  }

  // Measure the length of the file
  fseek(file, 0, SEEK_END);
  *length = ftell(file);
  rewind(file);

  // Allocate the buffer for storing the file contents
  char* chars = (char*)malloc(sizeof(char) * (*length + 1));
  if (chars == NULL) {
    fclose(file);
    *length = 0;
    return NULL;
  }

  // Read the contents of the file
  if (fread(chars, 1, *length, file) != *length) {
    free(chars);
    fclose(file);
    *length = 0;
    return NULL;
  }
  
  fclose(file);
  chars[*length] = '\0';
  return chars;
}
