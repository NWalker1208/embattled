#pragma once
#include <stdint.h>
#include "utilities/text.h"
#include "processor/process.h"

// A mapping between a source span and a memory address.
typedef struct {
  TextSpan sourceSpan;
  unsigned short memoryAddress;
} SourceMemoryMapping;

// An assembled binary program.
typedef struct {
  // The number of source-memory mappings.
  size_t mappingsCount;
  // The array of source-memory mappings sorted by source span.
  SourceMemoryMapping* sourceToMemoryMappings;
  // The array of source-memory mappings sorted by memory address.
  SourceMemoryMapping* memoryToSourceMappings;

  // The bytes of the program.
  unsigned char bytes[MEMORY_SIZE];
} BinaryProgram;

// Frees the memory referenced by the BinaryProgram struct.
void DestroyBinaryProgram(BinaryProgram* program);
