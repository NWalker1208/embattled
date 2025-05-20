#include "processor/opcode.h"
#include <stddef.h>

#pragma region Opcode execute function declarations

void execute_nop(OpcodeExecuteArguments args);

void execute_jmp_r(OpcodeExecuteArguments args);
void execute_jmp_i(OpcodeExecuteArguments args);

void execute_jmz_r(OpcodeExecuteArguments args);
void execute_jmz_i(OpcodeExecuteArguments args);

void execute_slp_r(OpcodeExecuteArguments args);
void execute_slp_i(OpcodeExecuteArguments args);

void execute_set_r(OpcodeExecuteArguments args);
void execute_set_i(OpcodeExecuteArguments args);

void execute_ldb_r(OpcodeExecuteArguments args);
void execute_ldb_i(OpcodeExecuteArguments args);

void execute_ldw_r(OpcodeExecuteArguments args);
void execute_ldw_i(OpcodeExecuteArguments args);

void execute_stb_rr(OpcodeExecuteArguments args);
void execute_stb_ri(OpcodeExecuteArguments args);
void execute_stb_ir(OpcodeExecuteArguments args);
void execute_stb_ii(OpcodeExecuteArguments args);

void execute_stw_rr(OpcodeExecuteArguments args);
void execute_stw_ri(OpcodeExecuteArguments args);
void execute_stw_ir(OpcodeExecuteArguments args);
void execute_stw_ii(OpcodeExecuteArguments args);

void execute_pshb(OpcodeExecuteArguments args);

void execute_pshw(OpcodeExecuteArguments args);

void execute_popb(OpcodeExecuteArguments args);

void execute_popw(OpcodeExecuteArguments args);

void execute_add_r(OpcodeExecuteArguments args);
void execute_add_i(OpcodeExecuteArguments args);

void execute_sub_rr(OpcodeExecuteArguments args);
void execute_sub_ri(OpcodeExecuteArguments args);
void execute_sub_ir(OpcodeExecuteArguments args);

void execute_mul_r(OpcodeExecuteArguments args);
void execute_mul_i(OpcodeExecuteArguments args);

void execute_divs_rr(OpcodeExecuteArguments args);
void execute_divs_ri(OpcodeExecuteArguments args);
void execute_divs_ir(OpcodeExecuteArguments args);

void execute_divu_rr(OpcodeExecuteArguments args);
void execute_divu_ri(OpcodeExecuteArguments args);
void execute_divu_ir(OpcodeExecuteArguments args);

void execute_rems_rr(OpcodeExecuteArguments args);
void execute_rems_ri(OpcodeExecuteArguments args);
void execute_rems_ir(OpcodeExecuteArguments args);

void execute_remu_rr(OpcodeExecuteArguments args);
void execute_remu_ri(OpcodeExecuteArguments args);
void execute_remu_ir(OpcodeExecuteArguments args);

void execute_and_r(OpcodeExecuteArguments args);
void execute_and_i(OpcodeExecuteArguments args);

void execute_ior_r(OpcodeExecuteArguments args);
void execute_ior_i(OpcodeExecuteArguments args);

void execute_xor_r(OpcodeExecuteArguments args);
void execute_xor_i(OpcodeExecuteArguments args);

void execute_lsh_rr(OpcodeExecuteArguments args);
void execute_lsh_ri(OpcodeExecuteArguments args);
void execute_lsh_ir(OpcodeExecuteArguments args);

void execute_rshs_rr(OpcodeExecuteArguments args);
void execute_rshs_ri(OpcodeExecuteArguments args);
void execute_rshs_ir(OpcodeExecuteArguments args);

void execute_rshu_rr(OpcodeExecuteArguments args);
void execute_rshu_ri(OpcodeExecuteArguments args);
void execute_rshu_ir(OpcodeExecuteArguments args);

void execute_ceq_r(OpcodeExecuteArguments args);
void execute_ceq_i(OpcodeExecuteArguments args);

void execute_cne_r(OpcodeExecuteArguments args);
void execute_cne_i(OpcodeExecuteArguments args);

