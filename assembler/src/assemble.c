#include <stdlib.h>
#include <string.h>
#include "assembler/assemble.h"
#include "processor/instruction.h"

#define ASSEMBLING_ERROR(_message, _sourceSpan) (AssemblingError){.message = (_message), .sourceSpan = (_sourceSpan)}

bool TryAssemble(const AssemblyProgram* program, unsigned char* memory, AssemblingError* error) {
  unsigned short currentMemoryAddr = 0;

  // Setup label and reference tables for filling in addresses
  TextContentsSpan* currentLabelSpan = NULL;

  TextContentsSpan* labelTableLabelSpans = malloc(sizeof(TextContentsSpan) * program->lineCount);
  unsigned short* labelTableAddresses = malloc(sizeof(unsigned short) * program->lineCount);
  size_t labelCount = 0;

  unsigned short* referenceTableAddresses = malloc(sizeof(unsigned short) * program->lineCount);
  TextContentsSpan* referenceTableLabelSpans = malloc(sizeof(TextContentsSpan) * program->lineCount);
  size_t referenceCount = 0;

  // Write all lines to memory
  for (size_t i = 0; i < program->lineCount; i++) {
    AssemblyLine line = program->lines[i];

    switch (line.kind) {
      case ASSEMBLY_LINE_LABEL: {
        if (currentLabelSpan != NULL) {
          *error = ASSEMBLING_ERROR(MULTIPLE_LABELS, line.sourceSpan);
          return false;
        }
        for (size_t j = 0; j < labelCount; j++) {
          if (TextContentsCompareSpans(&program->sourceText, line.labelSpan, &program->sourceText, labelTableLabelSpans[j]) == 0) {
            *error = ASSEMBLING_ERROR(DUPLICATE_LABEL, line.labelSpan);
            return false;
          }
        }
        currentLabelSpan = &line.labelSpan;
      } break;

      case ASSEMBLY_LINE_ADDRESS: {
        if (line.address < currentMemoryAddr) {
          *error = ASSEMBLING_ERROR(ADDRESS_TOO_LOW, line.sourceSpan);
          return false;
        }
        currentMemoryAddr = line.address;
      } break;


      case ASSEMBLY_LINE_INSTRUCTION:
      case ASSEMBLY_LINE_DATA: {
        if (currentLabelSpan != NULL) {
          labelTableLabelSpans[labelCount] = *currentLabelSpan;
          labelTableAddresses[labelCount] = currentMemoryAddr;
          labelCount++;
          currentLabelSpan = NULL;
        }

        if (line.kind == ASSEMBLY_LINE_INSTRUCTION) {
          struct Instruction instruction;
          instruction.opcode = line.instruction.opcode;
          
          const struct OpcodeInfo* opcodeInfo = &OPCODE_INFO[instruction.opcode];
          bool hasRegA = opcodeInfo->parameterLayout.hasRegA;
          bool hasRegB = opcodeInfo->parameterLayout.hasRegB;
          bool hasImmediate = opcodeInfo->parameterLayout.numImmBits != 0;
          
          unsigned int requiredParameterCount =
            (hasRegA ? 1 : 0) + (hasRegB ? 1 : 0) + (hasImmediate ? 1 : 0);
          if (line.instruction.parameterCount < requiredParameterCount) {
            *error = ASSEMBLING_ERROR(TOO_FEW_PARAMS, line.sourceSpan);
            goto failed;
          } else if (line.instruction.parameterCount > requiredParameterCount) {
            *error = ASSEMBLING_ERROR(TOO_MANY_PARAMS, line.sourceSpan);
            goto failed;
          }

          // Extract the parameters
          unsigned int p = 0;

          if (hasRegA) {
            AssemblyParameter param = line.instruction.parameters[p];
            if (param.kind != ASSEMBLY_PARAM_REGISTER) {
              *error = ASSEMBLING_ERROR(EXPECTED_REGISTER, param.sourceSpan);
              goto failed;
            }
            p++;

            instruction.parameters.registerA = param.registerName;
          }

          if (hasRegB) {
            AssemblyParameter param = line.instruction.parameters[p];
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
            AssemblyParameter param = line.instruction.parameters[p];
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
              referenceCount++;

              instruction.parameters.immediate.u16 = 0; // Will be filled in later
            }
          }

          unsigned short bytesWritten = storeInstruction(memory, currentMemoryAddr, instruction);
          if (bytesWritten == 0) {
            *error = ASSEMBLING_ERROR(INVALID_INSTRUCTION, line.sourceSpan);
            goto failed;
          }

          currentMemoryAddr += bytesWritten;
        } else { // DATA
          AssemblyData data = line.data;
          
          // Write the data directly to memory
          memcpy(&memory[currentMemoryAddr], data.bytes, data.length);
          currentMemoryAddr += data.length;
        }
      } break;

      default: {
        *error = ASSEMBLING_ERROR(INVALID_LINE, line.sourceSpan);
        return false;
      }
    }
  }

  // Fill in label references
  for (size_t i = 0; i < referenceCount; i++) {
    unsigned short referenceAddress = referenceTableAddresses[i];
    TextContentsSpan labelSpan = referenceTableLabelSpans[i];

    unsigned short labelAddress;
    bool foundLabel = false;
    for (unsigned int j = 0; j < labelCount; j++) {
      if (TextContentsCompareSpans(&program->sourceText, labelTableLabelSpans[j], &program->sourceText, labelSpan) == 0) {
        labelAddress = labelTableAddresses[j];
        foundLabel = true;
        break;
      }
    }
    if (!foundLabel) {
      *error = ASSEMBLING_ERROR(UNDEFINED_LABEL, labelSpan);
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

  return true;

failed:
  free(labelTableLabelSpans);
  free(labelTableAddresses);
  free(referenceTableLabelSpans);
  free(referenceTableAddresses);
  
  return false;
}
