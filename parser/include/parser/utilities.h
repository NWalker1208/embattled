#pragma once
#include <stdbool.h>

// Returns true if c is a space or a tab character. Otherwise, returns false.
static inline bool isInlineWhitespace(char c) { return c == ' ' || c == '\t'; }

// Returns true if c is a newline or a carriage return. Otherwise, returns false.
static inline bool isEndOfLine(char c) { return c == '\n' || c == '\r'; }

// Returns true if c is an inline whitespace or end-of-line character. Otherwise, returns false.
// Similar to the standard isspace() function, but does not return true for \f or \v.
static inline bool isAnyWhitespace(char c) { return isInlineWhitespace(c) || isEndOfLine(c); }

// Returns true if c is a letter, number, or underscore. Otherwise, returns false.
bool isWordChar(char c);

// Returns true if text begins with the specified word. Otherwise, returns false.
bool startsWithWordCaseInsensitive(const char* text, const char* word);

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
