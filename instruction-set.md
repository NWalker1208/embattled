# Version 1

| Opcode | Parameters      | Definition |
|--------|-----------------|------------|
| Control Flow                        |||
| `nop`  | N/A             | No effect |
| `jmp`  | `imm[16]`       | `ac = ip; ip = imm` |
| `jmz`  | `imm[16]`       | `if ac == 0, then ip = imm, else no effect` |
| `slp`  | `regA, imm[4]`  | `sleep for regA + imm (unsigned) cycles` (not implemented) |
| Memory                              |||
| `mov`  | `regA, regB`    | `regA = regB` |
| `ldib` | `imm[8]`        | `ac = 0:imm` |
| `ldiw` | `imm[16]`       | `ac = imm` |
| `ldmb` | `regA, imm[4]`  | `ac = 0:mem[addr]             where addr = regA + imm (signed)` |
| `ldmw` | `regA, imm[4]`  | `ac = mem[addr + 1]:mem[addr] where addr = regA + imm (signed)` |
| `stmb` | `regA, imm[4]`  | `mem[addr] = ac (low)         where addr = regA + imm (signed)` |
| `stmw` | `regA, imm[4]`  | `mem[addr + 1]:mem[addr] = ac where addr = regA + imm (signed)` |
| `pshb` | `regA`          | `mem[sp - 1] = regA (low); sp -= 1` |
| `pshw` | `regA`          | `mem[sp - 1]:mem[sp - 2] = regA; sp -= 2` |
| `popb` | `regA`          | `sp += 1; regA = 0:mem[sp - 1]` |
| `popw` | `regA`          | `sp += 2; regA = mem[sp - 1]:mem[sp - 2]; ` |
| Math and Logic                      |||
| `inc`  | `regA, imm[4]`  | `regA += imm (unsigned)` |
| `dec`  | `regA, imm[4]`  | `regA -= imm (unsigned)` |
| `add`  | `regA, regB`    | `ac = regA + regB` |
| `sub`  | `regA, regB`    | `ac = regA - regB` |
| `mul`  | `regA, regB`    | `ac = regA * regB` |
| `divs` | `regA, regB`    | `ac = regA (signed)   / regB (signed)` |
| `divu` | `regA, regB`    | `ac = regA (unsigned) / regB (unsigned)` |
| `rems` | `regA, regB`    | `ac = regA (signed)   % regB (signed)` |
| `remu` | `regA, regB`    | `ac = regA (unsigned) % regB (unsigned)` |
| `lsh`  | `regA, regB`    | `ac = regA << regB (unsigned)` |
| `rshs` | `regA, regB`    | `ac = regA (signed)   >> regB (unsigned)` |
| `rshu` | `regA, regB`    | `ac = regA (unsigned) >> regB (unsigned)` |
| `lsi`  | `regA, imm[4]`  | `ac = regA << imm (unsigned)` |
| `rsis` | `regA, imm[4]`  | `ac = regA (signed)   >> imm (unsigned)` |
| `rsiu` | `regA, imm[4]`  | `ac = regA (unsigned) >> imm (unsigned)` |
| `and`  | `regA, regB`    | `ac = regA & regB` |
| `ior`  | `regA, regB`    | `ac = regA | regB` |
| `xor`  | `regA, regB`    | `ac = regA ^ regB` |
| `ceq`  | `regA, regB`    | `ac = (regA == regB) ? 1 : 0` |
| `cne`  | `regA, regB`    | `ac = (regA != regB) ? 1 : 0` |
| `clts` | `regA, regB`    | `ac = (regA (signed)   <  regB (signed))   ? 1 : 0` |
| `cltu` | `regA, regB`    | `ac = (regA (unsigned) <  regB (unsigned)) ? 1 : 0` |
| `cges` | `regA, regB`    | `ac = (regA (signed)   >= regB (signed))   ? 1 : 0` |
| `cgeu` | `regA, regB`    | `ac = (regA (unsigned) >= regB (unsigned)) ? 1 : 0` |



# Version 2

## Notes being addressed

- "Need instruction for reading from immediate address."
- "Need way to load memory or immediate value into any register"
- "Maybe would be helpful to jump based on different register?"

## Terminology changes

- "Opcode" now refers to the actual encoding of an operator, rather than its name.
- "Mnemonic" or "opcode mnemonic" refers to the name of an opcode. Multiple opcodes that accept different kinds or numbers of parameters may share or "overload" the same mnemonic.
- As before, "instruction" refers to the combination of an opcode and some number of operands.
- "Operand" refers to a parameter of an instruction.
- "Statement" or "assembly statement" refers to a line of assembly code (label, data, instruction, etc.).

## Architectural changes

- Instructions can accept up to 3 operands instead of only 2.
- A single mnemonic can map to different opcodes such that the last read-only parameter can be either a register or an immediate value.
- Division by zero will result in the most negative/positive representable value, depending on the context.
  - If performing unsigned division, the result is the maximum unsigned short (`0xFFFF`).
  - If performing signed division and the dividend is positive, the result is the maximum signed short (`0x7FFF`).
  - If performing signed division and the dividend is negative, the result is the minimum signed short (`0x8000`).
- I considered switching to a flags-based approach for comparing values and handling errors. However, I decided to stick with the design of having multiple compare instructions, rather than having flags or multiple jump/branch instructions.
  - This prevents needing to have many extra instructions for both branching on and loading flags.
  - This also prevents needing to have multiple jump instructions in addition to multiple compare instructions.
- I considered allowing a single mnemonic to map to different opcodes that accept different sizes of immediate values (4, 8, 12, or 16-bit), but decided this didn't offer enough value to be worth the complexity. Instead, instructions will simply accept the size of immediate value that makes sense for their use case. This decision should be easy to reverse later if necessary.
