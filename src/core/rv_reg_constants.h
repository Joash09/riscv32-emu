#ifndef RV_REG_CONSTANTS_H_
#define RV_REG_CONSTANTS_H_

// Define bit masks
#define OPCODE    0x7F

#define R_RD_POS         07
#define R_FUNC3_POS      12
#define R_RS1_POS        15
#define R_RS2_POS        20
#define R_FUNC7_POS      25
#define R_FUNC7_TYPE_POS 30

#define R_RD_MASK         0x1F
#define R_FUNC3_MASK      0x07
#define R_RS1_MASK        0x1F
#define R_RS2_MASK        0x1F
#define R_FUNC7_MASK      0x7F
#define R_FUNC7_TYPE_MASK 0x01

#define I_RD_POS               7
#define I_FUNC_3_POS           12
#define I_RS1_POS              15
#define I_IMM_POS              20
#define I_IMM_SHIFT_AMOUNT_POS 20
#define I_IMM_SHIFT_TYPE_POS   30

#define I_RD_MASK               0x1F
#define I_FUNC3_MASK            0x07
#define I_RS1_MASK              0x1F
#define I_IMM_MASK              0xFFF
#define I_IMM_SHIFT_AMOUNT_MASK 0x1F
#define I_IMM_SHIFT_TYPE_MASK   0x1

#define S_IMM_LSB_POS 7
#define S_FUNC_3_POS  12
#define S_RS1_POS     15
#define S_RS2_POS     20
#define S_IMM_MSB_POS 25

#define S_IMM_LSB_MASK 0x1F
#define S_FUNC3_MASK   0x07
#define S_RS1_MASK     0x1F
#define S_RS2_MASK     0x1F
#define S_IMM_MSB_MASK 0x7F

#define U_RD_POS  7
#define U_IMM_POS 12

#define U_RD_MASK  0x1F
#define U_IMM_MASK 0xFFFFF

#define B_IMM_4_1_POS  8
#define B_IMM_10_5_POS 25
#define B_IMM_11_POS   7
#define B_IMM_12_POS   30
#define B_FUNC3_POS    12
#define B_RS1_POS      14
#define B_RS2_POS      20

#define B_IMM_4_1_MASK  0xF
#define B_IMM_10_5_MASK 0x3F
#define B_IMM_11_MASK   0x1
#define B_IMM_12_MASK   0x1
#define B_FUNC3_MASK    0x07
#define B_RS1_MASK      0x1F
#define B_RS2_MASK      0x1F

#define J_RD_POS        7
#define J_IMM_19_12_POS 12
#define J_IMM_11_POS    20
#define J_IMM_10_1_POS  21
#define J_IMM_20_POS    31

#define J_RD_MASK        0x1F
#define J_IMM_19_12_MASK 0xFF
#define J_IMM_11_MASK    0x1
#define J_IMM_10_1_MASK  0x3FF
#define J_IMM_20_MASK    0x1

#endif // _RV_REG_CONSTANTS_H_
