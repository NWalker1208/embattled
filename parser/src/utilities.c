#include <stdlib.h>
#include <ctype.h>
#include "parser/utilities.h"

bool startsWithCaseInsensitive(const char* text, const char* prefix) {
  while (*text != '\0' && *prefix != '\0') {
    if (tolower(*text) != tolower(*prefix)) {
      return false;
    }
    text++;
    prefix++;
  }
  return *prefix == '\0';
}

bool isInlineWhitespace(char c) {
  return c == ' ' || c == '\t';
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
  while (isspace(**text)) {
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
  while (*str != '\0' && *str != '\n' && *str != '\r') {
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
