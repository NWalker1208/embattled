#include <stdlib.h>
#include "assembler/assembly.h"

void DestroyAssemblyProgram(AssemblyProgram* program) {
  DestroyTextContents(&program->sourceText);
  free(program->lines);
  program->lines = NULL;
  program->lineCount = 0;
}

void DestroyAssemblyLine(AssemblyLine* line) {
  switch (line->kind) {
    case ASSEMBLY_LINE_INSTRUCTION: {
      DestroyAssemblyInstruction(&line->instruction);
      break;
    }
    case ASSEMBLY_LINE_DATA: {
      DestroyAssemblyData(&line->data);
      break;
    }
    default: break;
  }
}

void DestroyAssemblyInstruction(AssemblyInstruction* instruction) {
  free(instruction->parameters);
  instruction->parameters = NULL;
  instruction->parameterCount = 0;
}

void DestroyAssemblyData(AssemblyData* data) {
  free(data->bytes);
  data->bytes = NULL;
  data->length = 0;
}
