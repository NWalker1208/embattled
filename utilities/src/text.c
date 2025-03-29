#include "utilities/text.h"
#include <stdlib.h>
#include <string.h>

TextContents InitTextContents(char* chars, size_t length) {
  // Split the chars into lines, allowing for both Unix and Windows style line endings
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

TextContents InitTextContentsAsCopy(const char* chars, size_t length) {
  char* charsCopy = malloc(sizeof(char) * (length + 1));
  memcpy(charsCopy, chars, sizeof(char) * length);
  charsCopy[length] = '\0';
  return InitTextContents(charsCopy, length);
}

TextContents InitTextContentsAsCopyCStr(const char* str) {
  return InitTextContentsAsCopy(str, strlen(str));
}

void DestroyTextContents(TextContents* text) {
  free(text->chars);
  text->chars = NULL;
  text->length = 0;

  free(text->lines);
  text->lines = NULL;
  text->lineCount = 0;
}

bool IsTextContentsInitialized(const TextContents* text) {
  return text->chars != NULL && text->lines != NULL;
}

TextContentsOffset NormalizeTextContentsOffset(const TextContents* text, TextContentsOffset offset) {
  while (offset.line < text->lineCount && offset.column > text->lines[offset.line].length) {
    offset.column -= text->lines[offset.line].length + 1;
    offset.line++;
  }

  if (offset.line >= text->lineCount) {
    offset.line += offset.column;
    offset.column = 0;
  }
  
  return offset;
}

char GetCharAtNormalizedOffset(const TextContents* text, TextContentsOffset offset) {
  if (offset.line >= text->lineCount) {
    return '\0';
  }

  TextContentsLine line = text->lines[offset.line];
  if (offset.column >= line.length) {
    return (offset.line == text->lineCount - 1) ? '\0' : '\n';
  } else {
    return text->chars[line.startIndex + offset.column];
  }
}

char GetCharAtTextContentsOffset(const TextContents* text, TextContentsOffset offset) {
  return GetCharAtNormalizedOffset(text, NormalizeTextContentsOffset(text, offset));
}

int CompareTextContentsSpans(const TextContents* textA, TextContentsSpan spanA, const TextContents* textB, TextContentsSpan spanB) {
  TextContentsOffset offsetA = NormalizeTextContentsOffset(textA, spanA.start);
  TextContentsOffset offsetB = NormalizeTextContentsOffset(textB, spanB.start);
  
  size_t i;
  for (i = 0; i < spanA.length && i < spanB.length && offsetA.line < textA->lineCount && offsetB.line < textB->lineCount; i++) {
    char charA = GetCharAtNormalizedOffset(textA, offsetA);
    char charB = GetCharAtNormalizedOffset(textB, offsetB);
    if (charA < charB) {
      return -1;
    } else if (charA > charB) {
      return 1;
    }

    offsetA.column++;
    if (offsetA.column > textA->lines[offsetA.line].length) {
      offsetA.column = 0;
      offsetA.line++;
    }

    offsetB.column++;
    if (offsetB.column > textB->lines[offsetB.line].length) {
      offsetB.column = 0;
      offsetB.line++;
    }
  }

  // Reached the end of one or both spans. Compare lengths.
  if (offsetA.line >= textA->lineCount) {
    spanA.length = i;
  }
  if (offsetB.line >= textB->lineCount) {
    spanB.length = i;
  }

  if (spanA.length < spanB.length) {
    return -1;
  } else if (spanA.length > spanB.length) {
    return 1;
  } else {
    return 0;
  }
}
