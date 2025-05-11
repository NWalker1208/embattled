#include <stdlib.h>
#include <string.h>
#include "assembler/assemble.h"
#include "processor/instruction.h"
#include "processor/process.h"

#define ASSEMBLING_ERROR(_message, _sourceSpan) (AssemblingError){.message = (_message), .sourceSpan = (_sourceSpan)}

bool TryAssembleProgram(const TextContents* sourceText, const AssemblyProgram* assemblyProgram, uint8_t* memory, AssemblingError* error) {
  unsigned short currentMemoryAddr = 0;
  memset(memory, 0x00, sizeof(unsigned char) * MEMORY_SIZE); // Clear memory
  // TODO: Check if program goes beyond max address.

  // Setup label and reference tables for filling in addresses
  AssemblyLabel* currentLabel = NULL;
  TextSpan currentLabelLineSpan;

  TextSpan* labelTableLabelSpans = malloc(sizeof(TextSpan) * assemblyProgram->lineCount);
  unsigned short* labelTableAddresses = malloc(sizeof(unsigned short) * assemblyProgram->lineCount);
  size_t labelCount = 0;

  unsigned short* referenceTableAddresses = malloc(sizeof(unsigned short) * assemblyProgram->lineCount);
  TextSpan* referenceTableLabelSpans = malloc(sizeof(TextSpan) * assemblyProgram->lineCount);
  TextSpan* referenceTableParamSpans = malloc(sizeof(TextSpan) * assemblyProgram->lineCount);
  size_t referenceCount = 0;

  // Write all lines to memory
  for (size_t i = 0; i < assemblyProgram->lineCount; i++) {
    AssemblyLine* line = &assemblyProgram->lines[i];

    if (line->kind == ASSEMBLY_LINE_LABEL) {
      if (currentLabel != NULL) {
        *error = ASSEMBLING_ERROR(MULTIPLE_LABELS, line->sourceSpan);
        goto failed;
      }
      
      bool hasName = !IsEmptyTextSpan(sourceText, line->label.nameSpan);
      bool hasAddress = !IsEmptyTextSpan(sourceText, line->label.addressSpan);
      if (!hasName && !hasAddress) {
        *error = ASSEMBLING_ERROR(INVALID_LABEL, line->sourceSpan);
        goto failed;
      }

      if (hasName) {
        for (size_t j = 0; j < labelCount; j++) {
          if (CompareTextSpans(sourceText, line->label.nameSpan, sourceText, labelTableLabelSpans[j]) == 0) {
            *error = ASSEMBLING_ERROR(DUPLICATE_LABEL_NAME, line->label.nameSpan);
            goto failed;
          }
        }
      }

      if (hasAddress) {
        if (line->label.address < currentMemoryAddr) {
          *error = ASSEMBLING_ERROR(LABEL_ADDRESS_TOO_LOW, line->sourceSpan);
          goto failed;
        }

        currentMemoryAddr = line->label.address;
      }

      currentLabel = &line->label;
      currentLabelLineSpan = line->sourceSpan;
    } else if (line->kind == ASSEMBLY_LINE_INSTRUCTION || line->kind == ASSEMBLY_LINE_DATA) {
      if (currentLabel != NULL) {
        if (!IsEmptyTextSpan(sourceText, currentLabel->nameSpan)) {
          labelTableLabelSpans[labelCount] = currentLabel->nameSpan;
          labelTableAddresses[labelCount] = currentMemoryAddr;
          labelCount++;
        }
        currentLabel = NULL;
      }

      if (line->kind == ASSEMBLY_LINE_INSTRUCTION) {
        struct Instruction instruction;
        instruction.opcode = line->instruction.opcode;

        if (instruction.opcode < 0 || instruction.opcode >= OPCODE_COUNT) {
          *error = ASSEMBLING_ERROR(INVALID_INSTRUCTION, line->sourceSpan);
          goto failed;
        }
        
        const struct OpcodeInfo* opcodeInfo = &OPCODE_INFO[instruction.opcode];
        bool hasRegA = opcodeInfo->parameterLayout.hasRegA;
        bool hasRegB = opcodeInfo->parameterLayout.hasRegB;
        bool hasImmediate = opcodeInfo->parameterLayout.numImmBits != 0;
        
        unsigned int requiredParameterCount =
          (hasRegA ? 1 : 0) + (hasRegB ? 1 : 0) + (hasImmediate ? 1 : 0);
        if (line->instruction.parameterCount < requiredParameterCount) {
          *error = ASSEMBLING_ERROR(TOO_FEW_PARAMS, line->sourceSpan);
          goto failed;
        } else if (line->instruction.parameterCount > requiredParameterCount) {
          *error = ASSEMBLING_ERROR(TOO_MANY_PARAMS, line->sourceSpan);
          goto failed;
        }

        // Extract the parameters
        unsigned int p = 0;

        if (hasRegA) {
          AssemblyParameter param = line->instruction.parameters[p];
          if (param.kind != ASSEMBLY_PARAM_REGISTER) {
            *error = ASSEMBLING_ERROR(EXPECTED_REGISTER, param.sourceSpan);
            goto failed;
          }
          p++;

          instruction.parameters.registerA = param.registerName;
        }

        if (hasRegB) {
          AssemblyParameter param = line->instruction.parameters[p];
          if (param.kind != ASSEMBLY_PARAM_REGISTER) {
            *error = ASSEMBLING_ERROR(EXPECTED_REGISTER, param.sourceSpan);
            goto failed;
          }
          p++;

          instruction.parameters.registerB = param.registerName;
        }

        if (hasImmediate) {
          bool immIsSigned = opcodeInfo->parameterLayout.immIsSigned;
          unsigned char numImmBits = opcodeInfo->parameterLayout.numImmBits;
          AssemblyParameter param = line->instruction.parameters[p];
          if (numImmBits != 16) {
            if (param.kind != ASSEMBLY_PARAM_IMMEDIATE) {
              *error = ASSEMBLING_ERROR(EXPECTED_IMMEDIATE_VALUE, param.sourceSpan);
              goto failed;
            }
          } else {
            if (param.kind != ASSEMBLY_PARAM_IMMEDIATE && param.kind != ASSEMBLY_PARAM_LABEL) {
              *error = ASSEMBLING_ERROR(EXPECTED_IMMEDIATE_VALUE_OR_LABEL_REF, param.sourceSpan);
              goto failed;
            }
          }

          if (param.kind == ASSEMBLY_PARAM_IMMEDIATE) {
            signed int minValue = immIsSigned ? (~0U << (numImmBits - 1)) : 0;
            signed int maxValue = immIsSigned ? ((1 << (numImmBits - 1)) - 1) : ((1 << numImmBits) - 1);

            signed int immediateValue = param.immediateValue;
            if (immediateValue < minValue || immediateValue > maxValue) {
              *error = ASSEMBLING_ERROR(IMMEDIATE_VALUE_OUT_OF_RANGE, param.sourceSpan);
              goto failed;
            }

            instruction.parameters.immediate.u16 = (unsigned short)immediateValue;
          } else { // LABEL_REFERENCE
            referenceTableAddresses[referenceCount] = currentMemoryAddr + 1; // Immediate value always starts on the second byte of the instruction
            referenceTableLabelSpans[referenceCount] = param.labelSpan;
            referenceTableParamSpans[referenceCount] = param.sourceSpan;
            referenceCount++;

            instruction.parameters.immediate.u16 = 0; // Will be filled in later
          }
        }

        unsigned short bytesWritten = storeInstruction(memory, currentMemoryAddr, instruction);
        if (bytesWritten == 0) {
          *error = ASSEMBLING_ERROR(INVALID_INSTRUCTION, line->sourceSpan);
          goto failed;
        }

        currentMemoryAddr += bytesWritten;
      } else { // DATA
        AssemblyData data = line->data;
        
        // Write the data directly to memory
        memcpy(&memory[currentMemoryAddr], data.bytes, data.length);
        currentMemoryAddr += data.length;
      }
    } else {
      *error = ASSEMBLING_ERROR(INVALID_LINE, line->sourceSpan);
      goto failed;
    }
  }

  if (currentLabel != NULL) {
    *error = ASSEMBLING_ERROR(EXPECTED_INSTRUCTION_OR_DATA, currentLabelLineSpan);
    goto failed;
  }

  // Fill in label references
  for (size_t i = 0; i < referenceCount; i++) {
    unsigned short referenceAddress = referenceTableAddresses[i];
    TextSpan labelSpan = referenceTableLabelSpans[i];

    unsigned short labelAddress;
    bool foundLabel = false;
    for (unsigned int j = 0; j < labelCount; j++) {
      if (CompareTextSpans(sourceText, labelTableLabelSpans[j], sourceText, labelSpan) == 0) {
        labelAddress = labelTableAddresses[j];
        foundLabel = true;
        break;
      }
    }
    if (!foundLabel) {
      *error = ASSEMBLING_ERROR(UNDEFINED_LABEL_NAME, referenceTableParamSpans[i]);
      goto failed;
    }

    memory[referenceAddress] = labelAddress & 0xFF;
    memory[referenceAddress + 1] = labelAddress >> 8;
  }

  // Free dynamically allocated memory
  free(labelTableLabelSpans);
  free(labelTableAddresses);
  free(referenceTableLabelSpans);
  free(referenceTableAddresses);
  free(referenceTableParamSpans);

  return true;

failed:
  free(labelTableLabelSpans);
  free(labelTableAddresses);
  free(referenceTableLabelSpans);
  free(referenceTableAddresses);
  free(referenceTableParamSpans);
  
  return false;
}
