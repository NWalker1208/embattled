#include <stdlib.h>
#include <string.h>
#include "assembler/assemble.h"
#include "processor/instruction.h"

#define ASSEMBLING_ERROR(_message, _lineNumber) (struct AssemblingError){.message = (_message), .lineNumber = (_lineNumber)}

bool tryAssemble(struct AssemblyLine* lines, unsigned int linesCount, unsigned char* memory, struct AssemblingError* error) {
  unsigned short currentMemoryAddr = 0;

  // Setup label and reference tables for filling in addresses
  const char** labelTableLabels = malloc(sizeof(char*) * linesCount);
  unsigned short* labelTableAddresses = malloc(sizeof(unsigned short) * linesCount);
  unsigned int labelCount = 0;

  unsigned short* referenceTableAddresses = malloc(sizeof(unsigned short) * linesCount);
  const char** referenceTableLabels = malloc(sizeof(char*) * linesCount);
  unsigned int* referenceTableLineNumbers = malloc(sizeof(unsigned int) * linesCount);
  unsigned int referenceCount = 0;

  // Write all lines to memory
  for (unsigned int lineNumber = 0; lineNumber < linesCount; lineNumber++) {
    struct AssemblyLine line = lines[lineNumber];

    if (line.label != NULL) {
      labelTableLabels[labelCount] = line.label;
      labelTableAddresses[labelCount] = currentMemoryAddr;
      labelCount++;
    }

    if (line.kind == INSTRUCTION) {
      struct Instruction instruction;
      instruction.opcode = line.instruction.opcode;
      
      const struct OpcodeInfo* opcodeInfo = &OPCODE_INFO[instruction.opcode];
      bool hasRegA = opcodeInfo->parameterLayout.hasRegA;
      bool hasRegB = opcodeInfo->parameterLayout.hasRegB;
      bool hasImmediate = opcodeInfo->parameterLayout.numImmBits != 0;
      
      unsigned int requiredParameterCount =
        (hasRegA ? 1 : 0) + (hasRegB ? 1 : 0) + (hasImmediate ? 1 : 0);
      if (line.instruction.parameterCount < requiredParameterCount) {
        *error = ASSEMBLING_ERROR(TOO_FEW_PARAMS, lineNumber);
        goto failed;
      } else if (line.instruction.parameterCount > requiredParameterCount) {
        *error = ASSEMBLING_ERROR(TOO_MANY_PARAMS, lineNumber);
        goto failed;
      }

      // Extract the parameters
      unsigned int p = 0;

      if (hasRegA) {
        struct AssemblyParameter param = line.instruction.parameters[p];
        if (param.kind != REGISTER) {
          *error = ASSEMBLING_ERROR(EXPECTED_REGISTER, lineNumber);
          goto failed;
        }
        p++;

        instruction.parameters.registerA = param.registerName;
      }

      if (hasRegB) {
        struct AssemblyParameter param = line.instruction.parameters[p];
        if (param.kind != REGISTER) {
          *error = ASSEMBLING_ERROR(EXPECTED_REGISTER, lineNumber);
          goto failed;
        }
        p++;

        instruction.parameters.registerB = param.registerName;
      }

      if (hasImmediate) {
        bool immIsSigned = opcodeInfo->parameterLayout.immIsSigned;
        unsigned char numImmBits = opcodeInfo->parameterLayout.numImmBits;
        struct AssemblyParameter param = line.instruction.parameters[p];
        if (numImmBits != 16 && param.kind != IMMEDIATE_VALUE) {
          *error = ASSEMBLING_ERROR(EXPECTED_IMMEDIATE_VALUE, lineNumber);
          goto failed;
        }
        if (param.kind != IMMEDIATE_VALUE && param.kind != LABEL_REFERENCE) {
          *error = ASSEMBLING_ERROR(EXPECTED_IMMEDIATE_VALUE_OR_LABEL_REF, lineNumber);
          goto failed;
        }

        if (param.kind == IMMEDIATE_VALUE) {
          signed int minValue = immIsSigned ? (-1 << (numImmBits - 1)) : 0;
          signed int maxValue = immIsSigned ? ((1 << (numImmBits - 1)) - 1) : ((1 << numImmBits) - 1);

          signed int immediateValue = param.immediateValue;
          if (immediateValue < minValue || immediateValue > maxValue) {
            *error = ASSEMBLING_ERROR(IMMEDIATE_VALUE_OUT_OF_RANGE, lineNumber);
            goto failed;
          }

          instruction.parameters.immediate.u16 = (unsigned short)immediateValue;
        } else { // LABEL_REFERENCE
          referenceTableAddresses[referenceCount] = currentMemoryAddr + 1; // Immediate value always starts on the second byte of the instruction
          referenceTableLabels[referenceCount] = param.referencedLabel;
          referenceTableLineNumbers[referenceCount] = lineNumber;
          referenceCount++;

          instruction.parameters.immediate.u16 = 0; // Will be filled in later
        }
      }

      int bytesWritten = storeInstruction(memory, currentMemoryAddr, instruction);
      if (bytesWritten == 0) {
        *error = ASSEMBLING_ERROR(INVALID_INSTRUCTION, lineNumber);
        goto failed;
      }

      currentMemoryAddr += bytesWritten;

    } else { // DATA
      struct AssemblyData data = line.data;
      
      // Write the data directly to memory
      memcpy(&memory[currentMemoryAddr], data.bytes, data.length);
      currentMemoryAddr += data.length;
    }
  }

  // Fill in label references
  for (unsigned int i = 0; i < referenceCount; i++) {
    unsigned short referenceAddress = referenceTableAddresses[i];
    const char* label = referenceTableLabels[i];

    unsigned short labelAddress;
    bool foundLabel = false;
    for (unsigned int j = 0; j < labelCount; j++) {
      if (strcmp(labelTableLabels[j], label) == 0) {
        labelAddress = labelTableAddresses[j];
        foundLabel = true;
        break;
      }
    }
    if (!foundLabel) {
      *error = ASSEMBLING_ERROR(UNDEFINED_LABEL, referenceTableLineNumbers[i]);
      goto failed;
    }

    memory[referenceAddress] = labelAddress & 0xFF;
    memory[referenceAddress + 1] = labelAddress >> 8;
  }

  // Free dynamically allocated memory
  free(labelTableLabels);
  free(labelTableAddresses);
  free(referenceTableAddresses);
  free(referenceTableLabels);
  free(referenceTableLineNumbers);

  return true;

failed:
  free(labelTableLabels);
  free(labelTableAddresses);
  free(referenceTableAddresses);
  free(referenceTableLabels);
  free(referenceTableLineNumbers);
  return false;
}
