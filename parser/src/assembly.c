#include <stdlib.h>
#include "parser/assembly.h"

void destroyAssemblyLine(struct AssemblyLine* line) {
  free(line->label);
  line->label = NULL;
  if (line->kind == INSTRUCTION) {
    destroyAssemblyInstruction(&line->instruction);
  } else {
    destroyAssemblyData(&line->data);
  }
}

void destroyAssemblyInstruction(struct AssemblyInstruction* instruction) {
  if (instruction->parameters != NULL) {
    for (int i = 0; i < instruction->parameterCount; i++) {
      destroyAssemblyParameter(&instruction->parameters[i]);
    }
    free(instruction->parameters);
    instruction->parameters = NULL;
  }
  instruction->parameterCount = 0;
}

void destroyAssemblyParameter(struct AssemblyParameter* parameter) {
  if (parameter->kind == LABEL_REFERENCE) {
    free(parameter->referencedLabel);
    parameter->referencedLabel = NULL;
  }
}

void destroyAssemblyData(struct AssemblyData* data) {
  free(data->bytes);
  data->bytes = NULL;
  data->length = 0;
}
