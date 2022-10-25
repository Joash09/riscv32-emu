#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "core/rv_opcode_constants.h"
#include "core/rv_core.h"
#include "dram/dram.h"

int readfile(char *filename, unsigned char *dram) {

        FILE *file;
        uint8_t *buffer;

        file = fopen(filename, "rb");
        if(!file) return 1;
        // TODO: Better error handling

        fseek(file, 0, SEEK_END);
        int fileLen = ftell(file);
        fseek(file, 0, SEEK_SET);

        buffer = (uint8_t*) malloc(fileLen+1);
        // TODO: Check space was allocated

        size_t ret = fread(buffer, fileLen, 1, file);
        if (ret)
                memcpy(dram, buffer, fileLen+1);

        free(buffer);
        fclose(file);

        return 0;
}

int main()
{

  init_rv_func_tbls();
  struct riscv_core *core = core_init();

  // Testing opcode dispatch table
  /* uint32_t instruction = 0x7FFFF1B7; */
  /* unsigned int opcode = instruction & OPCODE; */
  /* opcode_tbl[opcode](core, instruction); */

  // Testing reading file into dram
  readfile("./rv32ui-p-lui.bin", core->bus->dram);
  /* for (int i = 0; i < 1080; i++) { */
  /*         printf("%x ", core->bus->dram[i]); */
  /* } */

  // Read words in dram and execute
  uint32_t instruction;
  for (int i = 0; i < 100; i++) {
    instruction = load_word(core->bus->dram, core->program_counter);
    printf("%x ", instruction & OPCODE);
    core->program_counter += 4;
  }

  free_core(core);

  // Check to see if instruction converted correctly
  // struct r_type *formatted_instr = instr_to_r(instruction);
  // printf("Opcode:\t %x\n", formatted_instr->opcode);
  // printf("Rd:\t %x\n", formatted_instr->rd);
  // printf("Function 3:\t %x\n", formatted_instr->func_3);
  // printf("Rs1:\t %x\n", formatted_instr->rs1);
  // printf("Rs2:\t %x\n", formatted_instr->rs2);
  // printf("Function 7:\t %x\n", formatted_instr->func_7);
  // free_r_type(formatted_instr);

  return 0;
}
