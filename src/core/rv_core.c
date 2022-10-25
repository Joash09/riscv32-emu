#include <stdint.h>
#include <stdio.h>

#include "rv_core.h"
#include "rv_instr.h"
#include "rv_func_hashmap.h"
#include "rv_opcode_constants.h"
#include "../dram/dram.h"

#define PRINT_REG(addr, val) printf("Addr: %x, %x\n", addr, val)

/*
 * Define static opcode hash table and lookup arrays for functions
 */
#define OPCODE_HASHTAB_SIZE 11
#define BRANCH_TBL_SIZE     5
#define LOAD_TBL_SIZE       5
#define STORE_TBL_SIZE      3
#define IMM_TBL_SIZE        6
#define REG_TBL_SIZE        7

static struct func_nlist opcode_hashtab[OPCODE_HASHTAB_SIZE] = {0}; // Not as simple as opcode % size

static branch_func_ptr branch_func_tbl[BRANCH_TBL_SIZE];
static load_func_ptr load_func_tbl[LOAD_TBL_SIZE];
static store_func_ptr store_func_tbl[STORE_TBL_SIZE];
static imm_func_ptr imm_func_tbl[IMM_TBL_SIZE];
static reg_func_ptr reg_func_tbl[REG_TBL_SIZE];

/*
 * static methods
 */
static signed int sign_ext(unsigned int bit_len, int ext_len, uint32_t val) {
        signed int r;
        unsigned int x;
        int const m = 1U << (bit_len - 1);
        x = val & ((1U << bit_len) - 1);
        r = (x ^ m) - m;
        return r;
}

/*
 * Places U-immediate value in the top 20 bits of destination register rd
 */
static void lui_compute(struct riscv_core *core, uint32_t instr) {
        printf("Executing LUI\n"); //DEBUG
        struct u_type *u_instr = instr_to_u(instr);
        core->registers[u_instr->rd] = (u_instr->imm)<<12;
        PRINT_REG(u_instr->rd, core->registers[u_instr->rd]);
        free(u_instr);
}

/*
 * Create 32 bit offset with immediate (bottom 12 bits are zero)
 * Add offset to rd and place result in PC
 */
static void auipc_compute(struct riscv_core *core, uint32_t instr) {
        printf("Executing AUIPC");
        struct u_type *u_instr = instr_to_u(instr);
        core->registers[u_instr->rd] += (u_instr->imm)<<12;
        core->program_counter = core->registers[u_instr->rd];
        free(u_instr);
}

/*
 * Encodes *signed* offset that is added to address of jump instruction (current PC counter)
 * Stores the address of the register following the jump (pc+4) in register rd
 */
static void jal_compute(struct riscv_core *core, uint32_t instr) {
        struct j_type *j_instr = instr_to_j(instr);

        unsigned int concat_imm = (j_instr->imm_20) << 20
                | (j_instr->imm_19_12)<<12
                | (j_instr->imm_11)<<11
                | (j_instr->imm_10_1)<<1;
        signed int sgn_ext_imm_offset = sign_ext(12, 32, concat_imm);
        core->program_counter += sgn_ext_imm_offset; // signed 32 bit + unsigned 32 bit
        core->registers[j_instr->rd] = core->program_counter;
        free(j_instr);
}

/*
 * Write PC + 4 to rd
 * Set PC to rs1 + sign extended offset
 */
static void jalr_compute(struct riscv_core *core, uint32_t instr) {
        struct i_type *i_instr = instr_to_i(instr);
        core->registers[i_instr->rd] = core->program_counter + 4;
        signed int sgn_ext_imm_offset = sign_ext(12, 32, i_instr->imm);
        core->program_counter = i_instr->rs1 + sgn_ext_imm_offset;
        free_i_type(i_instr);
}


static void beq_compute(struct riscv_core *core, struct b_type* b_instr) {
        unsigned int concat_offset = b_instr->imm_12<<12
                | b_instr->imm_11<<11
                | b_instr->imm_10_5<<5
                | b_instr->imm_4_1 << 1;
        signed int sgn_ext_offset = sign_ext(12, 32, concat_offset);

        if (b_instr->rs1 == b_instr->rs2) {
                signed int signed_pc_offset = sgn_ext_offset + (signed) core->program_counter;
                core->program_counter = (unsigned) signed_pc_offset;
        }
}

