#include "processor/opcode.h"

#pragma region Opcode execute function declarations

void execute_nop(ProcessState* state, InstructionOperands operands);

void execute_jmp_r(ProcessState* state, InstructionOperands operands);
void execute_jmp_i(ProcessState* state, InstructionOperands operands);

void execute_jmz_r(ProcessState* state, InstructionOperands operands);
void execute_jmz_i(ProcessState* state, InstructionOperands operands);

void execute_slp_r(ProcessState* state, InstructionOperands operands);
void execute_slp_i(ProcessState* state, InstructionOperands operands);

void execute_set_r(ProcessState* state, InstructionOperands operands);
void execute_set_i(ProcessState* state, InstructionOperands operands);

void execute_ldb_r(ProcessState* state, InstructionOperands operands);
void execute_ldb_i(ProcessState* state, InstructionOperands operands);

void execute_ldw_r(ProcessState* state, InstructionOperands operands);
void execute_ldw_i(ProcessState* state, InstructionOperands operands);

void execute_stb_rr(ProcessState* state, InstructionOperands operands);
void execute_stb_ri(ProcessState* state, InstructionOperands operands);
void execute_stb_ir(ProcessState* state, InstructionOperands operands);
void execute_stb_ii(ProcessState* state, InstructionOperands operands);

void execute_stw_rr(ProcessState* state, InstructionOperands operands);
void execute_stw_ri(ProcessState* state, InstructionOperands operands);
void execute_stw_ir(ProcessState* state, InstructionOperands operands);
void execute_stw_ii(ProcessState* state, InstructionOperands operands);

void execute_pshb(ProcessState* state, InstructionOperands operands);

void execute_pshw(ProcessState* state, InstructionOperands operands);

void execute_popb(ProcessState* state, InstructionOperands operands);

void execute_popw(ProcessState* state, InstructionOperands operands);

void execute_add_r(ProcessState* state, InstructionOperands operands);
void execute_add_i(ProcessState* state, InstructionOperands operands);

void execute_sub_rr(ProcessState* state, InstructionOperands operands);
void execute_sub_ri(ProcessState* state, InstructionOperands operands);
void execute_sub_ir(ProcessState* state, InstructionOperands operands);

void execute_mul_r(ProcessState* state, InstructionOperands operands);
void execute_mul_i(ProcessState* state, InstructionOperands operands);

void execute_divs_rr(ProcessState* state, InstructionOperands operands);
void execute_divs_ri(ProcessState* state, InstructionOperands operands);
void execute_divs_ir(ProcessState* state, InstructionOperands operands);

void execute_divu_rr(ProcessState* state, InstructionOperands operands);
void execute_divu_ri(ProcessState* state, InstructionOperands operands);
void execute_divu_ir(ProcessState* state, InstructionOperands operands);

void execute_rems_rr(ProcessState* state, InstructionOperands operands);
void execute_rems_ri(ProcessState* state, InstructionOperands operands);
void execute_rems_ir(ProcessState* state, InstructionOperands operands);

void execute_remu_rr(ProcessState* state, InstructionOperands operands);
void execute_remu_ri(ProcessState* state, InstructionOperands operands);
void execute_remu_ir(ProcessState* state, InstructionOperands operands);

void execute_and_r(ProcessState* state, InstructionOperands operands);
void execute_and_i(ProcessState* state, InstructionOperands operands);

void execute_ior_r(ProcessState* state, InstructionOperands operands);
void execute_ior_i(ProcessState* state, InstructionOperands operands);

void execute_xor_r(ProcessState* state, InstructionOperands operands);
void execute_xor_i(ProcessState* state, InstructionOperands operands);

void execute_lsh_rr(ProcessState* state, InstructionOperands operands);
void execute_lsh_ri(ProcessState* state, InstructionOperands operands);
void execute_lsh_ir(ProcessState* state, InstructionOperands operands);

void execute_rshs_rr(ProcessState* state, InstructionOperands operands);
void execute_rshs_ri(ProcessState* state, InstructionOperands operands);
void execute_rshs_ir(ProcessState* state, InstructionOperands operands);

void execute_rshu_rr(ProcessState* state, InstructionOperands operands);
void execute_rshu_ri(ProcessState* state, InstructionOperands operands);
void execute_rshu_ir(ProcessState* state, InstructionOperands operands);

void execute_ceq_r(ProcessState* state, InstructionOperands operands);
void execute_ceq_i(ProcessState* state, InstructionOperands operands);

void execute_cne_r(ProcessState* state, InstructionOperands operands);
void execute_cne_i(ProcessState* state, InstructionOperands operands);

void execute_clts_rr(ProcessState* state, InstructionOperands operands);
void execute_clts_ri(ProcessState* state, InstructionOperands operands);
void execute_clts_ir(ProcessState* state, InstructionOperands operands);

void execute_cltu_rr(ProcessState* state, InstructionOperands operands);
void execute_cltu_ri(ProcessState* state, InstructionOperands operands);
void execute_cltu_ir(ProcessState* state, InstructionOperands operands);

void execute_cges_rr(ProcessState* state, InstructionOperands operands);
void execute_cges_ri(ProcessState* state, InstructionOperands operands);
void execute_cges_ir(ProcessState* state, InstructionOperands operands);

void execute_cgeu_rr(ProcessState* state, InstructionOperands operands);
void execute_cgeu_ri(ProcessState* state, InstructionOperands operands);
void execute_cgeu_ir(ProcessState* state, InstructionOperands operands);

#pragma endregion

const OpcodeInfo OPCODE_INFO[OPCODE_COUNT] = {
  {
    .identifier = "nop",
    .operandLayout = OPERAND_LAYOUT_NONE,
  }
  // TODO: Define info for other opcodes
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
