#include <stdlib.h>
#include "assembler/assemble.h"
#include "processor/instruction.h"

#define ASSEMBLING_ERROR(_message, _lineNumber) (struct AssemblingError){.message = (_message), .lineNumber = (_lineNumber)}

bool tryAssemble(struct AssemblyLine* lines, unsigned int linesCount, unsigned char* memory, struct AssemblingError* error) {
  return false;
}
