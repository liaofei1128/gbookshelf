#ifndef LIBMDFA_H
#define LIBMDFA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef REF_IGNORECASE
#define REF_IGNORECASE  0x00000001 /** 'i' flag */
#endif

#ifndef REF_SINGLELINE
#define REF_SINGLELINE  0x00000002 /** 's' flag */
#endif

#ifndef REF_MULTILINE
#define REF_MULTILINE   0x00000004 /** 'm' flag */
#endif

#ifndef REF_GLOBAL
#define REF_GLOBAL      0x00000008 /** 'g' flag */
#endif

typedef void mdfa_t;

/* libregex.c */
extern void dfa_destroy(mdfa_t *prog);
extern void *dfa_compile(char *regex, int flags);
extern void *mdfa_compile(int n, char **regex, int *flags);
extern int dfa_match(mdfa_t *prog, uint8_t *text, int len, int *start, int *end, int flags);
extern int dfa_nstate(mdfa_t *prog);

#endif
