#include <stdlib.h>
#include "processor/process.h"
#include "processor/instruction.h"

static inline uint16_t* ptrOrDefault(uint16_t* ptr, uint16_t* defaultPtr) {
  return ptr != NULL ? ptr : defaultPtr;
}

void stepProcess(ProcessState* state) {
  Instruction instruction = { 0 };
  state->registers.ip += fetchInstruction(state->memory, state->registers.ip, &instruction);

  const OpcodeInfo* opcodeInfo = getOpcodeInfo(instruction.opcode);
  if (opcodeInfo == NULL) {
    return;
  }

  uint16_t nl = 0;
  opcodeInfo->execute((OpcodeArguments){
    .process = state,
    .registerAPtr = ptrOrDefault(getRegisterPtr(&state->registers, instruction.operands.registerA), &nl),
    .registerBPtr = ptrOrDefault(getRegisterPtr(&state->registers, instruction.operands.registerB), &nl),
    .registerCPtr = ptrOrDefault(getRegisterPtr(&state->registers, instruction.operands.registerC), &nl),
    .immediateA = instruction.operands.immediateA,
    .immediateB =instruction.operands.immediateB,
  });
}
