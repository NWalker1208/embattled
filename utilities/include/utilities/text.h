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

// An offset within a TextContents struct.
typedef struct {
  // The line number of the offset.
  size_t line;
  // The column number of the offset.
  // If equal to the length of the line, represents the end of the line.
  // If greater than the length of the line, treated as equivalent to the offset (line + 1, column - len(line) - 1).
  size_t column;
} TextContentsOffset;

// A span within a TextContents struct.
typedef struct {
  // The starting offset of the span.
  TextContentsOffset start;
  // The number of characters in the span, where the end of each line counts as one character.
  size_t length;
} TextContentsSpan;

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

// Normalizes an offset such that the column number causes the offset to wrap onto the next line if it is
// greater than the length of the current line.
// If the offset lies outside the bounds of the text, wrapping continues as if there are an infinite number of 0-length lines
// past the end of the text.
TextContentsOffset NormalizeTextContentsOffset(const TextContents* text, TextContentsOffset offset);

// Gets the character at the specified offset into the text.
// If the offset lies at or beyond the end of the last line, returns '\0'.
// If the offset lies at the end of any other line, returns '\n'.
char GetCharAtTextContentsOffset(const TextContents* text, TextContentsOffset offset);

// Compares two spans of text according to strcmp rules.
int CompareTextContentsSpans(const TextContents* textA, TextContentsSpan spanA, const TextContents* textB, TextContentsSpan spanB);
