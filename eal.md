# Embattled Assembly Language

Embattled robots are programmed using an artificial assembly language called Embattled Assembly Language (EAL). EAL programs describe step by step what actions a robot's processor should perform.

However, before learning how to write EAL programs, it is important to understand the basics of how Embattled robot processors work.

## Key Concepts: Registers and Memory

Embattled robot processors operate on just two things: registers and memory. Both of these store binary data, but they are used in different ways.

A robot's **memory** holds both the program that the processor is executing as well as any data that has been written by the program over the course of the simulation. It is essentially just a large array of bytes (65,536 bytes to be exact) that can be read from and written to. Any byte in that array can be located via its 16-bit address. However, the processor can't perform any operations on memory beyond just reading and writing bytes.

**Registers** are where the processor stores both the inputs and outputs of the actions it performs. Each register contains exactly 2 bytes (16 bits). The processor can read bytes (or pairs of bytes) from memory into registers, perform actions on the values in those registers, and write the values of registers back into memory. From the perspective of an EAL program, registers essentially act as variables.

In total, there are sixteen different registers. Twelve of the registers are general purpose, meaning they can be used in whatever way an EAL program sees fit. The general purpose registers are named `x0`, `x1`, etc. all the way up to `x11`. The other four registers each have a special purpose:
- `nl` is the null register. Its value is always zero and cannot be changed.
- `ip` is the instruction pointer. Its value is the address in memory of the next instruction to be executed. It is updated after the current instruction has been loaded but before it is executed. This means that the current instruction can read the `ip` register to see the address of the next instruction, or it can set the `ip` register to change which instruction is next.
- `sp` is the stack pointer. It interacts with specific instructions for pushing bytes into or popping bytes out of a particular region in memory. This makes it easier for EAL programs to implement a stack data structure.
- `rt` is the return pointer. It holds the address of whatever the next instruction would have been prior to a jump to some other instruction.

## EAL Concepts

### Instructions

With the concepts of memory and registers covered, it will now be easier to explain how EAL programs are written.

EAL programs list the instructions that the processor should execute. Each instruction represents both a type of operation to be performed, as well as the values on which to perform that operation. On each step of the simulation, the processor will read an instruction from memory, execute it, and move to the next instruction.

The type of operation that an instruction represents is indicated by a 3-to-4-letter word called the **mnemonic**. For example, the `add` mnemonic is used to add two values together. In total, there are 36 different mnemonics representing all of the operations that can be performed. The table at the bottom of this page explains what action each mnemonic represents.

Most types of operations require one or more inputs or a place to store their output. Both the inputs and outputs of an instruction are represented as **operands**. In an EAL program, operands appear after the mnemonic, are listed one after another, and are separated by commas. Each operand can be one of three things: a register, an immediate value, or a label reference.

- **Registers** are used for outputs as well as inputs that might vary each time the instruction is executed. A register operand is indicated by a dollar sign (`$`) followed by the name of the register.
- **Immediate values** are used for inputs that never change. They represent a specific number that has been hard-coded into an instruction. Immediate values can be either a decimal number (e.g., `123`) or a hexadecimal number (e.g., `0xAB12`).
- **Label references** are similar to immediate values, except their value is determined from the address of some other part of the EAL program. Label references are indicated by an at-sign (`@`) followed by the name of the label. The concept of labels is covered in more detail in a later section.

Here are some examples of each type of operand:

| Type of operand               | Example |
|-------------------------------|---------|
| Register                      | `$x0`   |
| Immediate value (decimal)     | `123`   |
| Immediate value (hexadecimal) | `0xEF`  |
| Label reference               | `@loop` |

Different mnemonics expect different numbers and kinds of operands. When you combine a mnemonic with a compatible list of operands, you get an instruction! Here are some examples:

| Instruction        | Description                                           |
|--------------------|-------------------------------------------------------|
| `nop`              | Do nothing (`nop` doesn't expect any operands).       |
| `add $x0, $x1, 12` | Add 12 to the x1 register and store the result in x0. |
| `jmp @start`       | Jump to the instruction labeled `start`.              |

## Data

EAL programs can also contain segments of raw bytes known as **data**. These parts of a program don't represent behavior, but instead are used to store useful values for instructions to interact with. A data segment is indicated by the text `.data` appearing at the start of a line. Each byte of data is then represented as a pair of hexadecimal digits with a space appearing between each pair. The following is an example of a data segment:

```
.data 01 23 45 67 89 AB CD EF
```

### Labels

Often an EAL program will need to reference instructions or data in one spot in the program from somewhere else, such as to read data or to jump to another instruction. **Labels** are a way for EAL programs to assign a name to such a location. Labels are denoted by a name followed by a colon (`:`), as in the following examples:

```
start:
  nop
values:
  .data 12 34
```

As previously mentioned, instruction operands can reference labels by name. When the program is assembled, label references are replaced with the memory address of the instruction or data to which the label was applied. For example, the following EAL instruction will jump to the memory address of whatever instruction has the `start` label:

```
  jmp @start
```

Labels can also be used to force the labeled instruction or data to begin at a particular memory address. In fact, a label can exist without a name, in which case it is only used to specify the address of the instruction or data after it.

```
values@F000:
  .data 12 34

@F010:
  .data 56 78
```

### Comments

EAL programs can be a bit cryptic. To help clarify what a program is doing or to explain why it was written the way that it was, programmers can insert **comments**. Comments are bits of text in a program that do nothing and have no impact on how the program is represented in memory, but are instead used solely to improve the readability of the program. Any text after a semicolon (`;`) until the end of the same line is treated as a comment:

```
nop ; I don't do anything!
```

## EAL Instruction Set

The following tables list all of the supported mnemonics, what kinds of operands they accept, and what actions they perform.

### Control Flow 

| Mnemonic | Operands*              | Action(s)** |
|----------|------------------------|-------------|
| `nop`    | None                   | No effect. |
| `jmp`    | `regA`                 | `rt = ip`, `ip = regA` |
|          | `immA[16]`             | `rt = ip`, `ip = immA` |
| `jmz`    | `regA, regB`           | If `regA` equals 0, then `ip = regB`; otherwise, no effect. |
|          | `regA, immA[16]`       | If `regA` equals 0, then `ip = immA`; otherwise, no effect. |

### Memory

| Mnemonic | Operands*              | Action(s)** |
|----------|------------------------|-------------|
| `set`    | `regA, regB`           | `regA = regB` |
|          | `regA, immA[16]`       | `regA = immA` |
| `ldb`    | `regA, regB`           | `regA = mem[regB]` |
|          | `regA, immA[16]`       | `regA = mem[immA]` |
| `ldw`    | `regA, regB`           | `regA = (mem[regB + 1] << 8) + mem[regB]` |
|          | `regA, immA[16]`       | `regA = (mem[immA + 1] << 8) + mem[immA]` |
| `stb`    | `regA, regB`           | `mem[regB] = regA & 0xFF` |
|          | `regA, immA[16]`       | `mem[immA] = regA & 0xFF` |
|          | `immA[8], regA`        | `mem[regA] = immA` |
|          | `immA[8], immB[16]`    | `mem[immB] = immA` |
| `stw`    | `regA, regB`           | `mem[regB] = regA & 0xFF`, `mem[regB + 1] = regA >> 8` |
|          | `regA, immA[16]`       | `mem[immA] = regA & 0xFF`, `mem[immA + 1] = regA >> 8` |
|          | `immA[16], regA`       | `mem[regA] = immA & 0xFF`, `mem[regA + 1] = immA >> 8` |
|          | `immA[16], immB[16]`   | `mem[immB] = immA & 0xFF`, `mem[immB + 1] = immA >> 8` |
| `pshb`   | `regA`                 | `mem[sp - 1] = regA & 0xFF`, `sp = sp - 1` |
| `pshw`   | `regA`                 | `mem[sp - 2] = regA & 0xFF`, `mem[sp - 1] = regA >> 8`, `sp = sp - 2` |
| `popb`   | `regA`                 | `sp = sp + 1`, `regA = mem[sp - 1]` |
| `popw`   | `regA`                 | `sp = sp + 2`, `regA = (mem[sp - 1] << 8) + mem[sp - 2]` |

### Math

| Mnemonic | Operands*              | Action(s)** |
|----------|------------------------|-------------|
| `add`    | `regA, regB, regC`     | `regA = regB + regC` |
|          | `regA, regB, immA[16]` | `regA = regB + immA` |
|          | `regA, immA[16], regB` |                      |
| `sub`    | `regA, regB, regC`     | `regA = regB - regC` |
|          | `regA, regB, immA[16]` | `regA = regB - immA` |
|          | `regA, immA[16], regB` | `regA = immA - regB` |
| `mul`    | `regA, regB, regC`     | `regA = regB * regC` |
|          | `regA, regB, immA[16]` | `regA = regB * immA` |
|          | `regA, immA[16], regB` |                      |
| `divs`   | `regA, regB, regC`     | `regA = regB (signed)   / regC (signed)` |
|          | `regA, regB, immA[16]` | `regA = regB (signed)   / immA (signed)` |
|          | `regA, immA[16], regB` | `regA = immA (signed)   / regB (signed)` |
| `divu`   | `regA, regB, regC`     | `regA = regB (unsigned) / regC (unsigned)` |
|          | `regA, regB, immA[16]` | `regA = regB (unsigned) / immA (unsigned)` |
|          | `regA, immA[16], regB` | `regA = immA (unsigned) / regB (unsigned)` |
| `rems`   | `regA, regB, regC`     | `regA = regB (signed)   % regC (signed)` |
|          | `regA, regB, immA[16]` | `regA = regB (signed)   % immA (signed)` |
|          | `regA, immA[16], regB` | `regA = immA (signed)   % regB (signed)` |
| `remu`   | `regA, regB, regC`     | `regA = regB (unsigned) % regC (unsigned)` |
|          | `regA, regB, immA[16]` | `regA = regB (unsigned) % immA (unsigned)` |
|          | `regA, immA[16], regB` | `regA = immA (unsigned) % regB (unsigned)` |

### Bitwise logic

| Mnemonic | Operands*              | Action(s)** |
|----------|------------------------|-------------|
| `and`    | `regA, regB, regC`     | `regA = regB & regC` |
|          | `regA, regB, immA[16]` | `regA = regB & immA` |
|          | `regA, immA[16], regB` |                      |
| `ior`    | `regA, regB, regC`     | `regA = regB \| regC` |
|          | `regA, regB, immA[16]` | `regA = regB \| immA` |
|          | `regA, immA[16], regB` |                       |
| `xor`    | `regA, regB, regC`     | `regA = regB ^ regC` |
|          | `regA, regB, immA[16]` | `regA = regB ^ immA` |
|          | `regA, immA[16], regB` |                      |
| `lsh`    | `regA, regB, regC`     | `regA = regB << regC (unsigned)` |
|          | `regA, regB, immA[4]`  | `regA = regB << immA (unsigned)` |
|          | `regA, immA[16], regB` | `regA = immA << regB (unsigned)` |
| `rshs`   | `regA, regB, regC`     | `regA = regB (signed)   >> regC (unsigned)` |
|          | `regA, regB, immA[4]`  | `regA = regB (signed)   >> immA (unsigned)` |
|          | `regA, immA[16], regB` | `regA = immA (signed)   >> regB (unsigned)` |
| `rshu`   | `regA, regB, regC`     | `regA = regB (unsigned) >> regC (unsigned)` |
|          | `regA, regB, immA[4]`  | `regA = regB (unsigned) >> immA (unsigned)` |
|          | `regA, immA[16], regB` | `regA = immA (unsigned) >> regB (unsigned)` |

### Comparison

| Mnemonic | Operands*              | Action(s)** |
|----------|------------------------|-------------|
| `ceq`    | `regA, regB, regC`     | If `regB` equals `regC`, `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, regB, immA[16]` | If `regB` equals `immA`, `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, immA[16], regB` | If `immA` equals `regB`, `regA = 1`; otherwise, `regA = 0`. |
| `cne`    | `regA, regB, regC`     | If `regB` does not equal `regC`, `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, regB, immA[16]` | If `regB` does not equal `immA`, `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, immA[16], regB` | If `immA` does not equal `regB`, `regA = 1`; otherwise, `regA = 0`. |
| `clts`   | `regA, regB, regC`     | If `regB` (signed) is less than `regC` (signed), `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, regB, immA[16]` | If `regB` (signed) is less than `immA` (signed), `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, immA[16], regB` | If `immA` (signed) is less than `regB` (signed), `regA = 1`; otherwise, `regA = 0`. |
| `cltu`   | `regA, regB, regC`     | If `regB` (unsigned) is less than `regC` (unsigned), `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, regB, immA[16]` | If `regB` (unsigned) is less than `immA` (unsigned), `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, immA[16], regB` | If `immA` (unsigned) is less than `regB` (unsigned), `regA = 1`; otherwise, `regA = 0`. |
| `cges`   | `regA, regB, regC`     | If `regB` (signed) is greater than or equal to `regC` (signed), `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, regB, immA[16]` | If `regB` (signed) is greater than or equal to `immA` (signed), `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, immA[16], regB` | If `immA` (signed) is greater than or equal to `regB` (signed), `regA = 1`; otherwise, `regA = 0`. |
| `cgeu`   | `regA, regB, regC`     | If `regB` (unsigned) is greater than or equal to `regC` (unsigned), `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, regB, immA[16]` | If `regB` (unsigned) is greater than or equal to `immA` (unsigned), `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, immA[16], regB` | If `immA` (unsigned) is greater than or equal to `regB` (unsigned), `regA = 1`; otherwise, `regA = 0`. |
| `cgts`   | `regA, regB, regC`     | If `regB` (signed) is greater than `regC` (signed), `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, regB, immA[16]` | If `regB` (signed) is greater than `immA` (signed), `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, immA[16], regB` | If `immA` (signed) is greater than `regB` (signed), `regA = 1`; otherwise, `regA = 0`. |
| `cgtu`   | `regA, regB, regC`     | If `regB` (unsigned) is greater than `regC` (unsigned), `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, regB, immA[16]` | If `regB` (unsigned) is greater than `immA` (unsigned), `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, immA[16], regB` | If `immA` (unsigned) is greater than `regB` (unsigned), `regA = 1`; otherwise, `regA = 0`. |
| `cles`   | `regA, regB, regC`     | If `regB` (signed) is less than or equal to `regC` (signed), `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, regB, immA[16]` | If `regB` (signed) is less than or equal to `immA` (signed), `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, immA[16], regB` | If `immA` (signed) is less than or equal to `regB` (signed), `regA = 1`; otherwise, `regA = 0`. |
| `cleu`   | `regA, regB, regC`     | If `regB` (unsigned) is less than or equal to `regC` (unsigned), `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, regB, immA[16]` | If `regB` (unsigned) is less than or equal to `immA` (unsigned), `regA = 1`; otherwise, `regA = 0`. |
|          | `regA, immA[16], regB` | If `immA` (unsigned) is less than or equal to `regB` (unsigned), `regA = 1`; otherwise, `regA = 0`. |

\* `regA`, `regB`, and `regC` represent register operands.
`immA[...]` and `immB[...]` represent immediate value operands with the number in brackets next to them denoting the maximum size of the value in bits.
Label references can only be used in place of 16-bit immediate value operands.

\*\* Operands are referenced by the same names that appear in the "Operands" column.
Any specific registers that are involved are referenced by their usual names.
`mem[...]` represents a byte in memory with the value in brackets denoting the address.
