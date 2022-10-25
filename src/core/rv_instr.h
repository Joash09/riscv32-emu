#ifndef RV_INSTR_H_
#define RV_INSTR_H_

#include <stdlib.h>
#include <stdint.h>

#include "rv_reg_constants.h"

#define BF_EXTRACT_FIELD(BF, STRT_POS, MASK) ((BF>>STRT_POS) & MASK)

struct r_type {
        unsigned int opcode : 7;
        unsigned int rd     : 5;
        unsigned int func_3 : 3;
        unsigned int rs1    : 5;
        unsigned int rs2    : 5;
        unsigned int func_7 : 7;
};

struct i_type {
        unsigned int opcode : 7;
        unsigned int rd     : 5;
        unsigned int func_3 : 3;
        unsigned int rs1    : 5;
        unsigned int imm    : 11;
};

struct s_type {
        unsigned int opcode : 7;
        unsigned int imm_ls : 5;
        unsigned int func_3 : 3;
        unsigned int rs1    : 5;
        unsigned int rs2    : 5;
        unsigned int imm_ms : 11;
};

struct u_type {
        unsigned int opcode : 7;
        unsigned int rd     : 5;
        unsigned int imm    : 20;
};

struct j_type {
        unsigned int opcode    : 7;
        unsigned int rd        : 5;
        unsigned int imm_19_12 : 8;
        unsigned int imm_11    : 1;
        unsigned int imm_10_1  : 10;
        unsigned int imm_20    : 1;
};

struct b_type {
        unsigned int opcode   : 7;
        unsigned int imm_11   : 1;
        unsigned int imm_4_1  : 4;
        unsigned int func_3   : 3;
        unsigned int rs1      : 5;
        unsigned int rs2      : 5;
        unsigned int imm_10_5 : 6;
        unsigned int imm_12   : 1;
};

struct r_type* instr_to_r(uint32_t instr);
struct i_type* instr_to_i(uint32_t instr);
struct s_type* instr_to_s(uint32_t instr);
struct u_type* instr_to_u(uint32_t instr);
struct b_type* instr_to_b(uint32_t instr);
struct j_type* instr_to_j(uint32_t instr);

void free_r_type(struct r_type *r);
void free_i_type(struct i_type *i);
void free_s_type(struct s_type *s);
void free_u_type(struct u_type *u);
void free_b_type(struct b_type *b);
void free_j_type(struct j_type *j);

#endif // RV_INSTR_H_
