#include "rv_func_hashmap.h"

unsigned hash(int code, int hash_size) {
        unsigned hashval = code; // Ensure unsigned number
        return hashval % hash_size;
}

/*
 * Start at index produced by hash and search list starting at that index
 */
struct func_nlist *hashtab_lookup(struct func_nlist *hashtab, int hash_size, int code) {
        struct func_nlist *np;

        for (np = &hashtab[hash(code, hash_size)]; np != NULL; np = np->next)
                if (code == np->code)
                        return np;

        return NULL; // (i.e. operation not specified)
}

/*
 * Start at hash index and search list beginning at index
 * If no element is found insert element at the start of the list
 */
struct func_nlist *hashtab_install(struct func_nlist *hashtab, int hash_size, int code, opcode_func_ptr func) {
        struct func_nlist *np;
        unsigned hashval;

        if ((np = hashtab_lookup(hashtab, hash_size, code)) == NULL) { /* Not found */
                np = (struct func_nlist *) malloc(sizeof(*np));
                np->code = code;
                np->func = func;
                hashval = hash(code, hash_size);
                np->next = &hashtab[hashval];
                hashtab[hashval] = *np;

        } else /* Already there; override existing function */
                np->func = func;

        return np;
}
