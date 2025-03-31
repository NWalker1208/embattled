#pragma once
#include <stdint.h>
#include "utilities/text.h"
#include "processor/process.h"

// A mapping between an assembly line and a memory address.
typedef struct {
  size_t assemblyLineIndex;
  unsigned short memoryAddress;
} AssemblyMemoryMapping;

// An assembled binary program.
typedef struct {
  // The number of assembly-memory mappings.
  size_t assemblyMemoryMappingsCount;
  // The array of assembly-memory mappings.
  AssemblyMemoryMapping* assemblyMemoryMappings;

  // The bytes of the program.
  unsigned char bytes[MEMORY_SIZE];
} BinaryProgram;

// Frees the memory referenced by the BinaryProgram struct.
void DestroyBinaryProgram(BinaryProgram* program);
