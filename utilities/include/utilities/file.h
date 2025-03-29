#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "utilities/text.h"

// Reads the entire contents of a file into a dynamically allocated null-terminated char array.
// Outputs the length of the char array excluding the null terminator.
// If an error occurs, returns NULL and sets length to 0.
char* ReadAllText(const char* filePath, size_t* length);
