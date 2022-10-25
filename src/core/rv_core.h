#ifndef RV_CORE_H_
#define RV_CORE_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "rv_opcode_constants.h"
#include "rv_instr.h"

#define MEMORY_SIZE 1080*1080*1 // 128MiB

// Define bus and riscv_core structs
struct bus {
        unsigned char *dram;
};

struct riscv_core {
        uint32_t *registers;
        uint32_t program_counter;
        struct bus *bus;
};

// Define type for function pointers
typedef void (*opcode_func_ptr)(struct riscv_core*, uint32_t);
typedef void (*branch_func_ptr)(struct riscv_core*, struct b_type*);
typedef void (*load_func_ptr)(struct riscv_core*, struct i_type*);
typedef void (*store_func_ptr)(struct riscv_core*, struct s_type*);
typedef void (*imm_func_ptr)(struct riscv_core*, struct i_type*);
typedef void (*reg_func_ptr)(struct riscv_core*, struct r_type*);

void init_rv_func_tbls();

struct bus *init_bus();
void free_bus(struct bus *);

struct riscv_core *core_init();
void free_core(struct riscv_core *);

void execute_instr(struct riscv_core *, uint32_t);

#endif // RV_CORE_H_
