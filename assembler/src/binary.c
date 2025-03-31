#include "assembler/binary.h"
#include <stdlib.h>

void DestroyBinaryProgram(BinaryProgram* program) {
  free(program->assemblyMemoryMappings);
  program->assemblyMemoryMappings = NULL;
  program->assemblyMemoryMappingsCount = 0;
}
