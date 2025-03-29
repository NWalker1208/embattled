#pragma once
#include <stdint.h>
#include <stdbool.h>

// A line of a text contents struct.
typedef struct {
  // The index in the chars array at which the line begins.
  size_t startIndex;
  // The number of characters in the line, excluding the null terminator/line ending.
  size_t length;
} TextContentsLine;

// A collection of lines of text.
typedef struct {
  // The text as an array of chars with null characters between each line.
  char* chars;
  // The length of the chars array.
  size_t length;

  // The array of starting indexes and lengths of each line of text.
  TextContentsLine* lines;
  // The number of lines of text.
  unsigned int lineCount;
} TextContents;

// The offset of a character within some text.
typedef struct {
  // The line number of the offset.
  // If greater than or equal to the number of lines, the offset is equivalent to (lineCount - 1, len(lines[lineCount - 1])).
  size_t line;
  // The column number of the offset.
  // If equal to the length of the line, the offset represents the end of the line.
  // If greater than the length of the line, the offset is equivalent to (line + 1, column - len(lines[line]) - 1).
  size_t column;
} TextOffset;

// A span of characters within some text.
typedef struct {
  // The offset of the first character in the span.
  TextOffset start;
  // The offset of the first character after the span.
  // If equal to or less than start, the span is empty.
  // If greater than the last character of the text, the span extends to the end of the text.
  TextOffset end;
} TextSpan;

// Initializes a TextContents struct with the given chars array.
TextContents InitTextContents(char* chars, size_t length);

// Initializes a TextContents struct with a copy of the given chars array.
TextContents InitTextContentsAsCopy(const char* chars, size_t length);

// Initializes a TextContents struct with a copy of the given null-terminated C string.
TextContents InitTextContentsAsCopyCStr(const char* str);

// Frees the memory referenced by a TextContents struct.
void DestroyTextContents(TextContents* text);

// Checks if the TextContents struct has been initialized.
bool IsTextContentsInitialized(const TextContents* text);

// Gets the final offset for the given text contents, which is defined as (lineCount - 1, len(lines[lineCount - 1])).
static inline TextOffset GetTextContentsEnd(const TextContents* text) {
  return (TextOffset){ text->lineCount - 1, text->lines[text->lineCount - 1].length };
}

// Gets the final offset for the specified line of the given text contents.
// If line is greater than or equal to lineCount, returns the result of GetTextContentsEnd.
static inline TextOffset GetTextContentsEndOfLine(const TextContents* text, size_t line) {
  if (line >= text->lineCount) {
    return GetTextContentsEnd(text);
  } else {
    return (TextOffset){ line, text->lines[line].length };
  }
}

// Gets the character at the specified offset into the text.
// If the offset lies at or beyond the end of the last line, returns '\0'.
// If the offset lies at the end of any other line, returns '\n'.
char GetCharAtTextOffset(const TextContents* text, TextOffset offset);

// Normalizes an offset relative to the text.
// If column is greater than the length of the line, wraps the offset onto the next line.
// If after wrapping line is greater than or equal to lineCount, returns the result of GetTextContentsEnd(text).
TextOffset NormalizeTextOffset(const TextContents* text, TextOffset offset);

// Increments and normalizes an offset relative to the text.
TextOffset IncrementTextOffset(const TextContents* text, TextOffset offset);

// Compares the positions of two text offsets.
// If offsetA appears before offsetB, returns a negative value.
// If offsetA appears after offsetB, returns a positive value.
// If offsetA is equivalent to offsetB, returns zero.
int CompareTextOffsets(const TextContents* text, TextOffset offsetA, TextOffset offsetB);

// Normalizes a span.
// Normalizes both start and end offsets.
// If end is less than start, sets end equal to start.
TextSpan NormalizeTextSpan(const TextContents* text, TextSpan span);

// Compares two spans of text according to strcmp rules.
int CompareTextSpans(const TextContents* textA, TextSpan spanA, const TextContents* textB, TextSpan spanB);
