#ifndef RV_OPCODE_CONSTANTS_H_
#define RV_OPCODE_CONSTANTS_H_

#define LUI     0x37
#define AUIPC   0x17
#define JAL     0x6F
#define JALR    0x67
#define BRANCH  0x63
#define LOAD    0x3
#define STORE   0x23
#define INT_IMM 0x13
#define INT_REG 0x33
#define FENCE   0xF
#define ENV     0x73

#define BRANCH_FUNC3_BEQ  0x0
#define BRANCH_FUNC3_BNE  0x1
#define BRANCH_FUNC3_BLT  0x4
#define BRANCH_FUNC3_BGE  0x5
#define BRANCH_FUNC3_BLTU 0x6
#define BRANCH_FUNC3_BGEU 0x7

#define LOAD_FUNC3_LB  0x0
#define LOAD_FUNC3_LH  0x1
#define LOAD_FUNC3_LW  0x2
#define LOAD_FUNC3_LBU 0x4
#define LOAD_FUNC3_LHU 0x5

#define STORE_FUNC3_SB 0x0
#define STORE_FUNC3_SH 0x1
#define STORE_FUNC3_SW 0x2

#define IMM_FUNC3_ADDI  0x0
#define IMM_FUNC3_SLTI  0x2
#define IMM_FUNC3_SLTIU 0x3
#define IMM_FUNC3_XORI  0x4
#define IMM_FUNC3_ORI   0x6
#define IMM_FUNC3_ANDI  0x7

#define IMM_FUNC3_SLLI 0x1
#define IMM_FUNC3_SRLI_SRAI 0x5

#define REG_FUNC3_ADD_SUB 0x0
#define REG_FUNC3_SLL     0x1
#define REG_FUNC3_SLT     0x2
#define REG_FUNC3_SLTU    0x3
#define REG_FUNC3_XOR     0x4
#define REG_FUNC3_SRL_SRA 0x5
#define REG_FUNC3_OR      0x6
#define REG_FUNC3_AND     0x7

#endif // RV_OPCODE_CONSTANTS_H_
