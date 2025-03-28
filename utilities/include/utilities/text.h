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

void destroyTextContents(TextContents* contents);

// Checks if the text contents struct has been initialized.
bool isTextContentsInitialized(const TextContents* contents);

// Gets the line of text at the specified line number.
// If the line number is out of bounds, returns NULL.
const char* getLineOfTextContents(const TextContents* contents, size_t lineNumber);
