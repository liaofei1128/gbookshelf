#ifndef _DICT_H_
#define _DICT_H_

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct tree_node_st {
    char *word, *lowercase, *meaning;
    struct tree_node_st *left, *right;
} tree_node_t;

/* dict.c */
extern int dict_init(void);
extern void dict_fini(void);
extern char *dict_search(char *word);

#endif