void execute_clts_rr(OpcodeExecuteArguments args);
void execute_clts_ri(OpcodeExecuteArguments args);
void execute_clts_ir(OpcodeExecuteArguments args);

void execute_cltu_rr(OpcodeExecuteArguments args);
void execute_cltu_ri(OpcodeExecuteArguments args);
void execute_cltu_ir(OpcodeExecuteArguments args);

void execute_cges_rr(OpcodeExecuteArguments args);
void execute_cges_ri(OpcodeExecuteArguments args);
void execute_cges_ir(OpcodeExecuteArguments args);

void execute_cgeu_rr(OpcodeExecuteArguments args);
void execute_cgeu_ri(OpcodeExecuteArguments args);
void execute_cgeu_ir(OpcodeExecuteArguments args);

#pragma endregion

const OpcodeInfo OPCODE_INFO[OPCODE_COUNT] = {
  { .identifier = "nop", .layout = INSTRUCTION_LAYOUT_NONE, .execute = execute_nop, },

  { .identifier = "jmp_r", .layout = INSTRUCTION_LAYOUT_REGA, .execute = execute_jmp_r, },
  { .identifier = "jmp_i", .layout = INSTRUCTION_LAYOUT_IMMA16, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_jmp_i, },
  
  { .identifier = "jmz_r", .layout = INSTRUCTION_LAYOUT_REGA_REGB, .execute = execute_jmz_r, },
  { .identifier = "jmz_i", .layout = INSTRUCTION_LAYOUT_REGA_IMMA16, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_jmz_i, },
  
  { .identifier = "slp_r", .layout = INSTRUCTION_LAYOUT_REGA, .execute = execute_slp_r, },
  { .identifier = "slp_i", .layout = INSTRUCTION_LAYOUT_IMMA16, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_slp_i, },
  
  { .identifier = "set_r", .layout = INSTRUCTION_LAYOUT_REGA_REGB, .execute = execute_set_r, },
  { .identifier = "set_i", .layout = INSTRUCTION_LAYOUT_REGA_IMMA16, .execute = execute_set_i, },
  
  { .identifier = "ldb_r", .layout = INSTRUCTION_LAYOUT_REGA_REGB, .execute = execute_ldb_r, },
  { .identifier = "ldb_i", .layout = INSTRUCTION_LAYOUT_REGA_IMMA16, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_ldb_i, },
  
  { .identifier = "ldw_r", .layout = INSTRUCTION_LAYOUT_REGA_REGB, .execute = execute_ldw_r, },
  { .identifier = "ldw_i", .layout = INSTRUCTION_LAYOUT_REGA_IMMA16, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_ldw_i, },
  
  { .identifier = "stb_rr", .layout = INSTRUCTION_LAYOUT_REGA_REGB, .execute = execute_stb_rr, },
  { .identifier = "stb_ri", .layout = INSTRUCTION_LAYOUT_REGA_IMMA16, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_stb_ri, },
  { .identifier = "stb_ir", .layout = INSTRUCTION_LAYOUT_REGA_IMMA8, .execute = execute_stb_ir, },
  { .identifier = "stb_ii", .layout = INSTRUCTION_LAYOUT_IMMA8_IMMB16, .execute = execute_stb_ii, },
  
  { .identifier = "stw_rr", .layout = INSTRUCTION_LAYOUT_REGA_REGB, .execute = execute_stw_rr, },
  { .identifier = "stw_ri", .layout = INSTRUCTION_LAYOUT_REGA_IMMA16, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_stw_ri, },
  { .identifier = "stw_ir", .layout = INSTRUCTION_LAYOUT_REGA_IMMA16, .execute = execute_stw_ir, },
  { .identifier = "stw_ii", .layout = INSTRUCTION_LAYOUT_IMMA16_IMMB16, .execute = execute_stw_ii, },
  
  { .identifier = "pshb", .layout = INSTRUCTION_LAYOUT_REGA, .execute = execute_pshb, },
  
  { .identifier = "pshw", .layout = INSTRUCTION_LAYOUT_REGA, .execute = execute_pshw, },
  
  { .identifier = "popb", .layout = INSTRUCTION_LAYOUT_REGA, .execute = execute_popb, },
  
  { .identifier = "popw", .layout = INSTRUCTION_LAYOUT_REGA, .execute = execute_popw, },
  
  { .identifier = "add_r", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_add_r, },
  { .identifier = "add_i", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .execute = execute_add_i, },
  
  { .identifier = "sub_rr", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_sub_rr, },
  { .identifier = "sub_ri", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .execute = execute_sub_ri, },
  { .identifier = "sub_ir", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .execute = execute_sub_ir, },
  
  { .identifier = "mul_r", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_mul_r, },
  { .identifier = "mul_i", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .execute = execute_mul_i, },
  
  { .identifier = "divs_rr", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_divs_rr, },
  { .identifier = "divs_ri", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .immASignedness = SIGNEDNESS_SIGNED, .execute = execute_divs_ri, },
  { .identifier = "divs_ir", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .immASignedness = SIGNEDNESS_SIGNED, .execute = execute_divs_ir, },
  
  { .identifier = "divu_rr", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_divu_rr, },
  { .identifier = "divu_ri", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_divu_ri, },
  { .identifier = "divu_ir", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_divu_ir, },
  
  { .identifier = "rems_rr", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_rems_rr, },
  { .identifier = "rems_ri", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .immASignedness = SIGNEDNESS_SIGNED, .execute = execute_rems_ri, },
  { .identifier = "rems_ir", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .immASignedness = SIGNEDNESS_SIGNED, .execute = execute_rems_ir, },
  
  { .identifier = "remu_rr", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_remu_rr, },
  { .identifier = "remu_ri", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_remu_ri, },
  { .identifier = "remu_ir", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_remu_ir, },
  
  { .identifier = "and_r", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_and_r, },
  { .identifier = "and_i", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .execute = execute_and_i, },
  
  { .identifier = "ior_r", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_ior_r, },
  { .identifier = "ior_i", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .execute = execute_ior_i, },
  
  { .identifier = "xor_r", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_xor_r, },
  { .identifier = "xor_i", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .execute = execute_xor_i, },
  
  { .identifier = "lsh_rr", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_lsh_rr, },
  { .identifier = "lsh_ri", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA4, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_lsh_ri, },
  { .identifier = "lsh_ir", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .execute = execute_lsh_ir, },
  
  { .identifier = "rshs_rr", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_rshs_rr, },
  { .identifier = "rshs_ri", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA4, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_rshs_ri, },
  { .identifier = "rshs_ir", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .immASignedness = SIGNEDNESS_SIGNED, .execute = execute_rshs_ir, },
  
  { .identifier = "rshu_rr", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_rshu_rr, },
  { .identifier = "rshu_ri", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA4, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_rshu_ri, },
  { .identifier = "rshu_ir", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_rshu_ir, },
  
  { .identifier = "ceq_r", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_ceq_r, },
  { .identifier = "ceq_i", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .execute = execute_ceq_i, },
  
  { .identifier = "cne_r", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_cne_r, },
  { .identifier = "cne_i", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .execute = execute_cne_i, },
  
  { .identifier = "clts_rr", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_clts_rr, },
  { .identifier = "clts_ri", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .immASignedness = SIGNEDNESS_SIGNED, .execute = execute_clts_ri, },
  { .identifier = "clts_ir", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .immASignedness = SIGNEDNESS_SIGNED, .execute = execute_clts_ir, },
  
  { .identifier = "cltu_rr", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_cltu_rr, },
  { .identifier = "cltu_ri", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_cltu_ri, },
  { .identifier = "cltu_ir", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_cltu_ir, },
  
  { .identifier = "cges_rr", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_cges_rr, },
  { .identifier = "cges_ri", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .immASignedness = SIGNEDNESS_SIGNED, .execute = execute_cges_ri, },
  { .identifier = "cges_ir", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .immASignedness = SIGNEDNESS_SIGNED, .execute = execute_cges_ir, },
  
  { .identifier = "cgeu_rr", .layout = INSTRUCTION_LAYOUT_REGA_REGB_REGC, .execute = execute_cgeu_rr, },
  { .identifier = "cgeu_ri", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_cgeu_ri, },
  { .identifier = "cgeu_ir", .layout = INSTRUCTION_LAYOUT_REGA_REGB_IMMA16, .immASignedness = SIGNEDNESS_UNSIGNED, .execute = execute_cgeu_ir, },
};

