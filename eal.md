# Embattled Assembly Language

**[Example programs](https://github.com/NWalker1208/embattled/tree/master/examples)**

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

The type of operation that an instruction represents is indicated by a 3-to-4-letter word called the **mnemonic**. For example, the `add` mnemonic is used to add two values together. In total, there are 35 different mnemonics representing all of the operations that the processor can perform. The [EAL Instruction Set](#eal-instruction-set) section lists these mnemonics along with what action each mnemonic represents.

Most types of operations require one or more inputs and/or a place to store their output. Both the inputs and outputs of an instruction are represented as **operands**. In an EAL program, operands appear after the mnemonic, are listed one after another, and are separated by commas. Each operand can be one of three things: a register, an immediate value, or a label reference.

- **Registers** are used as both inputs and outputs. A register operand is indicated by a dollar sign (`$`) followed by the name of the register.
- **Immediate values** are used for inputs that never change. They represent a specific number that has been hard-coded into an instruction. Immediate values can be either a decimal number (e.g., `123`) or a hexadecimal number (e.g., `0xAB12`).
- **Label references** are similar to immediate values, except their value is determined by the address of some other part of the EAL program. Label references are indicated by an at sign (`@`) followed by the name of the label. The [Labels](#labels) section covers the concept of labels in more detail.

Here are some examples of each type of operand:

| Type of operand               | Example |
|-------------------------------|---------|
| Register                      | `$x0`   |
| Immediate value (decimal)     | `123`   |
| Immediate value (hexadecimal) | `0xEF`  |
| Label reference               | `@loop` |

Different mnemonics expect different numbers and kinds of operands. When you combine a mnemonic with a compatible list of operands, you get an instruction! Here are some examples:

| Instruction        | Description                                               |
|--------------------|-----------------------------------------------------------|
| `nop`              | Do nothing (`nop` doesn't expect any operands).           |
| `add $x0, $x1, 12` | Add 12 to the `x1` register and store the result in `x0`. |
| `jmp @start`       | Jump to the instruction labeled `start`.                  |

### Data

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

As previously mentioned, instruction operands can reference labels by name. When an EAL program is assembled, each label reference is replaced with the memory address of the instruction or data to which the label was applied. For example, the following EAL instruction will jump to the memory address of whatever instruction has the `start` label:

```
  jmp @start
```

Labels can also be used to force the labeled instruction or data to begin at a particular memory address. This is done by placing an at sign (`@`) after the label name followed by the 16-bit hexadecimal address. If the name is blank (i.e., there is nothing before the at sign), then the label only specifies the address of the instruction or data to which it is applied without assigning that address a name.

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

## Robot I/O

Each robot has three different systems with which EAL programs can interact: movement, a laser weapon, and a sensor. EAL programs interact with these systems by reading from and writing to specific pre-defined memory addresses. Those memory addresses are listed in the following tables:

### Inputs

| Name             | Address  | Description |
|------------------|----------|-------------|
| Sensor Distance  | `0xE000` | An unsigned byte that ranges between 0 and 255 to denote the relative distance to the nearest obstacle (whether that be a wall or another robot). |
| Sensor Kind      | `0xE001` | A byte indicating what kind of obstacle the sensor is detecting (0 = nothing, 1 = robot, 2 = wall). |

### Outputs

| Name             | Address  | Description |
|------------------|----------|-------------|
| Movement         | `0xF000` | A signed byte specifying how fast backwards (down to -128) or forwards (up to 127) the robot should move. |
| Rotation         | `0xF001` | A signed byte specifying how fast counter-clockwise (down to -128) or clockwise (up to 127) the robot should rotate. |
| Weapon Fire      | `0xF002` | An unsigned byte specifying whether the robot should fire its weapon (if greater than 0), and if so, how much power it should use (up to 255). |
| Sensor Direction | `0xF003` | A byte specifying the relative direction in which the robot should point its sensor (0 = forwards, 64 = right, 128/-128 = backwards, 192/-64 = left). |

## EAL Instruction Set

The following tables list all of the supported mnemonics, what kinds of operands they accept, and what actions they perform.

### Control Flow 

| Mnemonic | Operands[^1]           | Action(s)[^2] |
|----------|------------------------|---------------|
| `nop`    | None                   | No effect. |
| `jmp`    | `regA`                 | Set `rt` to `ip`, then set `ip` to `regA` (i.e., jump to the address `regA`). |
|          | `immA[16]`             | Set `rt` to `ip`, then set `ip` to `immA` (i.e., jump to the address `immA`). |
| `jmz`    | `regA, regB`           | If `regA` equals 0, set `ip` to `regB`; otherwise, no effect. |
|          | `regA, immA[16]`       | If `regA` equals 0, set `ip` to `immA`; otherwise, no effect. |

### Memory

| Mnemonic | Operands[^1]           | Action(s)[^2] |
|----------|------------------------|---------------|
| `set`    | `regA, regB`           | Set `regA` to `regB`. |
|          | `regA, immA[16]`       | Set `regA` to `immA`. |
| `ldb`    | `regA, regB`           | Load the byte at memory address `regB` into `regA`. |
|          | `regA, immA[16]`       | Load the byte at memory address `immA` into `regA`. |
| `ldw`    | `regA, regB`           | Load the 16-bit word at memory address `regB` into `regA`.[^3] |
|          | `regA, immA[16]`       | Load the 16-bit word at memory address `immA` into `regA`.[^3] |
| `stb`    | `regA, regB`           | Store the lower byte of `regA` in memory at address `regB`. |
|          | `regA, immA[16]`       | Store the lower byte of `regA` in memory at address `immA`. |
|          | `immA[8], regA`        | Store the byte `immA` in memory at address `regA`. |
|          | `immA[8], immB[16]`    | Store the byte `immA` in memory at address `immB`. |
| `stw`    | `regA, regB`           | Store the 16-bit word `regA` in memory at address `regB`.[^3] |
|          | `regA, immA[16]`       | Store the 16-bit word `regA` in memory at address `immA`.[^3] |
|          | `immA[16], regA`       | Store the 16-bit word `immA` in memory at address `regA`.[^3] |
|          | `immA[16], immB[16]`   | Store the 16-bit word `immA` in memory at address `immB`.[^3] |
| `pshb`   | `regA`                 | Store the lower byte of `regA` in memory at address `sp - 1`, then decrement `sp` by 1 (i.e., push the lower byte of `regA` onto the stack). |
| `pshw`   | `regA`                 | Store the 16-bit word `regA` in memory at address `sp - 2`, then decrement `sp` by 2 (i.e., push `regA` onto the stack).[^3] |
| `popb`   | `regA`                 | Increment `sp` by 1, then load the byte at memory address `sp - 1` into `regA` (i.e., pop a byte off of the stack and into `regA`). |
| `popw`   | `regA`                 | Increment `sp` by 2, then load the 16-bit word at memory address `sp - 2` into `regA` (i.e., pop a 16-bit word off of the stack and into `regA`).[^3] |

### Math

| Mnemonic | Operands[^1]           | Action(s)[^2] |
|----------|------------------------|---------------|
| `add`    | `regA, regB, regC`     | Set `regA` to `regB + regC`. |
|          | `regA, regB, immA[16]` | Set `regA` to `regB + immA`. |
|          | `regA, immA[16], regB` | Set `regA` to `immA + regB`. |
| `sub`    | `regA, regB, regC`     | Set `regA` to `regB - regC`. |
|          | `regA, regB, immA[16]` | Set `regA` to `regB - immA`. |
|          | `regA, immA[16], regB` | Set `regA` to `immA - regB`. |
| `mul`    | `regA, regB, regC`     | Set `regA` to `regB * regC`. |
|          | `regA, regB, immA[16]` | Set `regA` to `regB * immA`. |
|          | `regA, immA[16], regB` | Set `regA` to `immA * regB`. |
| `divs`   | `regA, regB, regC`     | Set `regA` to `regB / regC`, treating both values as signed integers. |
|          | `regA, regB, immA[16]` | Set `regA` to `regB / immA`, treating both values as signed integers. |
|          | `regA, immA[16], regB` | Set `regA` to `immA / regB`, treating both values as signed integers. |
| `divu`   | `regA, regB, regC`     | Set `regA` to `regB / regC`, treating both values as unsigned integers. |
|          | `regA, regB, immA[16]` | Set `regA` to `regB / immA`, treating both values as unsigned integers. |
|          | `regA, immA[16], regB` | Set `regA` to `immA / regB`, treating both values as unsigned integers. |
| `rems`   | `regA, regB, regC`     | Set `regA` to the remainder of `regB` divided by `regC`, treating both values as signed integers. |
|          | `regA, regB, immA[16]` | Set `regA` to the remainder of `regB` divided by `immA`, treating both values as signed integers. |
|          | `regA, immA[16], regB` | Set `regA` to the remainder of `immA` divided by `regB`, treating both values as signed integers. |
| `remu`   | `regA, regB, regC`     | Set `regA` to the remainder of `regB` divided by `regC`, treating both values as unsigned integers. |
|          | `regA, regB, immA[16]` | Set `regA` to the remainder of `regB` divided by `immA`, treating both values as unsigned integers. |
|          | `regA, immA[16], regB` | Set `regA` to the remainder of `immA` divided by `regB`, treating both values as unsigned integers. |

### Bitwise logic

| Mnemonic | Operands[^1]           | Action(s)[^2] |
|----------|------------------------|---------------|
| `and`    | `regA, regB, regC`     | Set `regA` to the bitwise AND of `regB` and `regC`. |
|          | `regA, regB, immA[16]` | Set `regA` to the bitwise AND of `regB` and `immA`. |
|          | `regA, immA[16], regB` | Set `regA` to the bitwise AND of `immA` and `regB`. |
| `ior`    | `regA, regB, regC`     | Set `regA` to the bitwise inclusive OR of `regB` and `regC`. |
|          | `regA, regB, immA[16]` | Set `regA` to the bitwise inclusive OR of `regB` and `immA`. |
|          | `regA, immA[16], regB` | Set `regA` to the bitwise inclusive OR of `immA` and `regB`. |
| `xor`    | `regA, regB, regC`     | Set `regA` to the bitwise exclusive OR of `regB` and `regC`. |
|          | `regA, regB, immA[16]` | Set `regA` to the bitwise exclusive OR of `regB` and `immA`. |
|          | `regA, immA[16], regB` | Set `regA` to the bitwise exclusive OR of `immA` and `regB`. |
| `lsh`    | `regA, regB, regC`     | Set `regA` to `regB` shifted left by `regC` bits. |
|          | `regA, regB, immA[4]`  | Set `regA` to `regB` shifted left by `immA` bits. |
|          | `regA, immA[16], regB` | Set `regA` to `immA` shifted left by `regB` bits. |
| `rshs`   | `regA, regB, regC`     | Set `regA` to `regB` shifted right by `regC` bits, treating `regB` as a signed integer (i.e., sign-extend). |
|          | `regA, regB, immA[4]`  | Set `regA` to `regB` shifted right by `immA` bits, treating `regB` as a signed integer (i.e., sign-extend). |
|          | `regA, immA[16], regB` | Set `regA` to `immA` shifted right by `regB` bits, treating `immA` as a signed integer (i.e., sign-extend). |
| `rshu`   | `regA, regB, regC`     | Set `regA` to `regB` shifted right by `regC` bits, treating `regB` as an unsigned integer (i.e., zero-extend). |
|          | `regA, regB, immA[4]`  | Set `regA` to `regB` shifted right by `immA` bits, treating `regB` as an unsigned integer (i.e., zero-extend). |
|          | `regA, immA[16], regB` | Set `regA` to `immA` shifted right by `regB` bits, treating `immA` as an unsigned integer (i.e., zero-extend). |

### Comparison

| Mnemonic | Operands[^1]           | Action(s)[^2] |
|----------|------------------------|---------------|
| `ceq`    | `regA, regB, regC`     | If `regB` equals `regC`, set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, regB, immA[16]` | If `regB` equals `immA`, set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, immA[16], regB` | If `immA` equals `regB`, set `regA` to 1; otherwise, set `regA` to 0. |
| `cne`    | `regA, regB, regC`     | If `regB` does not equal `regC`, set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, regB, immA[16]` | If `regB` does not equal `immA`, set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, immA[16], regB` | If `immA` does not equal `regB`, set `regA` to 1; otherwise, set `regA` to 0. |
| `clts`   | `regA, regB, regC`     | If `regB` is less than `regC` (treating both as signed integers), set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, regB, immA[16]` | If `regB` is less than `immA` (treating both as signed integers), set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, immA[16], regB` | If `immA` is less than `regB` (treating both as signed integers), set `regA` to 1; otherwise, set `regA` to 0. |
| `cltu`   | `regA, regB, regC`     | If `regB` is less than `regC` (treating both as unsigned integers), set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, regB, immA[16]` | If `regB` is less than `immA` (treating both as unsigned integers), set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, immA[16], regB` | If `immA` is less than `regB` (treating both as unsigned integers), set `regA` to 1; otherwise, set `regA` to 0. |
| `cges`   | `regA, regB, regC`     | If `regB` is greater than or equal to `regC` (treating both as signed integers), set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, regB, immA[16]` | If `regB` is greater than or equal to `immA` (treating both as signed integers), set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, immA[16], regB` | If `immA` is greater than or equal to `regB` (treating both as signed integers), set `regA` to 1; otherwise, set `regA` to 0. |
| `cgeu`   | `regA, regB, regC`     | If `regB` is greater than or equal to `regC` (treating both as unsigned integers), set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, regB, immA[16]` | If `regB` is greater than or equal to `immA` (treating both as unsigned integers), set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, immA[16], regB` | If `immA` is greater than or equal to `regB` (treating both as unsigned integers), set `regA` to 1; otherwise, set `regA` to 0. |
| `cgts`   | `regA, regB, regC`     | If `regB` is greater than `regC` (treating both as signed integers), set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, regB, immA[16]` | If `regB` is greater than `immA` (treating both as signed integers), set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, immA[16], regB` | If `immA` is greater than `regB` (treating both as signed integers), set `regA` to 1; otherwise, set `regA` to 0. |
| `cgtu`   | `regA, regB, regC`     | If `regB` is greater than `regC` (treating both as unsigned integers), set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, regB, immA[16]` | If `regB` is greater than `immA` (treating both as unsigned integers), set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, immA[16], regB` | If `immA` is greater than `regB` (treating both as unsigned integers), set `regA` to 1; otherwise, set `regA` to 0. |
| `cles`   | `regA, regB, regC`     | If `regB` is less than or equal to `regC` (treating both as signed integers), set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, regB, immA[16]` | If `regB` is less than or equal to `immA` (treating both as signed integers), set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, immA[16], regB` | If `immA` is less than or equal to `regB` (treating both as signed integers), set `regA` to 1; otherwise, set `regA` to 0. |
| `cleu`   | `regA, regB, regC`     | If `regB` is less than or equal to `regC` (treating both as unsigned integers), set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, regB, immA[16]` | If `regB` is less than or equal to `immA` (treating both as unsigned integers), set `regA` to 1; otherwise, set `regA` to 0. |
|          | `regA, immA[16], regB` | If `immA` is less than or equal to `regB` (treating both as unsigned integers), set `regA` to 1; otherwise, set `regA` to 0. |

[^1]: `regA`, `regB`, and `regC` represent register operands.
`immA[...]` and `immB[...]` represent immediate value operands with the number in brackets next to them denoting the maximum size of the value in bits.
Label references can only be used in place of 16-bit immediate value operands.

[^2]: Operands are referenced by the same names that appear in the "Operands" column.
Any specific registers that are involved are referenced by their usual names.

[^3]: All two-byte (16-bit) values are read from and written to memory with the least-significant byte appearing first (little-endian order).
