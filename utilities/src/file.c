#include "utilities/file.h"
#include <stdio.h>
#include <stdlib.h>

TextContents readAllText(const char* filePath) {
  // Open the file
  FILE* file = fopen(filePath, "rb");
  if (filePath == NULL) {
    return (TextContents){ 0 };
  }

  // Measure the length of the file
  fseek(file, 0, SEEK_END);
  size_t length = ftell(file);
  rewind(file);

  // Allocate the buffer for storing the file contents
  char* chars = (char*)malloc(sizeof(char) * (length + 1));
  if (chars == NULL) {
    fclose(file);
    return (TextContents){ 0 };
  }

  // Read the contents of the file
  if (fread(chars, 1, length, file) != length) {
    free(chars);
    fclose(file);
    return (TextContents){ 0 };
  }
  
  chars[length] = '\0';
  fclose(file);
  
  // Split the contents into lines, allowing for both Unix and Windows style line endings
  TextContentsLine* lines = NULL;
  size_t lineCount = 0;
  size_t currentLineStart = 0;

  for (size_t i = 0; i < length; i++) {
    if (chars[i] == '\n') {
      size_t currentLineEnd = (i > 0 && chars[i - 1] == '\r') ? i - 1 : i;
      chars[currentLineEnd] = '\0';

      lines = realloc(lines, sizeof(TextContentsLine) * (lineCount + 1));
      lines[lineCount] = (TextContentsLine){ currentLineStart, currentLineEnd - currentLineStart };
      lineCount++;

      currentLineStart = i + 1;
    }
  }

  lines = realloc(lines, sizeof(TextContentsLine) * (lineCount + 1));
  lines[lineCount] = (TextContentsLine){ currentLineStart, length - currentLineStart };
  lineCount++;

  return (TextContents){ chars, length, lines, lineCount };
}