const OpcodeInfo* getOpcodeInfo(Opcode opcode) {
  if (opcode >= 0 && opcode < OPCODE_COUNT) {
    return &OPCODE_INFO[opcode];
  } else {
    return NULL;
  }
}

Opcode byteToOpcode(uint8_t byte) {
  if (byte < OPCODE_COUNT) {
    return byte;
  } else {
    return OPCODE_NOP;
  }
}

#pragma region Opcode execute function implementations
// TODO: Implement these stubs
void execute_nop(OpcodeExecuteArguments args) { (void)args; }

void execute_jmp_r(OpcodeExecuteArguments args) { args.process->registers.rt = args.process->registers.ip; args.process->registers.ip = *args.registerAPtr; }
void execute_jmp_i(OpcodeExecuteArguments args) { args.process->registers.rt = args.process->registers.ip; args.process->registers.ip = args.immediateA.u16; }

void execute_jmz_r(OpcodeExecuteArguments args) { if (*args.registerAPtr == 0) { args.process->registers.ip = *args.registerBPtr; } }
void execute_jmz_i(OpcodeExecuteArguments args) { if (*args.registerAPtr == 0) { args.process->registers.ip = args.immediateA.u16; } }

void execute_slp_r(OpcodeExecuteArguments args) { (void)args; } // TODO
void execute_slp_i(OpcodeExecuteArguments args) { (void)args; } // TODO

