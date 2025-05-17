#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "assembler/assemble.h"
#include "processor/instruction.h"
#include "processor/process.h"

#define ASSEMBLING_ERROR(_message, _sourceSpan) (AssemblingError){.message = (_message), .sourceSpan = (_sourceSpan)}


typedef struct LabelTableEntry {
  TextSpan nameSpan;
  uint16_t address;
} LabelTableEntry;

typedef struct LabelTable {
  LabelTableEntry* entries;
  size_t count;
} LabelTable;

typedef struct ReferenceTableEntry {
  TextSpan nameSpan;
  TextSpan paramSpan;
  uint16_t address;
} ReferenceTableEntry;

typedef struct ReferenceTable {
  ReferenceTableEntry* entries;
  size_t count;
} ReferenceTable;


// Handles the given assembly label.
// If successful, updates the current memory address as necessary and returns true.
// Otherwise, outputs an error and returns false.
bool tryHandleLabel(const TextContents* sourceText, TextSpan lineSpan, AssemblyLabel label, LabelTable labelTable, uint16_t* currentMemoryAddrPtr, AssemblingError* errorOut);

// Tries to convert the given assembly instruction into an actual instruction that can be written to memory.
// If successful, appends to the reference table as necessary, outputs the instruction, and returns true.
// Otherwise, outputs an error and returns false.
bool tryConvertAssemblyInstructionToInstruction(TextSpan lineSpan, AssemblyInstruction asmInstruction, ReferenceTable* referenceTablePtr, Instruction* instructionOut, AssemblingError* errorOut);

// Tries to resolve all label references by writing the addresses of the referenced labels to the given memory buffer.
// If successful, returns true.
// Otherwise, returns false.
bool tryResolveLabelReferences(const TextContents* sourceText, LabelTable labelTable, ReferenceTable referenceTable, uint8_t* memory, AssemblingError* errorOut);


