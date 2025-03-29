#include "utilities/text.h"
#include <stdlib.h>

void DestroyTextContents(TextContents* contents) {
  free(contents->chars);
  contents->chars = NULL;
  contents->length = 0;

  free(contents->lines);
  contents->lines = NULL;
  contents->lineCount = 0;
}

bool IsTextContentsInitialized(const TextContents* contents) {
  return contents->chars != NULL && contents->lines != NULL;
}

const char* GetLineOfTextContents(const TextContents* contents, size_t line) {
  if (line >= contents->lineCount) {
    return NULL;
  }
  return &contents->chars[contents->lines[line].startIndex];
}
