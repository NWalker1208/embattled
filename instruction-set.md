# Current Instructions

| Opcode | Parameters      | Definition |
|--------|-----------------|------------|
| Control Flow                        |||
| `nop`  | N/A             | No effect
| `jmp`  | `imm[16]`       | `ac = ip; ip = imm`
| `jmz`  | `imm[16]`       | `if ac == 0, then ip = imm, else no effect`
| `slp`  | `regA, imm[4]`  | `sleep for regA + imm (unsigned) cycles` (not implemented)
| Memory                              |||
| `mov`  | `regA, regB`    | `regA = regB`
| `ldib` | `imm[8]`        | `ac = 0:imm`
| `ldiw` | `imm[16]`       | `ac = imm`
| `ldmb` | `regA, imm[4]`  | `ac = 0:mem[addr]             where addr = regA + imm (signed)`
| `ldmw` | `regA, imm[4]`  | `ac = mem[addr + 1]:mem[addr] where addr = regA + imm (signed)`
| `stmb` | `regA, imm[4]`  | `mem[addr] = ac (low)         where addr = regA + imm (signed)`
| `stmw` | `regA, imm[4]`  | `mem[addr + 1]:mem[addr] = ac where addr = regA + imm (signed)`
| `pshb` | `regA`          | `mem[sp - 1] = regA (low); sp -= 1`
| `pshw` | `regA`          | `mem[sp - 1]:mem[sp - 2] = regA; sp -= 2`
| `popb` | `regA`          | `sp += 1; regA = 0:mem[sp - 1]`
| `popw` | `regA`          | `sp += 2; regA = mem[sp - 1]:mem[sp - 2]; `
| Math and Logic                      |||
| `inc`  | `regA, imm[4]`  | `regA += imm (unsigned)`
| `dec`  | `regA, imm[4]`  | `regA -= imm (unsigned)`
| `add`  | `regA, regB`    | `ac = regA + regB`
| `sub`  | `regA, regB`    | `ac = regA - regB`
| `mul`  | `regA, regB`    | `ac = regA * regB`
| `divs` | `regA, regB`    | `ac = regA (signed)   / regB (signed)`
| `divu` | `regA, regB`    | `ac = regA (unsigned) / regB (unsigned)`
| `rems` | `regA, regB`    | `ac = regA (signed)   % regB (signed)`
| `remu` | `regA, regB`    | `ac = regA (unsigned) % regB (unsigned)`
| `lsh`  | `regA, regB`    | `ac = regA << regB (unsigned)`
| `rshs` | `regA, regB`    | `ac = regA (signed)   >> regB (unsigned)`
| `rshu` | `regA, regB`    | `ac = regA (unsigned) >> regB (unsigned)`
| `lsi`  | `regA, imm[4]`  | `ac = regA << imm (unsigned)`
| `rsis` | `regA, imm[4]`  | `ac = regA (signed)   >> imm (unsigned)`
| `rsiu` | `regA, imm[4]`  | `ac = regA (unsigned) >> imm (unsigned)`
| `and`  | `regA, regB`    | `ac = regA & regB`
| `ior`  | `regA, regB`    | `ac = regA | regB`
| `xor`  | `regA, regB`    | `ac = regA ^ regB`
| `ceq`  | `regA, regB`    | `ac = (regA == regB) ? 1 : 0`
| `cne`  | `regA, regB`    | `ac = (regA != regB) ? 1 : 0`
| `clts` | `regA, regB`    | `ac = (regA (signed)   <  regB (signed))   ? 1 : 0`
| `cltu` | `regA, regB`    | `ac = (regA (unsigned) <  regB (unsigned)) ? 1 : 0`
| `cges` | `regA, regB`    | `ac = (regA (signed)   >= regB (signed))   ? 1 : 0`
| `cgeu` | `regA, regB`    | `ac = (regA (unsigned) >= regB (unsigned)) ? 1 : 0`