static void bne_compute(struct riscv_core *core, struct b_type* b_instr) {
        unsigned int concat_offset = b_instr->imm_12<<12
                | b_instr->imm_11<<11
                | b_instr->imm_10_5<<5
                | b_instr->imm_4_1 << 1;
        signed int sgn_ext_offset = sign_ext(12, 32, concat_offset);

        if (b_instr->rs1 != b_instr->rs2) {
                signed int signed_pc_offset = sgn_ext_offset + (signed) core->program_counter;
                core->program_counter = (unsigned) signed_pc_offset;
        }
}

static void blt_compute(struct riscv_core *core, struct b_type *b_instr) {
        unsigned int concat_offset = b_instr->imm_12<<12
                | b_instr->imm_11<<11
                | b_instr->imm_10_5<<5
                | b_instr->imm_4_1 << 1;
        signed int sgn_ext_offset = sign_ext(12, 32, concat_offset);

        if (b_instr->rs1 < b_instr->rs2) {
                signed int signed_pc_offset = sgn_ext_offset + (signed) core->program_counter;
                core->program_counter = (unsigned) signed_pc_offset;
        }
}

static void bltu_compute(struct riscv_core *core, struct b_type *b_instr) {
        unsigned int concat_offset = b_instr->imm_12<<12
                | b_instr->imm_11<<11
                | b_instr->imm_10_5<<5
                | b_instr->imm_4_1 << 1;

        if (b_instr->rs1 < b_instr->rs2)
                core->program_counter += concat_offset;
}

static void bge_compute(struct riscv_core *core, struct b_type *b_instr) {
        unsigned int concat_offset = b_instr->imm_12<<12
                | b_instr->imm_11<<11
                | b_instr->imm_10_5<<5
                | b_instr->imm_4_1 << 1;
        signed int sgn_ext_offset = sign_ext(12, 32, concat_offset);

        if (b_instr->rs1 > b_instr->rs2) {
                signed int signed_pc_offset = sgn_ext_offset + (signed) core->program_counter;
                core->program_counter = (unsigned) signed_pc_offset;
        }
}

static void bgeu_compute(struct riscv_core *core, struct b_type *b_instr) {
        unsigned int concat_offset = b_instr->imm_12<<12
                | b_instr->imm_11<<11
                | b_instr->imm_10_5<<5
                | b_instr->imm_4_1 << 1;

        if (b_instr->rs1 > b_instr->rs2)
                core->program_counter += concat_offset;
}

static void branch_compute(struct riscv_core *core, uint32_t instr) {
        struct b_type *b_instr = instr_to_b(instr);
        branch_func_tbl[b_instr->func_3](core, b_instr);
        free(b_instr);
}


static void lw_compute(struct riscv_core *core, struct i_type *i_instr) {
        signed int offset = sign_ext(12, 32, i_instr->imm);
        signed int addr = offset + (signed) i_instr->rs1;
        core->registers[i_instr->rd] = load_word(core->bus->dram, (unsigned) addr);
}

static void lh_compute(struct riscv_core *core, struct i_type *i_instr) {
        signed int offset = sign_ext(12, 32, i_instr->imm);
        signed int addr = offset + (signed) i_instr->rs1;
        core->registers[i_instr->rd] = sign_ext(16, 32, load_half_word(core->bus->dram, (unsigned) addr));
}

static void lhu_compute(struct riscv_core *core, struct i_type *i_instr) {
        unsigned int addr = i_instr->imm + core->registers[i_instr->rs1];
        core->registers[i_instr->rd] = sign_ext(16, 32, core->bus->dram[addr]);
}

/*
 * Alternate to lh, lb is zero extended (right logical shift) not sign extended
 * Expecting zero extension is default behaviour for unsigned ints in C
 */
static void lb_compute(struct riscv_core *core, struct i_type *i_instr) {
        signed int offset = sign_ext(12, 32, i_instr->imm);
        signed int addr = offset + (signed) i_instr->rs1;
        core->registers[i_instr->rd] = load_byte(core->bus->dram, (unsigned) addr); // returns uint8
}

static void lbu_compute(struct riscv_core *core, struct i_type *i_instr) {
        unsigned int addr = i_instr->rs1 + i_instr->imm;
        core->registers[i_instr->rd] = load_byte(core->bus->dram, addr); // returns uint8
}

/*
 * Transfer value between registers and memory
 */
static void load_compute(struct riscv_core *core, uint32_t instr) {
        struct i_type *i_instr = instr_to_i(instr);
        load_func_tbl[i_instr->func_3](core, i_instr);
        free_i_type(i_instr);
}


