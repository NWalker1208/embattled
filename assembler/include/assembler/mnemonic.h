#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "assembler/operand.h"

#define MAX_MNEMONIC_OVERLOADS 4

// An identifier that maps to one or more opcodes depending on the parameters provided.
typedef enum AssemblyMnemonic {
  ASSEMBLY_MNEMONIC_NOP,
  ASSEMBLY_MNEMONIC_JMP,
  ASSEMBLY_MNEMONIC_JMZ,
  ASSEMBLY_MNEMONIC_SLP,
  ASSEMBLY_MNEMONIC_SET,
  ASSEMBLY_MNEMONIC_LDB,
  ASSEMBLY_MNEMONIC_LDW,
  ASSEMBLY_MNEMONIC_STB,
  ASSEMBLY_MNEMONIC_STW,
  ASSEMBLY_MNEMONIC_PSHB,
  ASSEMBLY_MNEMONIC_PSHW,
  ASSEMBLY_MNEMONIC_POPB,
  ASSEMBLY_MNEMONIC_POPW,
  ASSEMBLY_MNEMONIC_ADD,
  ASSEMBLY_MNEMONIC_SUB,
  ASSEMBLY_MNEMONIC_MUL,
  ASSEMBLY_MNEMONIC_DIVS,
  ASSEMBLY_MNEMONIC_DIVU,
  ASSEMBLY_MNEMONIC_REMS,
  ASSEMBLY_MNEMONIC_REMU,
  ASSEMBLY_MNEMONIC_AND,
  ASSEMBLY_MNEMONIC_IOR,
  ASSEMBLY_MNEMONIC_XOR,
  ASSEMBLY_MNEMONIC_LSH,
  ASSEMBLY_MNEMONIC_RSHS,
  ASSEMBLY_MNEMONIC_RSHU,
  ASSEMBLY_MNEMONIC_CEQ,
  ASSEMBLY_MNEMONIC_CNE,
  ASSEMBLY_MNEMONIC_CLTS,
  ASSEMBLY_MNEMONIC_CLTU,
  ASSEMBLY_MNEMONIC_CGES,
  ASSEMBLY_MNEMONIC_CGEU,
  ASSEMBLY_MNEMONIC_CGTS,
  ASSEMBLY_MNEMONIC_CGTU,
  ASSEMBLY_MNEMONIC_CLES,
  ASSEMBLY_MNEMONIC_CLEU,

  ASSEMBLY_MNEMONIC_COUNT
} AssemblyMnemonic;

// A particular overload of an assembly mnemonic. Matches a specific sequence of operand kinds and assembles to a specific opcode.
typedef struct AssemblyMnemonicOverload {
  size_t operandCount; // The number of operands this overload accepts.
  AssemblyOperandKind operandKinds[MAX_ASSEMBLY_OPERANDS]; // The kind of each operand accepted by this overload.
  bool swapRegisterBAndRegisterC; // Whether register C appears before register B (used by some pseudo-instructions).
  Opcode opcode; // The opcode that this overload assembles into.
} AssemblyMnemonicOverload;

// Describes the details of a particular assembly mnemonic.
typedef struct AssemblyMnemonicInfo {
  const char* identifier; // The identifier of the mnemonic as a string.
  size_t overloadCount; // The number of overloads of the mnemonic.
  AssemblyMnemonicOverload overloads[MAX_MNEMONIC_OVERLOADS]; // The overloads of the mnemonic.
} AssemblyMnemonicInfo;

// Gets the info for the specified assembly mnemonic.
// If the mnemonic is invalid, returns NULL.
const AssemblyMnemonicInfo* getAssemblyMnemonicInfo(AssemblyMnemonic mnemonic);