void execute_set_r(OpcodeExecuteArguments args) { *args.registerAPtr = *args.registerBPtr; }
void execute_set_i(OpcodeExecuteArguments args) { *args.registerAPtr = args.immediateA.u16; }

void execute_ldb_r(OpcodeExecuteArguments args) { *args.registerAPtr = (uint16_t)(args.process->memory[*args.registerBPtr]); }
void execute_ldb_i(OpcodeExecuteArguments args) { *args.registerAPtr = (uint16_t)(args.process->memory[args.immediateA.u16]); }

#define ADDR_OFFSET(addr, offset) ((uint16_t)((addr) + (offset)))
#define NEXT_ADDR(addr)           ADDR_OFFSET(addr, 1)

void execute_ldw_r(OpcodeExecuteArguments args) { *args.registerAPtr = ((uint16_t)(args.process->memory[NEXT_ADDR(*args.registerBPtr)]) << 8) | (uint16_t)(args.process->memory[*args.registerBPtr]); }
void execute_ldw_i(OpcodeExecuteArguments args) { *args.registerAPtr = ((uint16_t)(args.process->memory[NEXT_ADDR(args.immediateA.u16)]) << 8) | (uint16_t)(args.process->memory[args.immediateA.u16]); }

void execute_stb_rr(OpcodeExecuteArguments args) { args.process->memory[*args.registerBPtr] = (uint8_t)(*args.registerAPtr & 0xFF); }
void execute_stb_ri(OpcodeExecuteArguments args) { args.process->memory[args.immediateA.u16] = (uint8_t)(*args.registerAPtr & 0xFF); }
void execute_stb_ir(OpcodeExecuteArguments args) { args.process->memory[*args.registerAPtr] = (uint8_t)args.immediateA.u8; }
void execute_stb_ii(OpcodeExecuteArguments args) { args.process->memory[args.immediateB.u16] = (uint8_t)args.immediateA.u8; }