static void sb_compute(struct riscv_core *core, struct s_type *s_instr) {
        unsigned int concat_imm = s_instr->imm_ms<<5 | s_instr->imm_ls;
        signed int sgn_ext_offset = sign_ext(12, 32, concat_imm);
        signed int addr = sgn_ext_offset + (signed) s_instr->rs1;
        store_byte(core->bus->dram, addr, core->registers[s_instr->rs2]);
}

static void sh_compute(struct riscv_core *core, struct s_type *s_instr) {
        unsigned int concat_imm = s_instr->imm_ms<<5 | s_instr->imm_ls;
        signed int sgn_ext_offset = sign_ext(12, 32, concat_imm);
        signed int addr = sgn_ext_offset + (signed) s_instr->rs1;
        store_byte(core->bus->dram, addr, core->registers[s_instr->rs2]);
}

static void sw_compute(struct riscv_core *core, struct s_type *s_instr) {
        unsigned int concat_imm = s_instr->imm_ms<<5 | s_instr->imm_ls;
        signed int sgn_ext_offset = sign_ext(12, 32, concat_imm);
        signed int addr = sgn_ext_offset + (signed) s_instr->rs1;
        store_word(core->bus->dram, addr, core->registers[s_instr->rs2]);
}

static void store_compute(struct riscv_core *core, uint32_t instr) {
        struct s_type *s_instr = instr_to_s(instr);
        store_func_tbl[s_instr->func_3](core, s_instr);
        free_s_type(s_instr);
}


static void addi_compute(struct riscv_core* core, struct i_type* i_instr) {
        core->registers[i_instr->rd] =
                core->registers[i_instr->rs1] + sign_ext(12, 32, i_instr->imm);
}

static void slti_compute(struct riscv_core* core, struct i_type* i_instr) {
        if (core->registers[i_instr->rs1] < sign_ext(12, 32, i_instr->imm))
                core->registers[i_instr->rd] = 0x1;
        else
                core->registers[i_instr->rd] = 0x0;
}

static void sltiu_compute(struct riscv_core* core, struct i_type* i_instr) {

}

static void andi_compute(struct riscv_core* core, struct i_type* i_instr) {
        signed int sgn_ext_imm = sign_ext(12, 32, i_instr->imm);
        core->registers[i_instr->rd] = core->registers[i_instr->rs1] & sgn_ext_imm;
}

static void ori_compute(struct riscv_core* core, struct i_type* i_instr) {
        signed int sgn_ext_imm = sign_ext(12, 32, i_instr->imm);
        core->registers[i_instr->rd] = core->registers[i_instr->rs1] | sgn_ext_imm;
}

static void xori_compute(struct riscv_core* core, struct i_type* i_instr) {
        signed int sgn_ext_imm = sign_ext(12, 32, i_instr->imm);
        core->registers[i_instr->rd] = core->registers[i_instr->rs1] ^ sgn_ext_imm;
}

/*
 * Performs logical left shift
 */
static void slli_compute(struct riscv_core* core, struct i_type* i_instr) {
        unsigned int shift_amnt = BF_EXTRACT_FIELD(i_instr->imm, I_IMM_SHIFT_AMOUNT_POS, I_IMM_SHIFT_AMOUNT_MASK);
        core->registers[i_instr->rd] = core->registers[i_instr->rs1] << shift_amnt;
}

/*
 * Performs SRLI (Shift logical right) or SRAI (shift arithmetic right) depending on type
 */
static void shift_right_imm_compute(struct riscv_core* core, struct i_type* i_instr) {

       unsigned int type = BF_EXTRACT_FIELD(i_instr->imm, I_IMM_SHIFT_TYPE_POS, I_IMM_SHIFT_TYPE_MASK);
       unsigned int shift_amnt = BF_EXTRACT_FIELD(i_instr->imm, I_IMM_SHIFT_AMOUNT_POS, I_IMM_SHIFT_AMOUNT_MASK);

       if (type) { // srai

               // TODO
               core->registers[i_instr->rd] = core->registers[i_instr->rs1] >> shift_amnt;
       }
       else // srli
               core->registers[i_instr->rd] = core->registers[i_instr->rs1] >> shift_amnt;

}

static void int_imm_compute(struct riscv_core *core, uint32_t instr) {
        struct i_type *i_instr = instr_to_i(instr);
        imm_func_tbl[i_instr->func_3](core, i_instr);
        free(i_instr);
}


