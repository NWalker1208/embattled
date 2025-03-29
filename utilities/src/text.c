#include "utilities/text.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

char GetCharAtTextOffset_normalized(const TextContents* text, TextOffset offset) {
  assert(offset.line < text->lineCount);
  assert(offset.column <= text->lines[offset.line].length);

  TextContentsLine line = text->lines[offset.line];
  if (offset.column < line.length) {
    assert(line.startIndex + offset.column < text->length);
    return text->chars[line.startIndex + offset.column];
  } else {
    return (offset.line == text->lineCount - 1) ? '\0' : '\n';
  }
}

char GetCharAtTextOffset(const TextContents* text, TextOffset offset) {
  return GetCharAtTextOffset_normalized(text, NormalizeTextOffset(text, offset));
}

TextOffset NormalizeTextOffset(const TextContents* text, TextOffset offset) {
  while (offset.line < text->lineCount && offset.column > text->lines[offset.line].length) {
    offset.column -= text->lines[offset.line].length + 1;
    offset.line++;
  }
  if (offset.line >= text->lineCount) {
    offset = GetTextContentsEnd(text);
  }
  return offset;
}

int CompareTextOffsets_normalized(TextOffset offsetA, TextOffset offsetB) {
  if (offsetA.line < offsetB.line) {
    return -1;
  } else if (offsetA.line > offsetB.line) {
    return 1;
  } else if (offsetA.column < offsetB.column) {
    return -1;
  } else if (offsetA.column > offsetB.column) {
    return 1;
  } else {
    return 0;
  }
}

TextOffset IncrementTextOffset_normalized(const TextContents* text, TextOffset offset) {
  TextOffset end = GetTextContentsEnd(text);
  if (offset.line == end.line && offset.column == end.column) {
    return offset;
  }
  offset.column++;
  if (offset.column > text->lines[offset.line].length) {
    offset.column = 0;
    offset.line++;
  }
  return offset;
}

TextOffset IncrementTextOffset(const TextContents* text, TextOffset offset) {
  return NormalizeTextOffset(text, (TextOffset){ offset.line, offset.column + 1 });
}

int CompareTextOffsets(const TextContents* text, TextOffset offsetA, TextOffset offsetB) {
  return CompareTextOffsets_normalized(NormalizeTextOffset(text, offsetA), NormalizeTextOffset(text, offsetB));
}

TextSpan NormalizeTextSpan(const TextContents* text, TextSpan span) {
  span.start = NormalizeTextOffset(text, span.start);
  span.end = NormalizeTextOffset(text, span.end);
  if (CompareTextOffsets_normalized(span.start, span.end) > 0) {
    span.end = span.start;
  }
  return span;
}

int CompareTextSpans(const TextContents* textA, TextSpan spanA, const TextContents* textB, TextSpan spanB) {
  spanA = NormalizeTextSpan(textA, spanA);
  spanB = NormalizeTextSpan(textB, spanB);

  TextOffset offsetA = spanA.start, offsetB = spanB.start;
  
  while (CompareTextOffsets_normalized(offsetA, spanA.end) < 0 && CompareTextOffsets_normalized(offsetB, spanB.end) < 0) {
    char charA = GetCharAtTextOffset_normalized(textA, offsetA);
    char charB = GetCharAtTextOffset_normalized(textB, offsetB);
    if (charA < charB) {
      return -1;
    } else if (charA > charB) {
      return 1;
    }

    offsetA = IncrementTextOffset_normalized(textA, offsetA);
    offsetB = IncrementTextOffset_normalized(textB, offsetB);
  }

  // Reached the end of one or both spans. Return result based on lengths.
  int compareEndA = CompareTextOffsets_normalized(offsetA, spanA.end);
  int compareEndB = CompareTextOffsets_normalized(offsetB, spanB.end);
  if (compareEndA >= 0 && compareEndB >= 0) { // Both spans ended at the same time.
    return 0;
  } else if (compareEndA >= 0) { // Span A ended first
    return -1;
  } else { // Span B ended first
    return 1;
  }
}
