#include "rv_instr.h"

struct r_type* instr_to_r(uint32_t instr) {
        struct r_type *r_instr = malloc(sizeof(struct r_type));
        r_instr->opcode = instr                 & OPCODE;
        r_instr->rd     = BF_EXTRACT_FIELD(instr, R_RD_POS     , R_RD_MASK);
        r_instr->func_3 = BF_EXTRACT_FIELD(instr, R_FUNC3_POS  , R_FUNC3_MASK);
        r_instr->rs1    = BF_EXTRACT_FIELD(instr, R_RS1_POS    , R_RS1_MASK);
        r_instr->rs2    = BF_EXTRACT_FIELD(instr, R_RS2_POS    , R_RS2_MASK);
        r_instr->func_7 = BF_EXTRACT_FIELD(instr, R_FUNC7_POS  , R_FUNC7_MASK);
        return r_instr;
}

void free_r_type(struct r_type *r) {
        free(r);
}

struct i_type* instr_to_i(uint32_t instr) {
        struct i_type *i_instr = malloc(sizeof(struct i_type));
        i_instr->opcode = instr                 & OPCODE;
        i_instr->rd     = BF_EXTRACT_FIELD(instr, I_RD_POS     , I_RD_MASK);
        i_instr->func_3 = BF_EXTRACT_FIELD(instr, I_FUNC_3_POS , I_FUNC3_MASK);
        i_instr->rs1    = BF_EXTRACT_FIELD(instr, I_RS1_POS    , I_RS1_MASK);
        i_instr->imm    = BF_EXTRACT_FIELD(instr, I_IMM_POS    , I_IMM_MASK);
        return i_instr;
}

void free_i_type(struct i_type *i) {
        free(i);
}

struct s_type* instr_to_s(uint32_t instr) {
        struct s_type *s_instr = malloc(sizeof(struct s_type));
        s_instr->opcode = instr                 & OPCODE;
        s_instr->imm_ls = BF_EXTRACT_FIELD(instr, S_IMM_LSB_POS, I_IMM_MASK);
        s_instr->func_3 = BF_EXTRACT_FIELD(instr, S_FUNC_3_POS, S_FUNC3_MASK);
        s_instr->rs1    = BF_EXTRACT_FIELD(instr, S_RS1_POS, S_RS1_MASK);
        s_instr->rs2    = BF_EXTRACT_FIELD(instr, S_RS2_POS, S_RS2_MASK);
        s_instr->imm_ms = BF_EXTRACT_FIELD(instr, S_IMM_MSB_POS, S_IMM_MSB_MASK);
        return s_instr;
}

void free_s_type(struct s_type *s) {
        free(s);
}

struct u_type* instr_to_u(uint32_t instr) {
        struct u_type *u_instr = malloc(sizeof(struct u_type));
        u_instr->opcode        = instr & OPCODE;
        u_instr->rd            = BF_EXTRACT_FIELD(instr, U_RD_POS, U_RD_MASK);
        u_instr->imm           = BF_EXTRACT_FIELD(instr, U_IMM_POS, U_IMM_MASK);
        return u_instr;
}

void free_u_type(struct u_type *u) {
        free(u);
}

struct b_type* instr_to_b(uint32_t instr) {
        struct b_type *b_instr = malloc(sizeof(struct b_type));
        b_instr->opcode   = instr & OPCODE;
        b_instr->imm_11   = BF_EXTRACT_FIELD(instr, B_IMM_11_POS, B_IMM_11_MASK);
        b_instr->imm_4_1  = BF_EXTRACT_FIELD(instr, B_IMM_4_1_POS, B_IMM_4_1_MASK);
        b_instr->func_3   = BF_EXTRACT_FIELD(instr, B_FUNC3_POS, B_FUNC3_MASK);
        b_instr->rs1      = BF_EXTRACT_FIELD(instr, B_RS1_POS, B_RS1_MASK);
        b_instr->rs2      = BF_EXTRACT_FIELD(instr, B_RS2_POS, B_RS2_MASK);
        b_instr->imm_10_5 = BF_EXTRACT_FIELD(instr, B_IMM_10_5_POS, B_IMM_10_5_MASK);
        b_instr->imm_12   = BF_EXTRACT_FIELD(instr, B_IMM_12_POS, B_IMM_12_MASK);
        return b_instr;
}

void free_b_type(struct b_type *b) {
        free(b);
}

struct j_type *instr_to_j(uint32_t instr) {
        struct j_type *j_instr = malloc(sizeof(struct j_type));
        j_instr->opcode = instr & OPCODE;
        j_instr->rd = BF_EXTRACT_FIELD(instr, J_RD_POS, J_RD_MASK);
        j_instr->imm_19_12 = BF_EXTRACT_FIELD(instr, J_IMM_19_12_POS, J_IMM_19_12_MASK);
        j_instr->imm_11 = BF_EXTRACT_FIELD(instr, J_IMM_11_POS, J_IMM_11_MASK);
        j_instr->imm_10_1 = BF_EXTRACT_FIELD(instr, J_IMM_10_1_POS, J_IMM_10_1_MASK);
        j_instr->imm_20 = BF_EXTRACT_FIELD(instr, J_IMM_20_POS, J_IMM_20_MASK);
        return j_instr;
}

void free_j_type(struct j_type *j) {
        free(j);
}