static void add_sub_compute(struct riscv_core *core, struct r_type *r_instr) {
        unsigned int type = BF_EXTRACT_FIELD(r_instr->func_7, R_FUNC7_TYPE_POS, R_FUNC7_TYPE_MASK);
        if (type) {
                core->registers[r_instr->rd] = core->registers[r_instr->rs1] - core->registers[r_instr->rs2];
        }
        else {
                // TODO: Ensure lower bits are stored
                core->registers[r_instr->rd] =
                        core->registers[r_instr->rs1] + core->registers[r_instr->rs2];
        }
}

static void slt_compute(struct riscv_core *core, struct r_type *r_instr) {
        if ((signed) core->registers[r_instr->rs1] < (signed) core->registers[r_instr->rs2])
                core->registers[r_instr->rd] = 0x1;
        else
                core->registers[r_instr->rd] = 0x0;
}

static void sltu_compute(struct riscv_core *core, struct r_type *r_instr) {
        if (core->registers[r_instr->rs1] < core->registers[r_instr->rs2])
                core->registers[r_instr->rd] = 0x1;
        else
                core->registers[r_instr->rd] = 0x0;
}

static void and_compute(struct riscv_core *core, struct r_type *r_instr) {
        core->registers[r_instr->rd] = core->registers[r_instr->rs1] & core->registers[r_instr->rs2];
}

static void or_compute(struct riscv_core *core, struct r_type *r_instr) {
        core->registers[r_instr->rd] = core->registers[r_instr->rs1] | core->registers[r_instr->rs2];
}

static void xor_compute(struct riscv_core *core, struct r_type *r_instr) {
        core->registers[r_instr->rd] = core->registers[r_instr->rs1] ^ core->registers[r_instr->rs2];
}

/*
 * shift r1 by the lower 5 bits in r2
 */
static void sll_compute(struct riscv_core *core, struct r_type *r_instr) {
        unsigned int shift = core->registers[r_instr->rs2] & 0x1F;
        core->registers[r_instr->rd] = core->registers[r_instr->rs1] << shift;
}

static void shift_right_compute(struct riscv_core *core, struct r_type *r_instr) {
        unsigned int type = BF_EXTRACT_FIELD(r_instr->func_7, R_FUNC7_TYPE_POS, R_FUNC7_TYPE_MASK);
        unsigned int shift = core->registers[r_instr->rs2] & 0x1F;
        if (type) { // shift arithmetic shift

        }
        else
                core->registers[r_instr->rd] = core->registers[r_instr->rs1] >> shift;
}

static void int_reg_compute(struct riscv_core *core, uint32_t instr) {
        struct r_type *r_instr = instr_to_r(instr);
        reg_func_tbl[r_instr->func_3](core, r_instr);
        free_r_type(r_instr);
}


static void ecall_compute(struct riscv_core *core, uint32_t instr) {

}

static void ebreak_compute(struct riscv_core *core, uint32_t instr) {

}

/*
 * Non-static methods
 */

