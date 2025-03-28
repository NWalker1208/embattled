#pragma once
#include <stdbool.h>
#include <string.h>
#include "utilities/text.h"

// Reads the entire contents of a file as text into a TextContents struct.
// If an error occurs, returns uninitialized text contents.
TextContents readAllText(const char* filePath);
