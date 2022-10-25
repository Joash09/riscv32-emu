#ifndef RV_FUNC_HASHMAP_H_
#define RV_FUNC_HASHMAP_H_

#include "rv_core.h"

struct func_nlist {
        struct func_nlist *next;
        int code;
        opcode_func_ptr func;
};

unsigned hash(int code, int hash_size);
struct func_nlist *hashtab_lookup(struct func_nlist *hashtab, int hash_size, int code);
struct func_nlist *hashtab_install(struct func_nlist *hashtab, int hash_size, int code, opcode_func_ptr func);

#endif // RV_FUNC_HASHMAP_H_
