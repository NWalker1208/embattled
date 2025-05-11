#include <stdlib.h>
#include "assembler/assembly.h"

void DestroyAssemblyProgram(AssemblyProgram* program) {
  free(program->lines);
  program->lines = NULL;
  program->lineCount = 0;
  free(program->dataBuffer);
  program->dataBuffer = NULL;
}