bool TryAssembleProgram(const TextContents* sourceText, const AssemblyProgram* assemblyProgram, uint8_t* memory, AssemblingError* error) {
  bool success = true;
  uint16_t currentMemoryAddr = 0;
  memset(memory, 0x00, sizeof(uint8_t) * MEMORY_SIZE); // Clear memory
  // TODO: Check if program goes beyond max address.

  // Setup label and reference tables for filling in addresses
  AssemblyLabel* currentLabel = NULL;
  TextSpan currentLabelLineSpan;

  LabelTable labelTable = { .entries = malloc(sizeof(LabelTableEntry) * assemblyProgram->lineCount), .count = 0 };
  ReferenceTable referenceTable = { .entries = malloc(sizeof(ReferenceTableEntry) * assemblyProgram->lineCount), .count = 0 };

  // Write all lines to memory
  for (size_t i = 0; i < assemblyProgram->lineCount; i++) {
    AssemblyLine* line = &assemblyProgram->lines[i];

    if (line->kind == ASSEMBLY_LINE_LABEL) {
      if (currentLabel != NULL) {
        *error = ASSEMBLING_ERROR(MULTIPLE_LABELS, line->sourceSpan);
        goto failed;
      }

      if (!tryHandleLabel(sourceText, line->sourceSpan, line->label, labelTable, &currentMemoryAddr, error)) {
        goto failed;
      }

      currentLabel = &line->label;
      currentLabelLineSpan = line->sourceSpan;

    } else if (line->kind == ASSEMBLY_LINE_INSTRUCTION || line->kind == ASSEMBLY_LINE_DATA) {
      if (currentLabel != NULL) {
        if (!IsEmptyTextSpan(sourceText, currentLabel->nameSpan)) {
          labelTable.entries[labelTable.count] = (LabelTableEntry){
            .nameSpan = currentLabel->nameSpan,
            .address = currentMemoryAddr,
          };
          labelTable.count++;
        }
        currentLabel = NULL;
      }

      if (line->kind == ASSEMBLY_LINE_INSTRUCTION) {
        Instruction instruction = { 0 };
        if (!tryConvertAssemblyInstructionToInstruction(line->sourceSpan, line->instruction, &referenceTable, &instruction, error)) {
          goto failed;
        }

        uint16_t bytesWritten = writeInstruction(memory, currentMemoryAddr, instruction);
        if (bytesWritten == 0) {
          *error = ASSEMBLING_ERROR(INVALID_INSTRUCTION, line->sourceSpan);
          goto failed;
        }
        if (currentMemoryAddr + bytesWritten < currentMemoryAddr) {
          *error = ASSEMBLING_ERROR(OUT_OF_MEMORY, line->sourceSpan);
          goto failed;
        }

        currentMemoryAddr += bytesWritten;

      } else {
        assert(line->kind == ASSEMBLY_LINE_DATA);
        AssemblyData data = line->data;

        if (data.byteCount > 0xFFFF || currentMemoryAddr + (uint16_t)data.byteCount < currentMemoryAddr) {
          *error = ASSEMBLING_ERROR(OUT_OF_MEMORY, line->sourceSpan);
          goto failed;
        }
        
        // Write the data directly to memory
        memcpy(&memory[currentMemoryAddr], &assemblyProgram->dataBuffer[data.startIndex], data.byteCount);
        currentMemoryAddr += data.byteCount;
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
  if (!tryResolveLabelReferences(sourceText, labelTable, referenceTable, memory, error)) {
    goto failed;
  }

  goto done;

failed:
  success = false;
done:
  // Free dynamically allocated memory
  free(labelTable.entries);
  free(referenceTable.entries);

  return success;
}


bool tryHandleLabel(const TextContents* sourceText, TextSpan lineSpan, AssemblyLabel label, LabelTable labelTable, uint16_t* currentMemoryAddrPtr, AssemblingError* errorOut) {
  bool hasName = !IsEmptyTextSpan(sourceText, label.nameSpan);
  bool hasAddress = !IsEmptyTextSpan(sourceText, label.addressSpan);
  
  if (!hasName && !hasAddress) {
    *errorOut = ASSEMBLING_ERROR(INVALID_LABEL, lineSpan);
    return false;
  }

  if (hasName) {
    for (size_t i = 0; i < labelTable.count; i++) {
      if (CompareTextSpans(sourceText, label.nameSpan, sourceText, labelTable.entries[i].nameSpan) == 0) {
        *errorOut = ASSEMBLING_ERROR(DUPLICATE_LABEL_NAME, label.nameSpan);
        return false;
      }
    }
  }

  if (hasAddress) {
    if (label.address < *currentMemoryAddrPtr) {
      *errorOut = ASSEMBLING_ERROR(LABEL_ADDRESS_TOO_LOW, lineSpan);
      return false;
    }

    *currentMemoryAddrPtr = label.address;
  }

  return true;
}

bool tryConvertAssemblyInstructionToInstruction(TextSpan lineSpan, AssemblyInstruction asmInstruction, ReferenceTable* referenceTablePtr, Instruction* instructionOut, AssemblingError* errorOut) {
  return false;
}

bool tryResolveLabelReferences(const TextContents* sourceText, LabelTable labelTable, ReferenceTable referenceTable, uint8_t* memory, AssemblingError* errorOut) {
  for (size_t i = 0; i < referenceTable.count; i++) {
    uint16_t referenceAddress = referenceTable.entries[i].address;
    TextSpan nameSpan = referenceTable.entries[i].nameSpan;

    uint16_t labelAddress;
    bool foundLabel = false;
    for (size_t j = 0; j < labelTable.count; j++) {
      if (CompareTextSpans(sourceText, labelTable.entries[j].nameSpan, sourceText, nameSpan) == 0) {
        labelAddress = labelTable.entries[j].address;
        foundLabel = true;
        break;
      }
    }
    if (!foundLabel) {
      *errorOut = ASSEMBLING_ERROR(UNDEFINED_LABEL_NAME, referenceTable.entries[i].paramSpan);
      return false;
    }

    memory[referenceAddress] = labelAddress & 0xFF;
    memory[referenceAddress + 1] = labelAddress >> 8;
  }

  return true;
}
