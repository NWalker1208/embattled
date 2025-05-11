#pragma once
#include <stdint.h>
#include "utilities/text.h"
#include "processor/process.h"

// A mapping between an assembly line and a memory address.
typedef struct AssemblyMemoryMapping {
  size_t assemblyLineIndex;
  uint16_t memoryAddress;
} AssemblyMemoryMapping;

// An assembled binary program.
// TODO: Make this the output of TryAssembleProgram instead of a plain memory array.
typedef struct BinaryProgram {
  // The number of assembly-memory mappings.
  size_t assemblyMemoryMappingsCount;
  // The array of assembly-memory mappings.
  AssemblyMemoryMapping* assemblyMemoryMappings;

  // The bytes of the program.
  uint8_t bytes[MEMORY_SIZE];
} BinaryProgram;

// Frees the memory referenced by the BinaryProgram struct.
void DestroyBinaryProgram(BinaryProgram* program);
