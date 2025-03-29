#pragma once
#include <stdbool.h>
#include "utilities/text.h"

// Returns true if c is a space or a tab character. Otherwise, returns false.
static inline bool isInlineWhitespace(char c) { return c == ' ' || c == '\t'; }

// Returns true if c is an inline whitespace or end-of-line character. Otherwise, returns false.
// Similar to the standard isspace() function, but does not return true for \f or \v.
static inline bool isAnyWhitespace(char c) { return isInlineWhitespace(c) || c == '\n'; }

// Returns true if c is a letter, number, or underscore. Otherwise, returns false.
bool isWordChar(char c);

// Returns true if text begins with the specified word at the specified offset. Otherwise, returns false.
bool startsWithWordCaseInsensitive(const TextContents* text, TextOffset start, const char* word);

// Advances position to the next character that isn't a space or a tab.
// Returns true if any characters were skipped. Otherwise, returns false.
bool skipInlineWhitespace(const TextContents* text, TextOffset* position);

// Advances position to the next non-whitespace character.
void skipAllWhitespace(const TextContents* text, TextOffset* position);

// Advances position to the next whitespace character.
void skipToNextWhitespace(const TextContents* text, TextOffset* position);

// Advances position to the next line or the end of the text.
static inline void skipToNextLine(const TextContents* text, TextOffset* position) {
  position->line++;
  position->column = 0;
  NormalizeTextOffset(text, position);
}

// Searches one line of text for the specified character starting at the specified offset.
// Returns true if the character is found. Otherwise, returns false.
bool lineContainsChar(const TextContents* text, TextOffset start, char c);

// Attempts to convert the given character to a nibble value.
// If the character is a hexadecimal digit, sets the byte at nibble to the digit's value and returns true.
// Otherwise, returns false.
bool tryHexToNibble(char c, unsigned char* nibble);
