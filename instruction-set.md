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
- A single mnemonic can map to different opcodes such that the one or more read-only parameter can be either a register or an immediate value.
- Division by zero will result in the most negative/positive representable value, depending on the context.
  - If the dividend is zero, the result is zero (`0x0000`).
  - If performing unsigned division and the dividend is not zero, the result is the maximum unsigned short (`0xFFFF`).
  - If performing signed division and the dividend is positive, the result is the maximum signed short (`0x7FFF`).
  - If performing signed division and the dividend is negative, the result is the minimum signed short (`0x8000`).
  - The remainder when dividing by zero is always equal to the dividend to maintain the invariant that `y * (x / y) + (x % y) = x`.
- I considered switching to a flags-based approach for comparing values and handling errors. However, I decided to stick with the design of having multiple compare instructions, rather than having flags or multiple jump/branch instructions.
  - This prevents needing to have many extra instructions for both branching on and loading flags.
  - This also prevents needing to have multiple jump instructions in addition to multiple compare instructions.
- I considered allowing a single mnemonic to map to different opcodes that accept different sizes of immediate values (4, 8, 12, or 16-bit), but decided this didn't offer enough value to be worth the complexity. Instead, instructions will simply accept the size of immediate value that makes sense for their use case. This decision should be easy to reverse later if necessary.
- For the sake of efficient storage, opcodes will occupy either 8 or 12 bits.
- The `ac` (accumulator) register is replaced with the `rt` (return) register.
- I considered having the left-shift and right-shift opcodes treat register B (the number of bits to shift) as signed, but this would require having signed and unsigned variants of the left-shift opcodes, which would be meaningless when the second operand is an immediate value.

## Instructions

