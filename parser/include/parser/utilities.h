#pragma once
#include <stdbool.h>

// Returns true if text begins with prefix. Otherwise, returns false.
bool startsWithCaseInsensitive(const char* text, const char* prefix);

// Returns true if c is a space or a tab character. Otherwise, returns false.
bool isInlineWhitespace(char c);

// Advances text to the next character that isn't a space or a tab.
// Returns true if any characters were skipped. Otherwise, returns false.
bool skipInlineWhitespace(const char** text);

// Advances text to the next non-whitespace character.
void skipAllWhitespace(const char** text);

// Advances text past the next newline character.
void skipToNextLine(const char** text);

// Returns a pointer to the first occurrence of c in the first line of text.
// If c is not found in the first line of text, returns NULL.
const char* findCharOnLine(const char* text, char c);

// Attempts to convert the given character to a nibble value.
// If the character is a hexadecimal digit, sets the byte at nibble to the digit's value and returns true.
// Otherwise, returns false.
bool tryHexToNibble(char c, unsigned char* nibble);