void execute_stw_rr(OpcodeExecuteArguments args) { args.process->memory[*args.registerBPtr] = (uint8_t)(*args.registerAPtr & 0xFF); args.process->memory[NEXT_ADDR(*args.registerBPtr)] = (uint8_t)((*args.registerAPtr >> 8) & 0xFF); }
void execute_stw_ri(OpcodeExecuteArguments args) { args.process->memory[args.immediateA.u16] = (uint8_t)(*args.registerAPtr & 0xFF); args.process->memory[NEXT_ADDR(args.immediateA.u16)] = (uint8_t)((*args.registerAPtr >> 8) & 0xFF); }
void execute_stw_ir(OpcodeExecuteArguments args) { args.process->memory[*args.registerAPtr] = (uint8_t)(args.immediateA.u16 & 0xFF); args.process->memory[NEXT_ADDR(*args.registerAPtr)] = (uint8_t)((args.immediateA.u16 >> 8) & 0xFF); }
void execute_stw_ii(OpcodeExecuteArguments args) { args.process->memory[args.immediateB.u16] = (uint8_t)(args.immediateA.u16 & 0xFF); args.process->memory[NEXT_ADDR(args.immediateB.u16)] = (uint8_t)((args.immediateA.u16 >> 8) & 0xFF); }

void execute_pshb(OpcodeExecuteArguments args) {
  args.process->memory[ADDR_OFFSET(args.process->registers.sp, -1)] = (uint8_t)(*args.registerAPtr & 0xFF);
  args.process->registers.sp -= 1;
}

void execute_pshw(OpcodeExecuteArguments args) {
  args.process->memory[ADDR_OFFSET(args.process->registers.sp, -2)] = (uint8_t)(*args.registerAPtr & 0xFF);
  args.process->memory[ADDR_OFFSET(args.process->registers.sp, -1)] = (uint8_t)((*args.registerAPtr >> 8) & 0xFF);
  args.process->registers.sp -= 2;
}

void execute_popb(OpcodeExecuteArguments args) {
  args.process->registers.sp += 1;
  *args.registerAPtr = (uint16_t)(args.process->memory[args.process->registers.sp - 1]);
}

void execute_popw(OpcodeExecuteArguments args) {
  args.process->registers.sp += 2;
  *args.registerAPtr = ((uint16_t)(args.process->memory[ADDR_OFFSET(args.process->registers.sp, -1)]) << 8)
                     | (uint16_t)(args.process->memory[ADDR_OFFSET(args.process->registers.sp, -2)]);
}

void execute_add_r(OpcodeExecuteArguments args) { *args.registerAPtr = *args.registerBPtr + *args.registerCPtr; }
void execute_add_i(OpcodeExecuteArguments args) { *args.registerAPtr = *args.registerBPtr + args.immediateA.u16; }

void execute_sub_rr(OpcodeExecuteArguments args) { *args.registerAPtr = *args.registerBPtr - *args.registerCPtr; }
void execute_sub_ri(OpcodeExecuteArguments args) { *args.registerAPtr = *args.registerBPtr - args.immediateA.u16; }
void execute_sub_ir(OpcodeExecuteArguments args) { *args.registerAPtr = args.immediateA.u16 - *args.registerBPtr; }

void execute_mul_r(OpcodeExecuteArguments args) { *args.registerAPtr = *args.registerBPtr * *args.registerCPtr; }
void execute_mul_i(OpcodeExecuteArguments args) { *args.registerAPtr = *args.registerBPtr * args.immediateA.u16; }

#define MAX_SAME_SIGN(x) (((x) != 0) ? (((x) > 0) ? 0x7FFF : 0x8000) : 0x0000)
void execute_divs_rr(OpcodeExecuteArguments args) { *args.registerAPtr = (*args.registerCPtr != 0) ? (uint16_t)((int16_t)(*args.registerBPtr) / (int16_t)(*args.registerCPtr)) : MAX_SAME_SIGN((int16_t)(*args.registerBPtr)); }
void execute_divs_ri(OpcodeExecuteArguments args) { *args.registerAPtr = (args.immediateA.u16 != 0) ? (uint16_t)((int16_t)(*args.registerBPtr) / args.immediateA.s16) : MAX_SAME_SIGN((int16_t)(*args.registerBPtr)); }
void execute_divs_ir(OpcodeExecuteArguments args) { *args.registerAPtr = (*args.registerBPtr != 0) ? (uint16_t)(args.immediateA.s16 / (int16_t)(*args.registerBPtr)) : MAX_SAME_SIGN(args.immediateA.s16); }