| Mnemonic | Opcode    | Parameters             | Definition |
|----------|-----------|------------------------|------------|
| Control Flow                                            ||||
| `nop`    | `nop`     | N/A                    | No effect |
| `jmp`    | `jmp_r`   | `regA`                 | `rt = ip; ip = regA` |
|          | `jmp_i`   | `immA[16]`             | `rt = ip; ip = immA` |
| `jmz`    | `jmz_r`   | `regA, regB`           | `if regA == 0, then ip = regB, else no effect` |
|          | `jmz_i`   | `regA, immA[16]`       | `if regA == 0, then ip = immA, else no effect` |
| `slp`    | `slp_r`   | `regA`                 | `sleep for regA cycles` |
|          | `slp_i`   | `immA[16]`             | `sleep for immA cycles` |
| Memory                                                  ||||
| `set`    | `set_r`   | `regA, regB`           | `regA = regB` |
|          | `set_i`   | `regA, immA[16]`       | `regA = immA` |
| `ldb`    | `ldb_r`   | `regA, regB`           | `regA = 0:mem[regB]` |
|          | `ldb_i`   | `regA, immA[16]`       | `regA = 0:mem[immA]` |
| `ldw`    | `ldw_r`   | `regA, regB`           | `regA = mem[regB + 1]:mem[regB]` |
|          | `ldw_i`   | `regA, immA[16]`       | `regA = mem[immA + 1]:mem[immA]` |
| `stb`    | `stb_rr`  | `regA, regB`           | `mem[regB] = regA[low]` |
|          | `stb_ri`  | `regA, immA[16]`       | `mem[immA] = regA[low]` |
|          | `stb_ir`  | `immA[8], regA`        | `mem[regA] = immA` |
|          | `stb_ii`  | `immA[8], immB[16]`    | `mem[immB] = immA` |
| `stw`    | `stw_rr`  | `regA, regB`           | `mem[regB + 1]:mem[regB] = regA` |
|          | `stw_ri`  | `regA, immA[16]`       | `mem[immA + 1]:mem[immA] = regA` |
|          | `stw_ir`  | `immA[16], regA`       | `mem[regA + 1]:mem[regA] = immA` |
|          | `stw_ii`  | `immA[16], immB[16]`   | `mem[immB + 1]:mem[immB] = immA` |
| `pshb`   | `pshb`    | `regA`                 | `mem[sp - 1] = regA[low]; sp -= 1` |
| `pshw`   | `pshw`    | `regA`                 | `mem[sp - 1]:mem[sp - 2] = regA; sp -= 2` |
| `popb`   | `popb`    | `regA`                 | `sp += 1; regA = 0:mem[sp - 1]` |
| `popw`   | `popw`    | `regA`                 | `sp += 2; regA = mem[sp - 1]:mem[sp - 2]; ` |
| Math                                                    ||||
| `add`    | `add_r`   | `regA, regB, regC`     | `regA = regB + regC` |
|          | `add_i`   | `regA, regB, immA[16]` | `regA = regB + immA` |
|          |           | `regA, immA[16], regB` |                      |
| `sub`    | `sub_rr`  | `regA, regB, regC`     | `regA = regB - regC` |
|          | `sub_ri`  | `regA, regB, immA[16]` | `regA = regB - immA` |
|          | `sub_ir`  | `regA, immA[16], regB` | `regA = immA - regB` |
| `mul`    | `mul_r`   | `regA, regB, regC`     | `regA = regB * regC` |
|          | `mul_i`   | `regA, regB, immA[16]` | `regA = regB * immA` |
|          |           | `regA, immA[16], regB` |                      |
| `divs`   | `divs_rr` | `regA, regB, regC`     | `regA = regB (signed)   / regC (signed)` |
|          | `divs_ri` | `regA, regB, immA[16]` | `regA = regB (signed)   / immA (signed)` |
|          | `divs_ir` | `regA, immA[16], regB` | `regA = immA (signed)   / regB (signed)` |
| `divu`   | `divu_rr` | `regA, regB, regC`     | `regA = regB (unsigned) / regC (unsigned)` |
|          | `divu_ri` | `regA, regB, immA[16]` | `regA = regB (unsigned) / immA (unsigned)` |
|          | `divu_ir` | `regA, immA[16], regB` | `regA = immA (unsigned) / regB (unsigned)` |
| `rems`   | `rems_rr` | `regA, regB, regC`     | `regA = regB (signed)   % regC (signed)` |
|          | `rems_ri` | `regA, regB, immA[16]` | `regA = regB (signed)   % immA (signed)` |
|          | `rems_ir` | `regA, immA[16], regB` | `regA = immA (signed)   % regB (signed)` |
| `remu`   | `remu_rr` | `regA, regB, regC`     | `regA = regB (unsigned) % regC (unsigned)` |
|          | `remu_ri` | `regA, regB, immA[16]` | `regA = regB (unsigned) % immA (unsigned)` |
|          | `remu_ir` | `regA, immA[16], regB` | `regA = immA (unsigned) % regB (unsigned)` |
| Bitwise logic                                           ||||
| `and`    | `and_r`   | `regA, regB, regC`     | `regA = regB & regC` |
|          | `and_i`   | `regA, regB, immA[16]` | `regA = regB & immA` |
|          |           | `regA, immA[16], regB` |                      |
| `ior`    | `ior_r`   | `regA, regB, regC`     | `regA = regB \| regC` |
|          | `ior_i`   | `regA, regB, immA[16]` | `regA = regB \| immA` |
|          |           | `regA, immA[16], regB` |                       |
| `xor`    | `xor_r`   | `regA, regB, regC`     | `regA = regB ^ regC` |
|          | `xor_i`   | `regA, regB, immA[16]` | `regA = regB ^ immA` |
|          |           | `regA, immA[16], regB` |                      |
| `lsh`    | `lsh_rr`  | `regA, regB, regC`     | `regA = regB << regC (unsigned)` |
|          | `lsh_ri`  | `regA, regB, immA[4]`  | `regA = regB << immA (unsigned)` |
|          | `lsh_ir`  | `regA, immA[16], regB` | `regA = immA << regB (unsigned)` |
| `rshs`   | `rshs_rr` | `regA, regB, regC`     | `regA = regB (signed)   >> regC (unsigned)` |
|          | `rshs_ri` | `regA, regB, immA[4]`  | `regA = regB (signed)   >> immA (unsigned)` |
|          | `rshs_ir` | `regA, immA[16], regB` | `regA = immA (signed)   >> regB (unsigned)` |
| `rshu`   | `rshu_rr` | `regA, regB, regC`     | `regA = regB (unsigned) >> regC (unsigned)` |
|          | `rshu_ri` | `regA, regB, immA[4]`  | `regA = regB (unsigned) >> immA (unsigned)` |
|          | `rshu_ir` | `regA, immA[16], regB` | `regA = immA (unsigned) >> regB (unsigned)` |
| Comparison                                              ||||
| `ceq`    | `ceq_r`   | `regA, regB, regC`     | `regA = (regB == regC) ? 1 : 0` |
|          | `ceq_i`   | `regA, regB, immA[16]` | `regA = (regB == immA) ? 1 : 0` |
|          |           | `regA, immA[16], regB` |                                 |
| `cne`    | `cne_r`   | `regA, regB, regC`     | `regA = (regB != regC) ? 1 : 0` |
|          | `cne_i`   | `regA, regB, immA[16]` | `regA = (regB != immA) ? 1 : 0` |
|          |           | `regA, immA[16], regB` |                                 |
| `clts`   | `clts_rr` | `regA, regB, regC`     | `regA = (regB (signed)   <  regC (signed))   ? 1 : 0` |
|          | `clts_ri` | `regA, regB, immA[16]` | `regA = (regB (signed)   <  immA (signed))   ? 1 : 0` |
|          | `clts_ir` | `regA, immA[16], regB` | `regA = (immA (signed)   <  regB (signed))   ? 1 : 0` |
| `cltu`   | `cltu_rr` | `regA, regB, regC`     | `regA = (regB (unsigned) <  regC (unsigned)) ? 1 : 0` |
|          | `cltu_ri` | `regA, regB, immA[16]` | `regA = (regB (unsigned) <  immA (unsigned)) ? 1 : 0` |
|          | `cltu_ir` | `regA, immA[16], regB` | `regA = (immA (unsigned) <  regB (unsigned)) ? 1 : 0` |
| `cges`   | `cges_rr` | `regA, regB, regC`     | `regA = (regB (signed)   >= regC (signed))   ? 1 : 0` |
|          | `cges_ri` | `regA, regB, immA[16]` | `regA = (regB (signed)   >= immA (signed))   ? 1 : 0` |
|          | `cges_ir` | `regA, immA[16], regB` | `regA = (immA (signed)   >= regB (signed))   ? 1 : 0` |
| `cgeu`   | `cgeu_rr` | `regA, regB, regC`     | `regA = (regB (unsigned) >= regC (unsigned)) ? 1 : 0` |
|          | `cgeu_ri` | `regA, regB, immA[16]` | `regA = (regB (unsigned) >= immA (unsigned)) ? 1 : 0` |
|          | `cgeu_ir` | `regA, immA[16], regB` | `regA = (immA (unsigned) >= regB (unsigned)) ? 1 : 0` |
| `cgts`*  | `clts_rr` | `regA, regC, regB`     | See definition above. |
|          | `clts_ir` | `regA, regB, immA[16]` |                       |
|          | `clts_ri` | `regA, immA[16], regB` |                       |
| `cgtu`*  | `cltu_rr` | `regA, regC, regB`     | See definition above. |
|          | `cltu_ir` | `regA, regB, immA[16]` |                       |
|          | `cltu_ri` | `regA, immA[16], regB` |                       |
| `cles`*  | `cges_rr` | `regA, regC, regB`     | See definition above. |
|          | `cges_ir` | `regA, regB, immA[16]` |                       |
|          | `cges_ri` | `regA, immA[16], regB` |                       |
| `cleu`*  | `cgeu_rr` | `regA, regC, regB`     | See definition above. |
|          | `cgeu_ir` | `regA, regB, immA[16]` |                       |
|          | `cgeu_ri` | `regA, immA[16], regB` |                       |

*The intructions `cgts`, `cgtu`, `cles`, and `cleu` reuse the opcodes for their complementary instructions (`clts`, `cltu`, `cges`, and `cgeu` respectively).
