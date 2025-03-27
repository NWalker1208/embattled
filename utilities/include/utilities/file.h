#pragma once
#include <stdbool.h>
#include <string.h>

// Reads the entire contents of a file as text into a dynamically allocated null-terminated string.
// If an error occurs, returns NULL.
char* readAllText(const char* filePath);
