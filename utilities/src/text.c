#include "utilities/text.h"
#include <stdlib.h>

void destroyTextContents(TextContents* contents) {
  free(contents->chars);
  contents->chars = NULL;
  contents->length = 0;

  free(contents->lines);
  contents->lines = NULL;
  contents->lineCount = 0;
}

bool isTextContentsInitialized(const TextContents* contents) {
  return contents->chars != NULL && contents->lines != NULL;
}

const char* getLineOfTextContents(const TextContents* contents, size_t lineNumber) {
  if (lineNumber >= contents->lineCount) {
    return NULL;
  }
  return &contents->chars[contents->lines[lineNumber].startIndex];
}