void init_rv_func_tbls() {

  // Create hash lookup table for opcode methods
  // This avoids switch statement and wasted space with a simple lookup array as used by the other function tables
  hashtab_install(opcode_hashtab, OPCODE_HASHTAB_SIZE, LUI, lui_compute);
  hashtab_install(opcode_hashtab, OPCODE_HASHTAB_SIZE, AUIPC, auipc_compute);
  hashtab_install(opcode_hashtab, OPCODE_HASHTAB_SIZE, JAL, jal_compute);
  hashtab_install(opcode_hashtab, OPCODE_HASHTAB_SIZE, JALR, jalr_compute);
  hashtab_install(opcode_hashtab, OPCODE_HASHTAB_SIZE, BRANCH, branch_compute);
  hashtab_install(opcode_hashtab, OPCODE_HASHTAB_SIZE, LOAD, load_compute);
  hashtab_install(opcode_hashtab, OPCODE_HASHTAB_SIZE, STORE, store_compute);
  hashtab_install(opcode_hashtab, OPCODE_HASHTAB_SIZE, INT_IMM, int_imm_compute);
  hashtab_install(opcode_hashtab, OPCODE_HASHTAB_SIZE, INT_REG, int_reg_compute);
  /* hashtab_install(opcode_hashtab, OPCODE_HASHTAB_SIZE, FENCE, fence_compute); */
  /* hashtab_install(opcode_hashtab, OPCODE_HASHTAB_SIZE, ENV, env_call_break_compute); */

  branch_func_tbl[BRANCH_FUNC3_BEQ  % BRANCH_TBL_SIZE] = beq_compute;
  branch_func_tbl[BRANCH_FUNC3_BNE  % BRANCH_TBL_SIZE] = bne_compute;
  branch_func_tbl[BRANCH_FUNC3_BLT  % BRANCH_TBL_SIZE] = blt_compute;
  branch_func_tbl[BRANCH_FUNC3_BGE  % BRANCH_TBL_SIZE] = bge_compute;
  branch_func_tbl[BRANCH_FUNC3_BLTU % BRANCH_TBL_SIZE] = bltu_compute;
  branch_func_tbl[BRANCH_FUNC3_BGEU % BRANCH_TBL_SIZE] = bgeu_compute;

  load_func_tbl[LOAD_FUNC3_LB  % LOAD_TBL_SIZE] = lb_compute;
  load_func_tbl[LOAD_FUNC3_LH  % LOAD_TBL_SIZE] = lh_compute;
  load_func_tbl[LOAD_FUNC3_LW  % LOAD_TBL_SIZE] = lw_compute;
  load_func_tbl[LOAD_FUNC3_LBU % LOAD_TBL_SIZE] = lbu_compute;
  load_func_tbl[LOAD_FUNC3_LHU % LOAD_TBL_SIZE] = lhu_compute;

  store_func_tbl[STORE_FUNC3_SB % STORE_TBL_SIZE] = sb_compute;
  store_func_tbl[STORE_FUNC3_SH % STORE_TBL_SIZE] = sh_compute;
  store_func_tbl[STORE_FUNC3_SW % STORE_TBL_SIZE] = sw_compute;

  imm_func_tbl[IMM_FUNC3_ADDI      % IMM_TBL_SIZE] = addi_compute;
  imm_func_tbl[IMM_FUNC3_SLTI      % IMM_TBL_SIZE] = slti_compute;
  imm_func_tbl[IMM_FUNC3_SLTIU     % IMM_TBL_SIZE] = sltiu_compute;
  imm_func_tbl[IMM_FUNC3_XORI      % IMM_TBL_SIZE] = xori_compute;
  imm_func_tbl[IMM_FUNC3_ORI       % IMM_TBL_SIZE] = ori_compute;
  imm_func_tbl[IMM_FUNC3_ANDI      % IMM_TBL_SIZE] = andi_compute;
  imm_func_tbl[IMM_FUNC3_SLLI      % IMM_TBL_SIZE] = slli_compute;
  imm_func_tbl[IMM_FUNC3_SRLI_SRAI % IMM_TBL_SIZE] = shift_right_imm_compute;

  reg_func_tbl[REG_FUNC3_ADD_SUB % REG_TBL_SIZE] = add_sub_compute;
  reg_func_tbl[REG_FUNC3_SLL     % REG_TBL_SIZE] = sll_compute;
  reg_func_tbl[REG_FUNC3_SLT     % REG_TBL_SIZE] = slt_compute;
  reg_func_tbl[REG_FUNC3_SLTU    % REG_TBL_SIZE] = sltu_compute;
  reg_func_tbl[REG_FUNC3_XOR     % REG_TBL_SIZE] = xor_compute;
  reg_func_tbl[REG_FUNC3_SRL_SRA % REG_TBL_SIZE] = shift_right_compute;
  reg_func_tbl[REG_FUNC3_OR      % REG_TBL_SIZE] = or_compute;
  reg_func_tbl[REG_FUNC3_AND     % REG_TBL_SIZE] = and_compute;

}

struct bus* init_bus() {
        struct bus* bus = malloc(sizeof(struct bus));
        bus->dram = malloc(sizeof(unsigned char) * MEMORY_SIZE);
        return bus;
}

void free_bus(struct bus* bus) {
        free(bus->dram);
        free(bus);
}

struct riscv_core* core_init() {
        struct riscv_core* core = malloc(sizeof(struct riscv_core));
        core->bus = init_bus();
        core->registers = malloc(sizeof(uint32_t) * 32);
        memset(core->registers, 0x0, 32*sizeof(uint32_t)); /* Set registers to 0 */
        core->program_counter = 0; // TODO: Change to memory base
        core->registers[0] = 0;
        core->registers[2] = MEMORY_SIZE;
        return core;
}

void free_core(struct riscv_core* core) {
        free_bus(core->bus);
        free(core->registers);
        free(core);
}

void execute_instr(struct riscv_core *core, uint32_t instr) {
   unsigned int opcode = instr & OPCODE;
   opcode_hashtab[opcode].func(core, instr);
}
