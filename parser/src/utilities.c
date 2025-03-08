#include <stdlib.h>
#include <ctype.h>
#include "parser/utilities.h"

inline bool isInlineWhitespace(char c) {
  return c == ' ' || c == '\t';
}

inline bool isEndOfLine(char c) {
  return c == '\n' || c == '\r';
}

inline bool isAnyWhitespace(char c) {
  return isInlineWhitespace(c) || isEndOfLine(c);
}

inline bool isWordChar(char c) {
  return isalnum(c) || c == '_';
}

bool startsWithWordCaseInsensitive(const char* text, const char* word) {
  while (*text != '\0' && *word != '\0') {
    if (tolower(*text) != tolower(*word)) {
      return false;
    }
    text++;
    word++;
  }
  return *word == '\0' && !isWordChar(*text);
}

bool skipInlineWhitespace(const char** text) {
  bool skippedAny = false;
  while (isInlineWhitespace(**text)) {
    (*text)++;
    skippedAny = true;
  }
  return skippedAny;
}

void skipAllWhitespace(const char** text) {
  while (isAnyWhitespace(**text)) {
    (*text)++;
  }
}

void skipToNextLine(const char** text) {
  while (**text != '\0' && **text != '\n') {
    (*text)++;
  }
  if (**text == '\n') {
    (*text)++;
  }
}

const char* findCharOnLine(const char* str, char c) {
  while (*str != '\0' && !isEndOfLine(*str)) {
    if (*str == c) {
      return str;
    }
    str++;
  }
  return NULL;
}

bool tryHexToNibble(char c, unsigned char* result) {
  if ('0' <= c && c <= '9') {
    *result = c - '0';
    return true;
  } else if ('a' <= c && c <= 'f') {
    *result = c - 'a' + 10;
    return true;
  } else if ('A' <= c && c <= 'F') {
    *result = c - 'A' + 10;
    return true;
  } else {
    return false;
  }
}