#define MAX_IF_POSITIVE(x) (((x) != 0) ? 0xFFFF : 0x0000)
void execute_divu_rr(OpcodeExecuteArguments args) { *args.registerAPtr = (*args.registerCPtr != 0) ? (*args.registerBPtr / *args.registerCPtr) : MAX_IF_POSITIVE(*args.registerBPtr); }
void execute_divu_ri(OpcodeExecuteArguments args) { *args.registerAPtr = (args.immediateA.u16 != 0) ? (*args.registerBPtr / args.immediateA.u16) : MAX_IF_POSITIVE(*args.registerBPtr); }
void execute_divu_ir(OpcodeExecuteArguments args) { *args.registerAPtr = (*args.registerBPtr != 0) ? (args.immediateA.u16 / *args.registerBPtr) : MAX_IF_POSITIVE(args.immediateA.u16); }

void execute_rems_rr(OpcodeExecuteArguments args) { *args.registerAPtr = (*args.registerCPtr != 0) ? (uint16_t)((int16_t)(*args.registerBPtr) % (int16_t)(*args.registerCPtr)) : *args.registerBPtr; }
void execute_rems_ri(OpcodeExecuteArguments args) { *args.registerAPtr = (args.immediateA.u16 != 0) ? (uint16_t)((int16_t)(*args.registerBPtr) % args.immediateA.s16) : *args.registerBPtr; }
void execute_rems_ir(OpcodeExecuteArguments args) { *args.registerAPtr = (*args.registerBPtr != 0) ? (uint16_t)(args.immediateA.s16 % (int16_t)(*args.registerBPtr)) : args.immediateA.u16; }

void execute_remu_rr(OpcodeExecuteArguments args) { *args.registerAPtr = (*args.registerCPtr != 0) ? (*args.registerBPtr % *args.registerCPtr) : *args.registerBPtr; }
void execute_remu_ri(OpcodeExecuteArguments args) { *args.registerAPtr = (args.immediateA.u16 != 0) ? (*args.registerBPtr % args.immediateA.u16) : *args.registerBPtr; }
void execute_remu_ir(OpcodeExecuteArguments args) { *args.registerAPtr = (*args.registerBPtr != 0) ? (args.immediateA.u16 % *args.registerBPtr) : args.immediateA.u16; }

void execute_and_r(OpcodeExecuteArguments args) { *args.registerAPtr = *args.registerBPtr & *args.registerCPtr; }
void execute_and_i(OpcodeExecuteArguments args) { *args.registerAPtr = *args.registerBPtr & args.immediateA.u16; }

void execute_ior_r(OpcodeExecuteArguments args) { *args.registerAPtr = *args.registerBPtr | *args.registerCPtr; }
void execute_ior_i(OpcodeExecuteArguments args) { *args.registerAPtr = *args.registerBPtr | args.immediateA.u16; }

void execute_xor_r(OpcodeExecuteArguments args) { *args.registerAPtr = *args.registerBPtr ^ *args.registerCPtr; }
void execute_xor_i(OpcodeExecuteArguments args) { *args.registerAPtr = *args.registerBPtr ^ args.immediateA.u16; }

void execute_lsh_rr(OpcodeExecuteArguments args) { *args.registerAPtr = *args.registerBPtr << *args.registerCPtr; }
void execute_lsh_ri(OpcodeExecuteArguments args) { *args.registerAPtr = *args.registerBPtr << args.immediateA.u4; }
void execute_lsh_ir(OpcodeExecuteArguments args) { *args.registerAPtr = args.immediateA.u16 << *args.registerBPtr; }

