#pragma once
#include <stdint.h>
#include <stdbool.h>

// A line of a text contents struct.
typedef struct {
  size_t startIndex;
  size_t length;
} TextContentsLine;

// A collection of lines of text.
typedef struct {
  // The text as an array of chars with null characters between each line.
  const char* chars;
  // The length of the chars array.
  size_t length;

  // The array of starting indexes and lengths of each line of text.
  TextContentsLine* lines;
  // The number of lines of text.
  unsigned int lineCount;
} TextContents;

// An offset within a TextContents struct.
typedef struct {
  size_t line;
  size_t column;
} TextContentsOffset;

// A span within a TextContents struct.
typedef struct {
  TextContentsOffset start;
  size_t length;
} TextContentsSpan;

// Frees the memory referenced by a TextContents struct.
void DestroyTextContents(TextContents* contents);

// Checks if the TextContents struct has been initialized.
bool TextContentsIsInitialized(const TextContents* contents);

// Gets the line of text at the specified line number.
// If the line number is out of bounds, returns NULL.
const char* TextContentsGetLine(const TextContents* contents, size_t line);
