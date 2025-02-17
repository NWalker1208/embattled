#include "instruction.h"

// Parses the next instruction from the given text and advances the text pointer.
// If an invalid instruction is encountered, the text pointer is set to NULL.
struct Instruction parseNextInstruction(char** text);