void execute_rshs_rr(OpcodeExecuteArguments args) { *args.registerAPtr = (uint16_t)((int16_t)(*args.registerBPtr) >> *args.registerCPtr); }
void execute_rshs_ri(OpcodeExecuteArguments args) { *args.registerAPtr = (uint16_t)((int16_t)(*args.registerBPtr) >> args.immediateA.u4); }
void execute_rshs_ir(OpcodeExecuteArguments args) { *args.registerAPtr = (uint16_t)(args.immediateA.s16 >> *args.registerBPtr); }

void execute_rshu_rr(OpcodeExecuteArguments args) { *args.registerAPtr = *args.registerBPtr >> *args.registerCPtr; }
void execute_rshu_ri(OpcodeExecuteArguments args) { *args.registerAPtr = *args.registerBPtr >> args.immediateA.u4; }
void execute_rshu_ir(OpcodeExecuteArguments args) { *args.registerAPtr = args.immediateA.u16 >> *args.registerBPtr; }

void execute_ceq_r(OpcodeExecuteArguments args) { *args.registerAPtr = (*args.registerBPtr == *args.registerCPtr) ? 1 : 0; }
void execute_ceq_i(OpcodeExecuteArguments args) { *args.registerAPtr = (*args.registerBPtr == args.immediateA.u16) ? 1 : 0; }

void execute_cne_r(OpcodeExecuteArguments args) { *args.registerAPtr = (*args.registerBPtr != *args.registerCPtr) ? 1 : 0; }
void execute_cne_i(OpcodeExecuteArguments args) { *args.registerAPtr = (*args.registerBPtr != args.immediateA.u16) ? 1 : 0; }

void execute_clts_rr(OpcodeExecuteArguments args) { *args.registerAPtr = ((int16_t)(*args.registerBPtr) < (int16_t)(*args.registerCPtr)) ? 1 : 0; }
void execute_clts_ri(OpcodeExecuteArguments args) { *args.registerAPtr = ((int16_t)(*args.registerBPtr) < args.immediateA.s16) ? 1 : 0; }
void execute_clts_ir(OpcodeExecuteArguments args) { *args.registerAPtr = (args.immediateA.s16 < (int16_t)(*args.registerBPtr)) ? 1 : 0; }

void execute_cltu_rr(OpcodeExecuteArguments args) { *args.registerAPtr = (*args.registerBPtr < *args.registerCPtr) ? 1 : 0; }
void execute_cltu_ri(OpcodeExecuteArguments args) { *args.registerAPtr = (*args.registerBPtr < args.immediateA.u16) ? 1 : 0; }
void execute_cltu_ir(OpcodeExecuteArguments args) { *args.registerAPtr = (args.immediateA.u16 < *args.registerBPtr) ? 1 : 0; }

void execute_cges_rr(OpcodeExecuteArguments args) { *args.registerAPtr = ((int16_t)(*args.registerBPtr) >= (int16_t)(*args.registerCPtr)) ? 1 : 0; }
void execute_cges_ri(OpcodeExecuteArguments args) { *args.registerAPtr = ((int16_t)(*args.registerBPtr) >= args.immediateA.s16) ? 1 : 0; }
void execute_cges_ir(OpcodeExecuteArguments args) { *args.registerAPtr = (args.immediateA.s16 >= (int16_t)(*args.registerBPtr)) ? 1 : 0; }

void execute_cgeu_rr(OpcodeExecuteArguments args) { *args.registerAPtr = (*args.registerBPtr >= *args.registerCPtr) ? 1 : 0; }
void execute_cgeu_ri(OpcodeExecuteArguments args) { *args.registerAPtr = (*args.registerBPtr >= args.immediateA.u16) ? 1 : 0; }
void execute_cgeu_ir(OpcodeExecuteArguments args) { *args.registerAPtr = (args.immediateA.u16 >= *args.registerBPtr) ? 1 : 0; }

#pragma endregion
