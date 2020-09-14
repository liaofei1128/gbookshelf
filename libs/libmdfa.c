/**
 * @file libmdfa.c
 * @author liaoxf<liaofei1128@gmail.com>
 * @brief implement of regex2postfix, postfix2nfa, nfa2dfa and dfa_match.
 * @see http://swtch.com/~rsc/regexp/regexp1.html
 * @see Ken Thompson, Regular expression search algorithm, Communications of the ACM 11(6)(June 1968), pp. 419-422.
 * @see http://pubs.opengroup.org/onlinepubs/000095399/basedefs/xbd_chap09.html
 *      The Open Group Base Specifications Issue 6 IEEE Std 1003.1, 2004 Edition
 *      Section 9. Regular Expressions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "liblist.h"
#include "libmdfa.h"

#define metachar_inside   "\\^-[]"
#define metachar_outside  "\\^$.[|()?*+{"

/* using char set [0...255] */
#define N_CHARSET        256

static uint8_t g_default_charset[N_CHARSET] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
    0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
    0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
    0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
    0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
    0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
    0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff,
};

static uint8_t g_dot_charset[N_CHARSET - 1] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,     0x0b,0x0c,0x0d,0x0e,0x0f,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
    0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
    0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
    0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
    0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
    0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
    0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff,
};

static inline uint8_t hexval(uint8_t c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else {
        return 0;
    }
}

/**
 * RE flags
 */
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

#define REF_MATCHBOL    0x00000010
#define REF_MATCHEOL    0x00000020
#define REF_MATCHEPS    0x00000040
#define REF_MATCHOMG    0x00000080
#define REF_MULTIREG    0x00000100

static inline char *get_flags_name(int flags)
{
    int i = 0;
    static char names[32] = {0, };

    if (flags & REF_IGNORECASE)
        names[i++] = 'i';
    if (flags & REF_SINGLELINE)
        names[i++] = 's';
    if (flags & REF_MULTILINE)
        names[i++] = 'm';
    names[i++] = '\0';
    return names;
}

/**
 * RE operators
 */

enum {
    REO_ALT, // '|'
    REO_CAT, // ''
    REO_STAR, // '*'
    REO_PLUS, // '+'
    REO_QUEST, // '?'
};

static inline char *get_opname(int operator)
{
    switch (operator) {
    case REO_ALT:
        return "ALT";
    case REO_CAT:
        return "CAT";
    case REO_STAR:
        return "STAR";
    case REO_PLUS:
        return "PLUS";
    case REO_QUEST:
        return "QUEST";
    default:
        return NULL;
    }
}

static inline int get_precedence(int operator)
{
    switch (operator) {
    case REO_ALT:
        return 0;
    case REO_CAT:
        return 1;
    case REO_STAR:
        return 2;
    case REO_PLUS:
        return 2;
    case REO_QUEST:
        return 2;
    default:
        return 3;
    }
}

/**
 * RE parse node: exchar
 * exchar has many types as enumerated below, make sure don't overflow 16.
 *
 * the operand epsilon means match any character, we have
 *  - r<cat><epsilon> = r
 *  - <epsilon><cat>r = r
 *  - r<alt><epsilon> = r<quest>
 *  - <epsilon><alt>r = r<quest>
 *  - <epsilon><star> = <epsilon>
 *  - <epsilon><quest> = <epsilon>
 *  - <epsilon><plus> = <epsilon>
 * for example, "()", "r{0}" which can match the empty string.
 *
 * the operand omega means don't match any character, we have
 *  - r<cat><omega> = <omega>
 *  - <omega><cat>r = <omega>
 *  - r<alt><omega> = r
 *  - <omega><alt>r = r
 *  - <omega><star> = <epsilon>
 *  - <omega><quest> = <epsilon>
 *  - <omega><plus> = <omega>
 * for example, "[^\s\S]", "[^\w\W]", "[^\x00-\xff]" which can't match everything.
 *
 * Note:
 *  - <omega><cat><epsilon> = <omega>
 *  - <omega><alt><epsilon> = <omega><quest> = <epsilon>
 */

#define RET_IS_OPERAND(t)   (t >= RET_OPERAND_CHAR)
enum {
    RET_OPERATOR,
    RET_PARENTHESIS_LEFT,
    RET_PARENTHESIS_RIGHT,

    RET_OPERAND_CHAR, /* c */
    RET_OPERAND_RANGE, /* c-c1 */
    RET_OPERAND_OMEGA,
    RET_OPERAND_EPSILON,
    RET_OPERAND_DOTCHAR, /* except newline */
    RET_OPERAND_ANYCHAR, /* include newline */
    RET_OPERAND_DUALCHAR, /* c | c1 */
    RET_OPERAND_TRIADCHAR, /* c | c1 | c2 */
    RET_OPERAND_NOCASECHAR,

    RET_OPERAND_MAX
};

/**
 * RE exchar: 32-bit
 */
static inline void *re_exchar_create(uint8_t t, uint8_t c, uint8_t c1, uint8_t c2)
{
    intptr_t ptr = ((t | (c << 8) | (c1 << 16) | (c2 << 24)) & 0xFFFFFFFFLLU);
    return (void *)ptr;
}

static inline void re_exchar_get(void *ptr, uint8_t *t, uint8_t *c, uint8_t *c1, uint8_t *c2)
{
    unsigned long value = (unsigned long)ptr;

    *t = value & 0xff;
    *c = (value & 0xff00) >> 8;
    *c1 = (value & 0xff0000) >> 16;
    *c2 = (value & 0xff000000) >> 24;
}

static inline uint8_t re_exchar_type(void *ptr)
{
    unsigned long value = (unsigned long)ptr;
    return (value & 0xff);
}

static inline uint8_t re_exchar_char(void *ptr)
{
    unsigned long value = (unsigned long) ptr;
    return ((value & 0xff00) >> 8);
}

static inline uint8_t re_exchar_char1(void *ptr)
{
    unsigned long value = (unsigned long) ptr;
    return ((value & 0xff0000) >> 16);
}

static inline uint8_t re_exchar_char2(void *ptr)
{
    unsigned long value = (unsigned long) ptr;
    return ((value & 0xff000000) >> 24);
}

#ifdef DEBUG
#define WRAPFMT(fmt) "\033[1;30m<\033[0m\033[1;36m"fmt"\033[0m\033[1;30m>\033[0m"
static int re_exchar_dump(void *ptr, void *arg, int arg_len)
{
    uint8_t t, c, c1, c2;

    re_exchar_get(ptr, &t, &c, &c1, &c2);
    if (t == RET_OPERATOR) {
        fprintf(stdout, WRAPFMT("%s"), get_opname(c));
    } else if (t == RET_PARENTHESIS_LEFT) {
        fprintf(stdout, WRAPFMT("("));
    } else if (t == RET_PARENTHESIS_RIGHT) {
        fprintf(stdout, WRAPFMT(")"));
    } else if (t == RET_OPERAND_CHAR) {
        if (c > 0x20 && c <= 0x7f) {
            fprintf(stdout, WRAPFMT("%c"), c);
        } else {
            fprintf(stdout, WRAPFMT("\\x%02x"), c);
        }
    } else if (t == RET_OPERAND_RANGE) {
        fprintf(stdout, WRAPFMT("[\\x%02x-\\x%02x]"), c, c1);
    } else if (t == RET_OPERAND_OMEGA) {
        fprintf(stdout, WRAPFMT("OMG"));
    } else if (t == RET_OPERAND_EPSILON) {
        fprintf(stdout, WRAPFMT("EPS"));
    } else if (t == RET_OPERAND_DOTCHAR) {
        fprintf(stdout, WRAPFMT("DOT"));
    } else if (t == RET_OPERAND_ANYCHAR) {
        fprintf(stdout, WRAPFMT("ANY"));
    } else if (t == RET_OPERAND_DUALCHAR) {
        fprintf(stdout, WRAPFMT("\\x%02x|\\x%02x"), c, c1);
    } else if (t == RET_OPERAND_TRIADCHAR) {
        fprintf(stdout, WRAPFMT("\\x%02x|\\x%02x|\\x%02x"), c, c1, c2);
    } else if (t == RET_OPERAND_NOCASECHAR) {
        fprintf(stdout, WRAPFMT("NC:%c"), c);
    } else {
        fprintf(stdout, WRAPFMT("UNKNOWN%d"), t);
    }

    return 0;
}
#endif

static inline int re_append_exchar(dlist_t *relist, uint8_t t, uint8_t c, uint8_t c1, uint8_t c2)
{
    return dlist_add_tail(relist, re_exchar_create(t, c, c1, c2));
}

static inline int re_append_left_parenthesis(dlist_t *relist)
{
    return re_append_exchar(relist, RET_PARENTHESIS_LEFT, 0, 0, 0);
}

static inline int re_append_right_parenthesis(dlist_t *relist)
{
    return re_append_exchar(relist, RET_PARENTHESIS_RIGHT, 0, 0, 0);
}

static inline int re_append_operator(dlist_t *relist, uint8_t operator)
{
    return re_append_exchar(relist, RET_OPERATOR, operator, 0, 0);
}

static inline int re_append_char(dlist_t *relist, uint8_t c)
{
    return re_append_exchar(relist, RET_OPERAND_CHAR, c, 0, 0);
}

static inline int re_append_range(dlist_t *relist, uint8_t c, uint8_t c1)
{
    return re_append_exchar(relist, RET_OPERAND_RANGE, c, c1, 0);
}

static inline int re_append_omega(dlist_t *relist)
{
    return re_append_exchar(relist, RET_OPERAND_OMEGA, 0, 0, 0);
}

static inline int re_append_epsilon(dlist_t *relist)
{
    return re_append_exchar(relist, RET_OPERAND_EPSILON, 0, 0, 0);
}

static inline int re_append_dotchar(dlist_t *relist)
{
    return re_append_exchar(relist, RET_OPERAND_DOTCHAR, 0, 0, 0);
}

static inline int re_append_anychar(dlist_t *relist)
{
    return re_append_exchar(relist, RET_OPERAND_ANYCHAR, 0, 0, 0);
}

static inline int re_append_dualchar(dlist_t *relist, uint8_t c, uint8_t c1)
{
    return re_append_exchar(relist, RET_OPERAND_DUALCHAR, c, c1, 0);
}

static inline int re_append_triadchar(dlist_t *relist, uint8_t c, uint8_t c1, uint8_t c2)
{
    return re_append_exchar(relist, RET_OPERAND_TRIADCHAR, c, c1, c2);
}

static inline int re_append_nocasechar(dlist_t *relist, uint8_t c)
{
    if (isupper(c)) {
        return re_append_exchar(relist, RET_OPERAND_NOCASECHAR, tolower(c), 0, 0);
    } else if (islower(c)) {
        return re_append_exchar(relist, RET_OPERAND_NOCASECHAR, c, 0, 0);
    } else {
        return re_append_exchar(relist, RET_OPERAND_CHAR, c, 0, 0);
    }

    return -1;
}

static inline int re_append_incasechar(dlist_t *relist, uint8_t c, int nocase)
{
    if (nocase) {
        return re_append_nocasechar(relist, c);
    } else {
        return re_append_char(relist, c);
    }

    return 0;
}

enum {
    NFA_STATE_TYPE_LITER,
    NFA_STATE_TYPE_DOTCH,
    NFA_STATE_TYPE_ANYCH,
    NFA_STATE_TYPE_RANGE,
    NFA_STATE_TYPE_DUAL,
    NFA_STATE_TYPE_TRIAD,
    NFA_STATE_TYPE_NOCASE,
    NFA_STATE_TYPE_SPLIT,
    NFA_STATE_TYPE_MATCH,
};

typedef struct nfa_state_st {
    uint8_t type:4;
    uint8_t marked:1;
    uint8_t bol:1;
    uint8_t eol:1;
    uint8_t ml:1;
    uint8_t c;
    uint8_t c1;
    uint8_t c2;
    uint32_t pid; /** the pattern id, for multi-pattern */
    struct nfa_state_st *out;
    struct nfa_state_st *out1;
} nfa_state_t;

typedef struct nfa_outlist_st {
    struct nfa_outlist_st *next;
} nfa_outlist_t;

typedef struct nfa_fragment_st {
    nfa_state_t *start;
    nfa_outlist_t *outlist;
} nfa_fragment_t;

struct nfa_st {
    uint32_t flags;
    uint32_t pid;
    uint32_t nstate;
    uint32_t nchar;
    uint8_t *charset;
    nfa_state_t *start;
};

typedef struct dfa_state_st {
    uint32_t pid:28;
    uint32_t bol:1;
    uint32_t eol:1;
    uint32_t ml:1;
    uint32_t accept:1;
    slist_t *nfa_set;
    struct dfa_state_st *next[N_CHARSET];
} dfa_state_t;

struct dfa_st {
    int flags;
    filo_t states;
    dfa_state_t *start;
};

static int regex_accept_alnum(char *accept_maps, int nocase, int negate)
{
    int i;

    if (!negate) {
        for (i=0x30; i<=0x39; i++)
            accept_maps[i] = 1; /* '0' - '9' */
        for (i=0x41; i<=0x5A; i++)
            accept_maps[i] = 1; /* 'A' - 'Z' */
        for (i=0x61; i<=0x7A; i++)
            accept_maps[i] = 1; /* 'a' - 'z' */
    } else {
        for (i=0x00; i<=0x2f; i++)
            accept_maps[i] = 1;
        for (i=0x3A; i<=0x40; i++)
            accept_maps[i] = 1;
        for (i=0x5B; i<=0x60; i++)
            accept_maps[i] = 1;
        for (i=0x7B; i<=0xFF; i++)
            accept_maps[i] = 1;
    }

    return 0;
}

static int regex_accept_alpha(char *accept_maps, int nocase, int negate)
{
    int i;

    if (!negate) {
        for (i=0x41; i<=0x5A; i++)
            accept_maps[i] = 1; /* 'A' - 'Z' */
        for (i=0x61; i<=0x7A; i++)
            accept_maps[i] = 1; /* 'a' - 'z' */
    } else {
        for (i=0x00; i<=0x40; i++)
            accept_maps[i] = 1;
        for (i=0x5B; i<=0x60; i++)
            accept_maps[i] = 1;
        for (i=0x7B; i<=0xFF; i++)
            accept_maps[i] = 1;
    }

    return 0;
}

static int regex_accept_ascii(char *accept_maps, int nocase, int negate)
{
    int i;

    if (!negate) {
        for (i=0x00; i<=0x7F; i++)
            accept_maps[i] = 1;
    } else {
        for (i=0x80; i<=0xFF; i++)
            accept_maps[i] = 1;
    }

    return 0;
}

static int regex_accept_blank(char *accept_maps, int nocase, int negate)
{
    int i;

    if (!negate) {
        accept_maps[0x09] = 1;
        accept_maps[0x20] = 1;
    } else {
        for (i=0x00; i<=0x08; i++)
            accept_maps[i] = 1;
        for (i=0x0A; i<=0x1F; i++)
            accept_maps[i] = 1;
        for (i=0x21; i<=0xFF; i++)
            accept_maps[i] = 1;
    }

    return 0;
}

static int regex_accept_cntrl(char *accept_maps, int nocase, int negate)
{
    int i;

    if (!negate) {
        for (i=0x00; i<=0x1F; i++)
            accept_maps[i] = 1;
        accept_maps[0x7F] = 1;
    } else {
        for (i=0x20; i<=0x7E; i++)
            accept_maps[i] = 1;
        for (i=0x80; i<=0xFF; i++)
            accept_maps[i] = 1;
    }

    return 0;
}

static int regex_accept_digit(char *accept_maps, int nocase, int negate)
{
    int i;

    if (!negate) {
        for (i=0x30; i<=0x39; i++)
            accept_maps[i] = 1; /* '0' - '9' */
    } else {
        for (i=0x00; i<=0x2F; i++)
            accept_maps[i] = 1;
        for (i=0x3A; i<=0xFF; i++)
            accept_maps[i] = 1;
    }

    return 0;
}

static int regex_accept_graph(char *accept_maps, int nocase, int negate)
{
    int i;

    if (!negate) {
        for (i=0x21; i<=0x7E; i++)
            accept_maps[i] = 1;
    } else {
        for (i=0x00; i<=0x20; i++)
            accept_maps[i] = 1;
        for (i=0x7F; i<=0xFF; i++)
            accept_maps[i] = 1;
    }

    return 0;
}

static int regex_accept_lower(char *accept_maps, int nocase, int negate)
{
    int i;

    if (!negate) {
        for (i=0x61; i<=0x7A; i++)
            accept_maps[i] = 1; /* 'a' - 'z' */
        if (nocase) {
            for (i=0x41; i<=0x5A; i++)
                accept_maps[i] = 1; /* 'A' - 'Z' */
        }
    } else {
        for (i=0x00; i<=0x60; i++)
            accept_maps[i] = 1;
        for (i=0x7B; i<=0xFF; i++)
            accept_maps[i] = 1;
        if (nocase) {
            for (i=0x41; i<=0x5A; i++)
                accept_maps[i] = 0;
        }
    }

    return 0;
}

static int regex_accept_print(char *accept_maps, int nocase, int negate)
{
    int i;

    if (!negate) {
        for (i=0x20; i<=0x7E; i++)
            accept_maps[i] = 1;
    } else {
        for (i=0x00; i<=0x1F; i++)
            accept_maps[i] = 1;
        for (i=0x7F; i<=0xFF; i++)
            accept_maps[i] = 1;
    }

    return 0;
}

static int regex_accept_punct(char *accept_maps, int nocase, int negate)
{
    int i;

    if (!negate) {
        for (i=0x20; i<=0x2F; i++)
            accept_maps[i] = 1;
        for (i=0x3A; i<=0x40; i++)
            accept_maps[i] = 1;
        for (i=0x5B; i<=0x60; i++)
            accept_maps[i] = 1;
        for (i=0x7B; i<=0x7E; i++)
            accept_maps[i] = 1;
    } else {
        for (i=0x00; i<=0x1F; i++)
            accept_maps[i] = 1;
        for (i=0x30; i<=0x39; i++)
            accept_maps[i] = 1;
        for (i=0x41; i<=0x5A; i++)
            accept_maps[i] = 1;
        for (i=0x61; i<=0x7A; i++)
            accept_maps[i] = 1;
        for (i=0x7F; i<=0xFF; i++)
            accept_maps[i] = 1;
    }

    return 0;
}

static int regex_accept_space(char *accept_maps, int nocase, int negate)
{
    int i;

    if (!negate) {
        for (i=0x09; i<=0x0D; i++)
            accept_maps[i] = 1;
        accept_maps[0x20] = 1;
    } else {
        for (i=0x00; i<=0x08; i++)
            accept_maps[i] = 1;
        for (i=0x0E; i<=0x1F; i++)
            accept_maps[i] = 1;
        for (i=0x21; i<=0xFF; i++)
            accept_maps[i] = 1;
    }

    return 0;
}

static int regex_accept_upper(char *accept_maps, int nocase, int negate)
{
    int i;

    if (!negate) {
        for (i=0x41; i<=0x5A; i++)
            accept_maps[i] = 1; /* 'A' - 'Z' */
        if (nocase) {
            for (i=0x61; i<=0x7A; i++)
                accept_maps[i] = 1; /* 'a' - 'z' */
        }
    } else {
        for (i=0x00; i<=0x40; i++)
            accept_maps[i] = 1;
        for (i=0x5B; i<=0xFF; i++)
            accept_maps[i] = 1;
        if (nocase) {
            for (i=0x61; i<=0x7A; i++)
                accept_maps[i] = 0;
        }
    }

    return 0;
}

static int regex_accept_word(char *accept_maps, int nocase, int negate)
{
    int i;

    if (!negate) {
        for (i=0x30; i<=0x39; i++)
            accept_maps[i] = 1; /* '0' - '9' */
        for (i=0x41; i<=0x5A; i++)
            accept_maps[i] = 1; /* 'A' - 'Z' */
        for (i=0x61; i<=0x7A; i++)
            accept_maps[i] = 1; /* 'a' - 'z' */
        accept_maps['_'] = 1;
    } else {
        for (i=0x00; i<=0x2F; i++)
            accept_maps[i] = 1;
        for (i=0x3A; i<=0x40; i++)
            accept_maps[i] = 1;
        for (i=0x5B; i<=0x5E; i++)
            accept_maps[i] = 1;
        accept_maps[0x60] = 1;
        for (i=0x7B; i<=0xFF; i++)
            accept_maps[i] = 1;
    }

    return 0;
}

static int regex_accept_xdigit(char *accept_maps, int nocase, int negate)
{
    int i;

    if (!negate) {
        for (i=0x30; i<=0x39; i++)
            accept_maps[i] = 1; /* '0' - '9' */
        for (i=0x41; i<=0x46; i++)
            accept_maps[i] = 1; /* 'A' - 'F' */
        for (i=0x61; i<=0x66; i++)
            accept_maps[i] = 1; /* 'a' - 'f' */
    } else {
        for (i=0x00; i<=0x2F; i++)
            accept_maps[i] = 1;
        for (i=0x3A; i<=0x40; i++)
            accept_maps[i] = 1;
        for (i=0x47; i<=0x60; i++)
            accept_maps[i] = 1;
        for (i=0x67; i<=0xFF; i++)
            accept_maps[i] = 1;
    }

    return 0;
}

static int regex_append_accept(dlist_t *relist, char *accept_maps)
{
    int n;
    int ret;
    int i, j, k;
    void *v;

    ret = re_append_left_parenthesis(relist);
    if (ret < 0)
        return ret;

    i = j = n = 0;
    while (i < N_CHARSET) {
        while (i < N_CHARSET && accept_maps[i] == 0)
            i++;
        if (i == N_CHARSET)
            break;
        j = i;
        while (i < N_CHARSET && accept_maps[i] == 1)
            i++;
        if (i - 1 >= j) {
            if (i - 1 == j) {
                ret = re_append_char(relist, j);
                if (ret < 0)
                    return ret;
                n++;
            } else {
                ret = re_append_range(relist, j, i - 1);
                if (ret < 0)
                    return ret;
                n++;
            }

            for (k=i; k<N_CHARSET; k++) {
                if (accept_maps[k]) {
                    ret = re_append_operator(relist, REO_ALT);
                    if (ret < 0)
                        return ret;
                    break;
                }
            }
        }
    }

    if (n > 0) {
        ret = re_append_right_parenthesis(relist);
        if (ret < 0)
            return ret;
    } else { /** delete the prev '(' and append an epsilon operand */
        dlist_del_tail(relist, &v);
        re_append_epsilon(relist);
    }

    return 0;
}

static int regex_append_digit(dlist_t *relist, int nocase, int negate)
{
    char accept_maps[N_CHARSET] = {0, };

    regex_accept_digit(accept_maps, nocase, negate);
    return regex_append_accept(relist, accept_maps);
}

static int regex_append_space(dlist_t *relist, int nocase, int negate)
{
    char accept_maps[N_CHARSET] = {0, };

    regex_accept_space(accept_maps, nocase, negate);
    return regex_append_accept(relist, accept_maps);
}

static int regex_append_word(dlist_t *relist, int nocase, int negate)
{
    char accept_maps[N_CHARSET] = {0, };

    regex_accept_word(accept_maps, nocase, negate);
    return regex_append_accept(relist, accept_maps);
}

static int regex_accept_posix_class(uint8_t *regex, uint8_t **start, char *accept_maps, int nocase)
{
    uint8_t *p = (*start);

    if (!memcmp(p, ":alnum:]", 8)) {
        regex_accept_alnum(accept_maps, nocase, 0);
        p+=8; *start = p; return 0;
    } else if (!memcmp(p, ":alpha:]", 8)) {
        regex_accept_alpha(accept_maps, nocase, 0);
        p+=8; *start = p; return 0;
    } else if (!memcmp(p, ":ascii:]", 8)) {
        regex_accept_ascii(accept_maps, nocase, 0);
        p+=8; *start = p; return 0;
    } else if (!memcmp(p, ":blank:]", 8)) {
        regex_accept_blank(accept_maps, nocase, 0);
        p+=8; *start = p; return 0;
    } else if (!memcmp(p, ":cntrl:]", 8)) {
        regex_accept_cntrl(accept_maps, nocase, 0);
        p+=8; *start = p; return 0;
    } else if (!memcmp(p, ":digit:]", 8)) {
        regex_accept_digit(accept_maps, nocase, 0);
        p+=8; *start = p; return 0;
    } else if (!memcmp(p, ":graph:]", 8)) {
        regex_accept_graph(accept_maps, nocase, 0);
        p+=8; *start = p; return 0;
    } else if (!memcmp(p, ":lower:]", 8)) {
        regex_accept_lower(accept_maps, nocase, 0);
        p+=8; *start = p; return 0;
    } else if (!memcmp(p, ":print:]", 8)) {
        regex_accept_print(accept_maps, nocase, 0);
        p+=8; *start = p; return 0;
    } else if (!memcmp(p, ":punct:]", 8)) {
        regex_accept_punct(accept_maps, nocase, 0);
        p+=8; *start = p; return 0;
    } else if (!memcmp(p, ":space:]", 8)) {
        regex_accept_space(accept_maps, nocase, 0);
        p+=8; *start = p; return 0;
    } else if (!memcmp(p, ":upper:]", 8)) {
        regex_accept_upper(accept_maps, nocase, 0);
        p+=8; *start = p; return 0;
    } else if (!memcmp(p, ":word:]", 7)) {
        regex_accept_word(accept_maps, nocase, 0);
        p+=7; *start = p; return 0;
    } else if (!memcmp(p, ":xdigit:]", 9)) {
        regex_accept_xdigit(accept_maps, nocase, 0);
        p+=9; *start = p; return 0;
    } else if (!memcmp(p, ":^alnum:]", 9)) {
        regex_accept_alnum(accept_maps, nocase, 1);
        p+=9; *start = p; return 0;
    } else if (!memcmp(p, ":^alpha:]", 9)) {
        regex_accept_alpha(accept_maps, nocase, 1);
        p+=9; *start = p; return 0;
    } else if (!memcmp(p, ":^ascii:]", 9)) {
        regex_accept_ascii(accept_maps, nocase, 1);
        p+=9; *start = p; return 0;
    } else if (!memcmp(p, ":^blank:]", 9)) {
        regex_accept_blank(accept_maps, nocase, 1);
        p+=9; *start = p; return 0;
    } else if (!memcmp(p, ":^cntrl:]", 9)) {
        regex_accept_cntrl(accept_maps, nocase, 1);
        p+=9; *start = p; return 0;
    } else if (!memcmp(p, ":^digit:]", 9)) {
        regex_accept_digit(accept_maps, nocase, 1);
        p+=9; *start = p; return 0;
    } else if (!memcmp(p, ":^graph:]", 9)) {
        regex_accept_graph(accept_maps, nocase, 1);
        p+=9; *start = p; return 0;
    } else if (!memcmp(p, ":^lower:]", 9)) {
        regex_accept_lower(accept_maps, nocase, 1);
        p+=9; *start = p; return 0;
    } else if (!memcmp(p, ":^print:]", 9)) {
        regex_accept_print(accept_maps, nocase, 1);
        p+=9; *start = p; return 0;
    } else if (!memcmp(p, ":^punct:]", 9)) {
        regex_accept_punct(accept_maps, nocase, 1);
        p+=9; *start = p; return 0;
    } else if (!memcmp(p, ":^space:]", 9)) {
        regex_accept_space(accept_maps, nocase, 1);
        p+=9; *start = p; return 0;
    } else if (!memcmp(p, ":^upper:]", 9)) {
        regex_accept_upper(accept_maps, nocase, 1);
        p+=9; *start = p; return 0;
    } else if (!memcmp(p, ":^word:]", 8)) {
        regex_accept_word(accept_maps, nocase, 1);
        p+=8; *start = p; return 0;
    } else if (!memcmp(p, ":^xdigit:]", 10)) {
        regex_accept_xdigit(accept_maps, nocase, 1);
        p+=10; *start = p; return 0;
    }

    return -1;
}

static int regex_parse_escape_outside(uint8_t *regex, uint8_t **start, dlist_t *relist, int nocase)
{
    int ret;
    uint8_t a, b, c;
    uint8_t *p = (*start);

    p++;
    if (*p == '\0') {
        fprintf(stderr, "need more escaped char at pos %d\n", (int)(p - regex));
        return -1;
    }

    if (strchr(metachar_outside, *p)) {
        ret = re_append_char(relist, *p);
        if (ret < 0)
            return ret;
        p++; *start = p; return 0;
    }

    if (*p == 'x') {
        p++;
        if (*p == '\0') {
            fprintf(stderr, "need more hex char at pos %d\n", (int)(p - regex));
            return -1;
        }
        a = *p;
        if (!isxdigit(a)) {
            fprintf(stderr, "invalid hex char at pos %d\n", (int)(p - regex));
            return -1;
        }
        p++;
        if (*p == '\0') {
            fprintf(stderr, "need more hex char at pos %d\n", (int)(p - regex));
            return -1;
        }
        b = *p;
        if (!isxdigit(b)) {
            fprintf(stderr, "invalid hex char at pos %d\n", (int)(p - regex));
            return -1;
        }
        ret = re_append_incasechar(relist, (hexval(a) << 4) | hexval(b), nocase);
        if (ret < 0)
            return ret;
        p++; *start = p; return 0;
    }

    if (*p == '0' || *p == '1' || *p == '2' || *p == '3') {
        a = *p;

        p++;
        if (*p == '\0') {
            fprintf(stderr, "need more octal char at pos %d\n", (int)(p - regex));
            return -1;
        }
        b = *p;
        if (!(b>='0' && b<'8')) {
            fprintf(stderr, "invalid octal char at pos %d\n", (int)(p - regex));
            return -1;
        }

        p++;
        if (*p == '\0') {
            fprintf(stderr, "need more octal char at pos %d\n", (int)(p - regex));
            return -1;
        }
        c = *p;
        if (!(c>='0' && c<'8')) {
            fprintf(stderr, "invalid octal char at pos %d\n", (int)(p - regex));
            return -1;
        }

        ret = re_append_incasechar(relist, ((a - '0') << 6) | ((b - '0') << 3) | (c - '0'), nocase);
        if (ret < 0)
            return ret;
        p++; *start = p; return 0;
    }

    if (*p == 'd') {
        ret = regex_append_digit(relist, nocase, 0);
        if (ret < 0)
            return ret;
        p++; *start = p; return 0;
    }
    if (*p == 'D') {
        ret = regex_append_digit(relist, nocase, 1);
        if (ret < 0)
            return ret;
        p++; *start = p; return 0;
    }

    if (*p == 'w') {
        ret = regex_append_word(relist, nocase, 0);
        if (ret < 0)
            return ret;
        p++; *start = p; return 0;
    }
    if (*p == 'W') {
        ret = regex_append_word(relist, nocase, 1);
        if (ret < 0)
            return ret;
        p++; *start = p; return 0;
    }

    if (*p == 's') {
        ret = regex_append_space(relist, nocase, 0);
        if (ret < 0)
            return ret;
        p++; *start = p; return 0;
    }
    if (*p == 'S') {
        ret = regex_append_space(relist, nocase, 1);
        if (ret < 0)
            return ret;
        p++; *start = p; return 0;
    }

    if (*p == 'c') {
        p++;
        if (*p == '\0') {
            fprintf(stderr, "need more control-x char at pos %d\n", (int)(p - regex));
            return -1;
        }
        a = toupper(*p);

        if (a & 0x40) {
            a = a & (~0x40);
        } else {
            a = a | 0x40;
        }

        ret = re_append_incasechar(relist, a, nocase);
        if (ret < 0)
            return ret;
        p++; *start = p; return 0;
    }

    if (*p == 'Q') {
        p++;
        if (*p == '\0') {
            fprintf(stderr, "need more verbatim char at pos %d\n", (int)(p - regex));
            return -1;
        }

        while (*p && (*p != '\\' || *(p + 1) != 'E')) {
            ret = re_append_char(relist, *p);
            if (ret < 0)
                return ret;
            p++;
        }
        if (*p == '\0' || *(p+1) == '\0' || (*p != '\\' && *(p+1) != 'E')) {
            fprintf(stderr, "invalid verbatim mode at pos %d\n", (int)(p - regex));
            return -1;
        }
        p+=2; *start = p; return 0;
    }

    if (*p == 'U') {
        p++;
        if (*p == '\0') {
            fprintf(stderr, "need more upper verbatim char at pos %d\n", (int)(p - regex));
            return -1;
        }

        while (*p && (*p != '\\' || *(p + 1) != 'E')) {
            a = toupper(*p);
            ret = re_append_char(relist, a);
            if (ret < 0)
                return ret;
            p++;
        }
        if (*p == '\0' || *(p+1) == '\0' || (*p != '\\' && *(p+1) != 'E')) {
            fprintf(stderr, "invalid verbatim mode at pos %d\n", (int)(p - regex));
            return -1;
        }
        p+=2; *start = p; return 0;
    }

    if (*p == 'L') {
        p++;
        if (*p == '\0') {
            fprintf(stderr, "need more lower verbatim char at pos %d\n", (int)(p - regex));
            return -1;
        }

        while (*p && (*p != '\\' || *(p + 1) != 'E')) {
            a = tolower(*p);
            ret = re_append_char(relist, a);
            if (ret < 0)
                return ret;
            p++;
        }
        if (*p == '\0' || *(p+1) == '\0' || (*p != '\\' && *(p+1) != 'E')) {
            fprintf(stderr, "invalid verbatim mode at pos %d\n", (int)(p - regex));
            return -1;
        }
        p+=2; *start = p; return 0;
    }

#define re_append_escape_char(dlist, c, v) \
    if (*p == c) {\
        ret = re_append_char(dlist, v);\
        if (ret < 0) \
            return -1; \
        p++; *start = p; return 0; \
    }

    re_append_escape_char(relist, 'a', 0x07);
    re_append_escape_char(relist, 'b', '\b');
    re_append_escape_char(relist, 'e', 0x1B);
    re_append_escape_char(relist, 'f', '\f');
    re_append_escape_char(relist, 'n', '\n');
    re_append_escape_char(relist, 'r', '\r');
    re_append_escape_char(relist, 't', '\t');
    re_append_escape_char(relist, 'v', '\v');

    /** others escaped char taken as the ordinary char */
    ret = re_append_incasechar(relist, *p, nocase);
    if (ret < 0)
        return ret;
    p++; *start = p; return 0;
}

static int regex_parse_escape_inside(uint8_t *regex, uint8_t **start, uint8_t *value)
{
    uint8_t a, b, c;
    uint8_t *p = (*start);

    p++;
    if (strchr(metachar_inside, *p)) {
        *value = *p;
        p++; *start = p; return 0;
    } else if (*p == 'a') {
        *value = 0x07;
        p++; *start = p; return 0;
    } else if (*p == 'b') {
        *value = 0x08;
        p++; *start = p; return 0;
    } else if (*p == 'e') {
        *value = 0x1B;
        p++; *start = p; return 0;
    } else if (*p == 'f') {
        *value = 0x0C;
        p++; *start = p; return 0;
    } else if (*p == 'n') {
        *value = 0x0A;
        p++; *start = p; return 0;
    } else if (*p == 'r') {
        *value = 0x0D;
        p++; *start = p; return 0;
    } else if (*p == 't') {
        *value = 0x09;
        p++; *start = p; return 0;
    } else if (*p == 'v') {
        *value = 0x11;
        p++; *start = p; return 0;
    } else if (*p == 'c') {
        p++;
        if (*p == '\0') {
            fprintf(stderr, "need more control-x char at pos %d\n", (int)(p - regex));
            return -1;
        }
        a = toupper(*p);
        if (a & 0x40) {
            a = a & (~0x40);
        } else {
            a = a | 0x40;
        }
        *value = a;
        p++; *start = p; return 0;
    } else if (*p == '0' || *p == '1' || *p == '2' || *p == '3') {
        a = *p;

        p++;
        if (*p == '\0') {
            fprintf(stderr, "need more octal char at pos %d\n", (int)(p - regex));
            return -1;
        }
        b = *p;
        if (!(b>='0' && b<'8')) {
            fprintf(stderr, "invalid octal char at pos %d\n", (int)(p - regex));
            return -1;
        }

        p++;
        if (*p == '\0') {
            fprintf(stderr, "need more octal char at pos %d\n", (int)(p - regex));
            return -1;
        }
        c = *p;
        if (!(c>='0' && c<'8')) {
            fprintf(stderr, "invalid octal char at pos %d\n", (int)(p - regex));
            return -1;
        }

        *value = ((a - '0') << 6) | ((b - '0') << 3) | (c - '0');
        p++; *start = p; return 0;
    } else if (*p == 'x') {
        p++;
        if (*p == '\0') {
            fprintf(stderr, "need more hex char at pos %d\n", (int)(p - regex));
            return -1;
        }
        a = *p;
        if (!isxdigit(a)) {
            fprintf(stderr, "invalid hex char at pos %d\n", (int)(p - regex));
            return -1;
        }
        p++;
        if (*p == '\0') {
            fprintf(stderr, "need more hex char at pos %d\n", (int)(p - regex));
            return -1;
        }
        b = *p;
        if (!isxdigit(b)) {
            fprintf(stderr, "invalid hex char at pos %d\n", (int)(p - regex));
            return -1;
        }
        *value = (hexval(a) << 4) | hexval(b);
        p++; *start = p; return 0;
    }

    return -2;
}

static int regex_parse_range_end(uint8_t *regex, uint8_t **start, uint8_t low, char *select_maps, int nocase)
{
    int i;
    int ret;
    uint8_t high;
    uint8_t *p = (*start);

    if (*p == '-') {
        p++;
        if (*p == '\0') {
            fprintf(stderr, "need more range char at pos %d\n", (int)(p - regex));
            return -1;
        } else if (*p == ']') {
            fprintf(stderr, "need more range char at pos %d\n", (int)(p - regex));
            return -1;
        } else if (*p == '\\') {
            ret = regex_parse_escape_inside(regex, &p, &high);
            if (ret == -1) {
                return -1;
            } else if (ret == 0) {
                if (high < low) {
                    fprintf(stderr, "invalid range [%d, %d] at pos %d\n", low, high, (int)(p - regex));
                    return -1;
                }

                for (i=low; i<=high; i++) {
                    select_maps[i] = 1;
                    if (nocase) {
                        if (islower(i))
                            select_maps[toupper(i)] = 1;
                        if (isupper(i))
                            select_maps[tolower(i)] = 1;
                    }
                }
                *start = p; return 0;
            } else if (ret == -2) {
                fprintf(stderr, "invalid end of range at pos %d\n", (int)(p - regex));
                return -1;
            }
        } else if (strchr(metachar_inside, *p)) {
            fprintf(stderr, "need escape at pos %d\n", (int)(p - regex));
            return -1;
        } else {
            high = *p;
            if (high < low) {
                fprintf(stderr, "invalid range at pos %d\n", (int)(p - regex));
                return -1;
            }

            for (i=low; i<=high; i++) {
                select_maps[i] = 1;
                if (nocase) {
                    if (islower(i))
                        select_maps[toupper(i)] = 1;
                    if (isupper(i))
                        select_maps[tolower(i)] = 1;
                }
            }
            p++; *start = p; return 0;
        }
    } else {
        select_maps[(int)low] = 1;
        if (nocase) {
            if (islower(low))
                select_maps[toupper(low)] = 1;
            if (isupper(low))
                select_maps[tolower(low)] = 1;
        }

        *start = p; return 0;
    }

    return -1;
}

static int regex_parse_class(uint8_t *regex, uint8_t **start, dlist_t *relist, int nocase)
{
    int i;
    int ret;
    uint8_t *p = (*start);
    uint8_t low;
    int inverse = 0;
    char select_maps[N_CHARSET] = {0, };

    p++;
    if (*p == '\0') {
        fprintf(stderr, "need more char in class at pos %d\n", (int)(p - regex));
        return -1;
    } else if (*p == ']') {
        /** take "]" followed "[" as an ordinary char */
        select_maps[(int)*p] = 1;
        p++;
    } else if (*p == '-') {
        /** take "-" followed "[" as an ordinary char */
        select_maps[(int)*p] = 1;
        p++;
    } else if (*p == '^') {
        inverse = 1;
        p++;
        if (*p == '\0') {
            fprintf(stderr, "need more char in class at pos %d\n", (int)(p - regex));
            return -1;
        } else if (*p == ']') {
            /** take "]" followed "[^" as an ordinary char */
            select_maps[(int)*p] = 1;
            p++;
        } else if (*p == '-') {
            /** take "-" followed "[^" as an ordinary char */
            select_maps[(int)*p] = 1;
            p++;
        }
    } else if (*p == ':') {
        ret = regex_accept_posix_class(regex, &p, select_maps, nocase);
        if (ret == 0) {
            *start = p;
            goto append_out;
        }
        p = (*start);
        p++;
    }

    while (*p) {
        if (*p == '\\') {
            ret = regex_parse_escape_inside(regex, &p, &low);
            if (ret == -1) {
                return -1;
            } else if (ret == 0) {
                if (*p == '\0') {
                    fprintf(stderr, "need class close char ']' at pos %d\n", (int)(p - regex));
                    return -1;
                }

                ret = regex_parse_range_end(regex, &p, low, select_maps, nocase);
                if (ret == -1) {
                    return -1;
                } else if (ret == 0) {
                    continue;
                }
            } else if (ret == -2) {
                p++;
                if (*p == '\0') {
                    fprintf(stderr, "need more char in class at pos %d\n", (int)(p - regex));
                    return -1;
                }

                if (*p == 'd') {
                    regex_accept_digit(select_maps, nocase, 0);
                    p++; continue;
                } else if (*p == 'D') {
                    regex_accept_digit(select_maps, nocase, 1);
                    p++; continue;
                } else if (*p == 's') {
                    regex_accept_space(select_maps, nocase, 0);
                    p++; continue;
                } else if (*p == 'S') {
                    regex_accept_space(select_maps, nocase, 1);
                    p++; continue;
                } else if (*p == 'w') {
                    regex_accept_word(select_maps, nocase, 0);
                    p++; continue;
                } else if (*p == 'W') {
                    regex_accept_word(select_maps, nocase, 1);
                    p++; continue;
                } else {
                    fprintf(stderr, "not support escaped char in class at pos %d\n", (int)(p - regex));
                    return -1;
                }
            }
        } else if (*p == '[') {
            fprintf(stderr, "not support sub-class at pos %d\n", (int)(p - regex));
            return -1;
        } else if (*p == ']') {
            break;
        } else if (strchr(metachar_inside, *p)){
            fprintf(stderr, "need escape at pos %d\n", (int)(p - regex));
            return -1;
        } else {
            low = *p;

            p++;
            if (*p == '\0') {
                fprintf(stderr, "need class close char ']' at pos %d\n", (int)(p - regex));
                return -1;
            }

            ret = regex_parse_range_end(regex, &p, low, select_maps, nocase);
            if (ret == -1) {
                return -1;
            } else if (ret == 1) {
                continue;
            }
        }
    }

    if (*p != ']') {
        fprintf(stderr, "missing char ']' to close the class at pos %d\n", (int)(p - regex));
        return -1;
    }

    p++;
    *start = p;

    if (inverse) {
        int omega = 0;
        for (i=0; i<N_CHARSET; i++) {
            if (select_maps[i]) {
                select_maps[i] = 0;
                omega++;
            } else {
                select_maps[i] = 1;
            }
        }

        if (omega == N_CHARSET) {
            return re_append_omega(relist);
        }
    }

append_out:
    return regex_append_accept(relist, select_maps);
}

static int regex_truncate_last_block(dlist_t *relist, dlist_t *lastblk)
{
    int blk = 0;
    int only_omega = 0;
    int only_epsilon = 0;

    void *v;
    uint8_t t, c, c1, c2;

    if (dlist_peek_tail(relist, &v)) {
        re_exchar_get(v, &t, &c, &c1, &c2);
        if (t == RET_OPERATOR) {
            return 0;
        } else if (t == RET_PARENTHESIS_LEFT) {
            return 0;
        }
    } else {
        return 0;
    }

    while (dlist_peek_tail(relist, &v)) {
        re_exchar_get(v, &t, &c, &c1, &c2);
        if (t == RET_PARENTHESIS_RIGHT) {
            blk++;
            dlist_add(lastblk, v);
            dlist_del_tail(relist, &v);
            continue;
        } else if (t == RET_PARENTHESIS_LEFT) {
            blk--;
            dlist_add(lastblk, v);
            dlist_del_tail(relist, &v);
            if (blk)
                continue;
            else
                break;
        } else if (RET_IS_OPERAND(t)) {
            if (t == RET_OPERAND_OMEGA)
                only_omega++;
            else
                only_omega = 0;
            if (t == RET_OPERAND_EPSILON)
                only_epsilon++;
            else
                only_epsilon = 0;
            dlist_add(lastblk, v);
            dlist_del_tail(relist, &v);
            if (blk)
                continue;
            else
                break;
        } else {
            if (blk) {
                dlist_add(lastblk, v);
                dlist_del_tail(relist, &v);
                continue;
            } else {
                break;
            }
        }
    }

    if (only_epsilon)
        return 1;

    if (only_omega)
        return 2;

    return 0;
}

static int regex_append_last_repeat(dlist_t *relist, int m, int n)
{
    int i;
    int ret = 0;
    dlist_t lastblk;

    dlist_init(&lastblk);
    ret = regex_truncate_last_block(relist, &lastblk);
    if (dlist_empty(&lastblk))
        return 0;

    if (ret == 1) { /** epsilon repeat is still epsilon */
        re_append_epsilon(relist);
        dlist_fini(&lastblk, NULL);
        return 0;
    } else if (ret == 2) {
        re_append_omega(relist);
        dlist_fini(&lastblk, NULL);
        return 0;
    }

    /**
     * General r{n,m} means n copies of r and m copies of r?.
     * The machine will do less work if we nest the final m copies,
     * so that r{2,5} = rr(r(r(r)?)?)?
     */
    if (m == 0) {
        if (n == -1) {
            /* case r{0,-1} is r* */
            dlist_append(relist, &lastblk);
            ret = re_append_operator(relist, REO_STAR);
            if (ret < 0)
                goto out;
        } else if (n == 0) {
            /* case r{0,0} is <EPSILON> */
            ret = re_append_epsilon(relist);
            if (ret < 0)
                goto out;
        } else if (n == 1) {
            /* case r{0,1} is r? */
            dlist_append(relist, &lastblk);
            ret = re_append_operator(relist, REO_QUEST);
            if (ret < 0)
                goto out;
        } else { /* n >=2 */
            /* case r{0,5} is (r(r(r(r(r)?)?)?)?)? */
            for (i = 0; i<n; i++) {
                ret = re_append_left_parenthesis(relist);
                if (ret < 0)
                    goto out;
                dlist_append(relist, &lastblk);
            }
            for (i = 0; i<n; i++) {
                ret = re_append_right_parenthesis(relist);
                if (ret < 0)
                    goto out;
                ret = re_append_operator(relist, REO_QUEST);
                if (ret < 0)
                    goto out;
            }
        }
    } else {
        if (n == -1) {
            /* case r{m, -1} repeat at least m times */
            for (i=0; i<m; i++) {
                dlist_append(relist, &lastblk);
            }
            ret = re_append_operator(relist, REO_PLUS);
            if (ret < 0)
                goto out;
        } else if (m == n) {
            /* case r{m} just repeat m times */
            for (i=0; i<m; i++) {
                dlist_append(relist, &lastblk);
            }
        } else {
            /* case r{m, n} repeat at least m and not more than n times */
            ret = re_append_left_parenthesis(relist);
            if (ret < 0)
                goto out;
            for (i=0; i<m; i++) {
                dlist_append(relist, &lastblk);
            }
            for (i=m; i<n; i++) {
                ret = re_append_left_parenthesis(relist);
                if (ret < 0)
                    goto out;
                dlist_append(relist, &lastblk);
            }
            for (i=m; i<n; i++) {
                ret = re_append_right_parenthesis(relist);
                if (ret < 0)
                    goto out;
                ret = re_append_operator(relist, REO_QUEST);
                if (ret < 0)
                    goto out;
            }
            ret = re_append_right_parenthesis(relist);
            if (ret < 0)
                goto out;
        }
    }

out:
    dlist_fini(&lastblk, NULL);
    return ret;
}

static int regex_parse_repeat(uint8_t *regex, uint8_t **start, dlist_t *relist)
{
    int m, n;
    uint8_t *p = (*start);

    p++;

    m = n = 0;
    while (*p) {
        if (*p < '0' || *p > '9') {
            fprintf(stderr, "invalid quantifier number at pos %d\n", (int)(p - regex));
            return -1;
        }
        m = m * 10 + (*p - '0');
        p++;
        if (*p == ',' || *p == '}')
            break;
    }

    if (*p == '\0') {
        fprintf(stderr, "need more quantifier char at pos %d\n", (int)(p - regex));
        return -1;
    }

    if (*p == '}') {
        n = m;
    } else if (*p == ',') {
        p++;
        if (*p == '\0') {
            fprintf(stderr, "need more quantifier char at pos %d\n", (int)(p - regex));
            return -1;
        }
        if (*p == '}') {
            n = -1;
        } else {
            n = 0;
            while (*p) {
                if (*p < '0' || *p > '9') {
                    fprintf(stderr, "invalid quantifier number at pos %d\n", (int)(p - regex));
                    return -1;
                }
                n = n * 10 + (*p - '0');
                p++;
                if (*p == '}')
                    break;
            }

            if (*p != '}') {
                fprintf(stderr, "need more quantifier char at pos %d\n", (int)(p - regex));
                return -1;
            }
        }
    }

    if (m >= 0 && n >= 0) {
        if (n < m) {
            fprintf(stderr, "invalid quantifier number(%d>%d) at pos %d\n", m, n, (int)(p - regex));
            return -1;
        }
    }

    if (m >= 65535 || n >= 65535) {
        fprintf(stderr, "to large repetition quantifier number!\n");
        return -1;
    }

    p++;
    *start = p;
    return regex_append_last_repeat(relist, m, n);
}

static int regex_parse(uint8_t *regex, dlist_t *relist, int *flags)
{
    int ret;
    uint8_t *p = regex;
    int parenlevel = 0; /** parenthesis level */
    int nocase = (*flags) & REF_IGNORECASE;
    int newline = (*flags) & REF_SINGLELINE;

    void *v, *v1;
    uint8_t t, c, c1, c2;

    if (*regex == '+'
            || *regex == '?'
            || *regex == '*'
            || *regex == '|'
            || *regex == ')') {
        fprintf(stderr, "invalid char at pos 0\n");
        return -1;
    }

    while(*p) {
        if (*p == '[') {
            ret = regex_parse_class(regex, &p, relist, nocase);
            if (ret < 0)
                return ret;
            continue;
        } else if (*p == '\\') {
            ret = regex_parse_escape_outside(regex, &p, relist, nocase);
            if (ret < 0)
                return ret;
            continue;
        } else if (*p == '{') {
            ret = regex_parse_repeat(regex, &p, relist);
            if (ret < 0)
                return ret;
            continue;
        } else if (*p == '(') {
            parenlevel++;
            ret = re_append_left_parenthesis(relist);
            if (ret < 0)
                return ret;
            p++;
            if (*p == '*'
                    || *p == '?'
                    || *p == '+'
                    || *p == '{'
                    || *p == '|') {
                fprintf(stderr, "invalid char after '(' at pos %d\n", (int)(p - regex));
                return -1;
            }
            continue;
        } else if (*p == ')') {
            if (dlist_peek_tail(relist, &v)) {
                re_exchar_get(v, &t, &c, &c1, &c2);
                if (t == RET_PARENTHESIS_LEFT) {
                    parenlevel--;
                    dlist_del_tail(relist, &v);
                    ret = re_append_epsilon(relist);
                    if (ret < 0)
                        return ret;
                    p++;
                    continue;
                } else if (RET_IS_OPERAND(t)) {
                    dlist_del_tail(relist, &v1);
                    if (dlist_peek_tail(relist, &v)) {
                        re_exchar_get(v, &t, &c, &c1, &c2);
                        if (t == RET_PARENTHESIS_LEFT) {
                            parenlevel--;
                            dlist_del_tail(relist, &v);
                            re_exchar_get(v1, &t, &c, &c1, &c2);
                            if (t == RET_OPERAND_EPSILON) {
                                ret = re_append_epsilon(relist);
                            } else {
                                ret = dlist_add_tail(relist, v1);
                            }
                            if (ret < 0)
                                return ret;
                            p++;
                            continue;
                        } else {
                            ret = dlist_add_tail(relist, v1);
                            if (ret < 0)
                                return ret;
                        }
                    } else {
                        fprintf(stderr, "should be not reachable: nothing before ')'!\n");
                        return -1;
                    }
                }
            } else {
                fprintf(stderr, "should be not reachable: nothing before ')'!\n");
                return -1;
            }
            parenlevel--;
            ret = re_append_right_parenthesis(relist);
            if (ret < 0)
                return ret;
            p++;
            continue;
        } else if (*p == '^') {
            if (p == regex) {
                (*flags) |= REF_MATCHBOL;
                p++; continue;
            } else {
                fprintf(stderr, "need to escape char '^' at pos %d\n", (int)(p - regex));
                return -1;
            }
        } else if (*p == '$') {
            if (*(p+1) == '\0') {
                (*flags) |= REF_MATCHEOL;
                p++; continue;
            } else {
                fprintf(stderr, "need to escape char '$' at pos %d\n", (int)(p - regex));
                return -1;
            }
        } else if (*p == '|') {
            ret = re_append_operator(relist, REO_ALT);
            if (ret < 0)
                return ret;
            p++;
            if (*p == '\0'
                    || *p == '*'
                    || *p == '?'
                    || *p == '+'
                    || *p == '{'
                    || *p == '|'
                    || *p == ')') {
                fprintf(stderr, "invalid char after '|' at pos %d\n", (int)(p - regex));
                return -1;
            }
            continue;
        } else if (*p == '*') {
            ret = re_append_operator(relist, REO_STAR);
            if (ret < 0)
                return ret;
            p++;
            if (*p == '*'
                    || *p == '?'
                    || *p == '+'
                    || *p == '{') {
                fprintf(stderr, "invalid char after '*' at pos %d\n", (int)(p - regex));
                return -1;
            }
            continue;
        } else if (*p == '?') {
            ret = re_append_operator(relist, REO_QUEST);
            if (ret < 0)
                return ret;
            p++;
            if (*p == '*'
                    || *p == '?'
                    || *p == '+'
                    || *p == '{') {
                fprintf(stderr, "invalid char after '?' at pos %d\n", (int)(p - regex));
                return -1;
            }
            continue;
        } else if (*p == '+') {
            ret = re_append_operator(relist, REO_PLUS);
            if (ret < 0)
                return ret;
            p++;
            if (*p == '*'
                    || *p == '?'
                    || *p == '+'
                    || *p == '{') {
                fprintf(stderr, "invalid char after '+' at pos %d\n", (int)(p - regex));
                return -1;
            }
            continue;
        } else if (*p == '.') {
            if (newline) {
                ret = re_append_anychar(relist);
                if (ret < 0)
                    return ret;
            } else {
                ret = re_append_dotchar(relist);
                if (ret < 0)
                    return ret;
            }
            p++;
            continue;
        } else {
            ret = re_append_incasechar(relist, *p, nocase);
            if (ret < 0)
                return ret;
            p++;
            continue;
        }
    }

    if (parenlevel) {
        fprintf(stderr, "non-matching(%d) left and right parenthesis\n", parenlevel);
        return -1;
    }

    return 0;
}

static int postfix_append_operand(dlist_t *postfix, void *value)
{
    return dlist_add_tail(postfix, value);
}

static int postfix_append_operator(dlist_t *postfix, void *value)
{
    return dlist_add_tail(postfix, value);
}

static int filo_enqueue_operator(filo_t *filo, dlist_t *postfix, uint8_t curop)
{
    void *value;
    uint8_t t, c, c1, c2;

    while (filo_peek(filo, &value)) {
        re_exchar_get(value, &t, &c, &c1, &c2);
        if (t != RET_PARENTHESIS_LEFT
               && get_precedence(c) >= get_precedence(curop)) {
            postfix_append_operator(postfix, value);
            filo_dequeue(filo, &value);
        } else {
            break;
        }
    }

    return filo_enqueue(filo, re_exchar_create(RET_OPERATOR, curop, 0, 0));
}

static int postfix_simplify_alt(filo_t * filo)
{
    dlist_t *s, *s1, *s2;
    void *u1, *u2;
    void *v1, *v2;

    uint8_t x, a, a1, a2;
    uint8_t y, b, b1, b2;

    if (!filo_dequeue(filo, &u1))
        return -1;
    if (!filo_dequeue(filo, &u2))
        return -1;

    s = malloc(sizeof(dlist_t));
    if (s == NULL)
        return -1;

    dlist_init(s);

    s1 = (dlist_t *) u1;
    s2 = (dlist_t *) u2;
    if (s1->nelm == 1 && s2->nelm == 1) {
        dlist_del(s1, &v1);
        dlist_del(s2, &v2);
        re_exchar_get(v1, &x, &a, &a1, &a2);
        re_exchar_get(v2, &y, &b, &b1, &b2);
        if (x == RET_OPERAND_CHAR && y == RET_OPERAND_CHAR) {   /* a | b = dual(a,b) */
            if (a + 1 == b) {
                re_append_range(s, a, b);
            } else if (b + 1 == a) {
                re_append_range(s, b, a);
            } else if (a == b) { /* a | a == a */
                re_append_char(s, a);
            } else {
                re_append_dualchar(s, a, b);
            }
        } else if (x == RET_OPERAND_DUALCHAR && y == RET_OPERAND_CHAR) {    /* dual(a,b) | c = triad(a,b,c) */
            if (b == a || b == a1) {
                re_append_dualchar(s, a, a1);
            } else {
                re_append_triadchar(s, a, a1, b);
            }
        } else if (x == RET_OPERAND_CHAR && y == RET_OPERAND_DUALCHAR) {    /* a | dual(b,c) = triad(a,b,c) */
            if (a == b || a == b1) {
                re_append_dualchar(s, b, b1);
            } else {
                re_append_triadchar(s, a, b, b1);
            }
        } else if (x == RET_OPERAND_CHAR && y == RET_OPERAND_RANGE) {   /* a | range(b,c) = range(a,c) */
            if (a + 1 == b) {
                re_append_range(s, a, b1);
            } else if (b1 + 1 == a) {
                re_append_range(s, b, a);
            } else if (a == b || a == b1) { /* a | range(a, c) == range(a,c) and c | range(a,c) = range(a,c) */
                re_append_range(s, b, b1);
            } else {
                dlist_add_tail(s, v2);
                dlist_add_tail(s, v1);
                re_append_operator(s, REO_ALT);
            }
        } else if (x == RET_OPERAND_RANGE && y == RET_OPERAND_CHAR) {   /* range(a,b) | c = range(a,c) */
            if (b + 1 == a) {
                re_append_range(s, b, a1);
            } else if (a1 + 1 == b) {
                re_append_range(s, a, b);
            } else if (b == a || b == a1) {
                re_append_range(s, a, a1);
            } else {
                dlist_add_tail(s, v2);
                dlist_add_tail(s, v1);
                re_append_operator(s, REO_ALT);
            }
        } else if (x == RET_OPERAND_EPSILON) {
            if (y == RET_OPERAND_OMEGA) {   /* <EPS> | <OMG> = <EPS> */
                re_append_epsilon(s);
            } else if (y == RET_OPERAND_EPSILON) {  /* <EPS> | <EPS> = <EPS> */
                re_append_epsilon(s);
            } else {            /* <EPS> | r = r<QUEST> */
                dlist_add_tail(s, v2);
                re_append_operator(s, REO_QUEST);
            }
        } else if (y == RET_OPERAND_EPSILON) {
            if (x == RET_OPERAND_OMEGA) {   /* <EPS> | <OMG> = <EPS> */
                re_append_epsilon(s);
            } else if (x == RET_OPERAND_EPSILON) {  /* <EPS> | <EPS> = <EPS> */
                re_append_epsilon(s);
            } else {            /* <EPS> | r = r<QUEST> */
                dlist_add_tail(s, v1);
                re_append_operator(s, REO_QUEST);
            }
        } else if (x == RET_OPERAND_OMEGA) {
            if (y == RET_OPERAND_EPSILON) { /* <OMG> | <EPS> = <EPS> */
                re_append_epsilon(s);
            } else if (y == RET_OPERAND_OMEGA) {    /* <OMG> | <OMG> = <OMG> */
                re_append_omega(s);
            } else {            /* <OMG> | r = r */
                dlist_add_tail(s, v2);
            }
        } else if (y == RET_OPERAND_OMEGA) {
            if (x == RET_OPERAND_EPSILON) { /* <OMG> | <EPS> = <EPS> */
                re_append_epsilon(s);
            } else if (x == RET_OPERAND_OMEGA) {    /* <OMG> | <OMG> = <OMG> */
                re_append_omega(s);
            } else {            /* <OMG> | r = r */
                dlist_add_tail(s, v1);
            }
        } else {
            dlist_add_tail(s, v2);
            dlist_add_tail(s, v1);
            re_append_operator(s, REO_ALT);
        }
    } else if (s1->nelm == 1 && s2->nelm > 1) {
        dlist_del(s1, &v1);
        re_exchar_get(v1, &x, &a, &a1, &a2);
        if (x == RET_OPERAND_EPSILON) { /* <EPS> | r = r<QUEST> */
            while (dlist_del(s2, &v2)) {
                dlist_add_tail(s, v2);
            }
            re_append_operator(s, REO_QUEST);
        } else if (x == RET_OPERAND_OMEGA) {    /* <OMG> | r = r */
            while (dlist_del(s2, &v2)) {
                dlist_add_tail(s, v2);
            }
        } else {
            while (dlist_del(s2, &v2)) {
                dlist_add_tail(s, v2);
            }
            dlist_add_tail(s, v1);
            re_append_operator(s, REO_ALT);
        }
    } else if (s1->nelm > 1 && s2->nelm == 1) {
        dlist_del(s2, &v2);
        re_exchar_get(v2, &y, &b, &b1, &b2);
        if (y == RET_OPERAND_EPSILON) { /* <EPS> | r = r<QUEST> */
            while (dlist_del(s1, &v1)) {
                dlist_add_tail(s, v1);
            }
            re_append_operator(s, REO_QUEST);
        } else if (y == RET_OPERAND_OMEGA) {    /* <OMG> | r = r */
            while (dlist_del(s1, &v1)) {
                dlist_add_tail(s, v1);
            }
        } else {
            dlist_add_tail(s, v2);
            while (dlist_del(s1, &v1)) {
                dlist_add_tail(s, v1);
            }
            re_append_operator(s, REO_ALT);
        }
    } else {
        while (dlist_del(s2, &v2)) {
            dlist_add_tail(s, v2);
        }
        while (dlist_del(s1, &v1)) {
            dlist_add_tail(s, v1);
        }
        re_append_operator(s, REO_ALT);
    }

    filo_enqueue(filo, s);
    free(s1);
    free(s2);
    return 0;
}

static int postfix_simplify_cat(filo_t * filo)
{
    dlist_t *s, *s1, *s2;
    void *u1, *u2;
    void *v1, *v2;

    uint8_t x, a, a1, a2;
    uint8_t y, b, b1, b2;

    if (!filo_dequeue(filo, &u1))
        return -1;
    if (!filo_dequeue(filo, &u2))
        return -1;

    s = malloc(sizeof(dlist_t));
    if (s == NULL)
        return -1;

    dlist_init(s);

    s1 = (dlist_t *) u1;
    s2 = (dlist_t *) u2;
    if (s1->nelm == 1 && s2->nelm == 1) {
        dlist_del(s1, &v1);
        dlist_del(s2, &v2);
        re_exchar_get(v1, &x, &a, &a1, &a2);
        re_exchar_get(v2, &y, &b, &b1, &b2);
        if (x == RET_OPERAND_EPSILON) {
            if (y == RET_OPERAND_OMEGA) {   /* <EPS><OMG> = <OMG> */
                re_append_omega(s);
            } else if (y == RET_OPERAND_EPSILON) {  /* <EPS><EPS> = <EPS> */
                re_append_epsilon(s);
            } else {            /* <EPS>r = r */
                dlist_add_tail(s, v2);
            }
        } else if (y == RET_OPERAND_EPSILON) {
            if (x == RET_OPERAND_OMEGA) {   /* <OMG><EPS> = <OMG> */
                re_append_omega(s);
            } else if (x == RET_OPERAND_EPSILON) {  /* <EPS><EPS> = <EPS> */
                re_append_epsilon(s);
            } else {            /* <EPS>r = r */
                dlist_add_tail(s, v1);
            }
        } else if (x == RET_OPERAND_OMEGA) {
            if (y == RET_OPERAND_EPSILON) { /* <OMG><EPS> = <OMG> */
                re_append_omega(s);
            } else if (y == RET_OPERAND_OMEGA) {    /* <OMG><OMG> = <OMG> */
                re_append_omega(s);
            } else {            /* <OMG>r = <OMG> */
                re_append_omega(s);
            }
        } else if (y == RET_OPERAND_OMEGA) {
            if (x == RET_OPERAND_EPSILON) { /* <OMG><EPS> = <OMG> */
                re_append_omega(s);
            } else if (x == RET_OPERAND_OMEGA) {    /* <OMG><OMG> = <OMG> */
                re_append_omega(s);
            } else {            /* <OMG>r = <OMG> */
                re_append_omega(s);
            }
        } else {
            dlist_add_tail(s, v2);
            dlist_add_tail(s, v1);
            re_append_operator(s, REO_CAT);
        }
    } else if (s1->nelm == 1 && s2->nelm > 1) {
        dlist_del(s1, &v1);
        re_exchar_get(v1, &x, &a, &a1, &a2);
        if (x == RET_OPERAND_EPSILON) { /* <EPS>r = r */
            while (dlist_del(s2, &v2)) {
                dlist_add_tail(s, v2);
            }
        } else if (x == RET_OPERAND_OMEGA) {    /* <OMG>r = <OMG> */
            while (dlist_del(s2, &v2)) ;
            re_append_omega(s);
        } else {
            while (dlist_del(s2, &v2)) {
                dlist_add_tail(s, v2);
            }
            dlist_add_tail(s, v1);
            re_append_operator(s, REO_CAT);
        }
    } else if (s1->nelm > 1 && s2->nelm == 1) {
        dlist_del(s2, &v2);
        re_exchar_get(v2, &y, &b, &b1, &b2);
        if (y == RET_OPERAND_EPSILON) { /* <EPS>r = r */
            while (dlist_del(s1, &v1)) {
                dlist_add_tail(s, v1);
            }
        } else if (y == RET_OPERAND_OMEGA) {    /* <OMG>r = <OMG> */
            while (dlist_del(s1, &v1)) ;
            re_append_omega(s);
        } else {
            dlist_add_tail(s, v2);
            while (dlist_del(s1, &v1)) {
                dlist_add_tail(s, v1);
            }
            re_append_operator(s, REO_CAT);
        }
    } else {
        while (dlist_del(s2, &v2)) {
            dlist_add_tail(s, v2);
        }
        while (dlist_del(s1, &v1)) {
            dlist_add_tail(s, v1);
        }
        re_append_operator(s, REO_CAT);
    }

    filo_enqueue(filo, s);
    free(s1);
    free(s2);
    return 0;
}

static int postfix_simplify_star(filo_t * filo)
{
    dlist_t *s, *s1;
    void *u1, *v1;

    uint8_t x, a, a1, a2;

    if (!filo_dequeue(filo, &u1))
        return -1;

    s = malloc(sizeof(dlist_t));
    if (s == NULL)
        return -1;

    dlist_init(s);

    s1 = (dlist_t *) u1;
    if (s1->nelm == 1) {
        dlist_del(s1, &v1);
        re_exchar_get(v1, &x, &a, &a1, &a2);
        if (x == RET_OPERAND_EPSILON) { /* <EPS>* = <EPS> */
            re_append_epsilon(s);
        } else if (x == RET_OPERAND_OMEGA) {    /* <OMG>* = <EPS> */
            re_append_epsilon(s);
        } else {
            dlist_add_tail(s, v1);
            re_append_operator(s, REO_STAR);
        }
    } else {
        while (dlist_del(s1, &v1)) {
            dlist_add_tail(s, v1);
        }
        re_append_operator(s, REO_STAR);
    }

    filo_enqueue(filo, s);
    free(s1);
    return 0;
}

static int postfix_simplify_plus(filo_t * filo)
{
    dlist_t *s, *s1;
    void *u1, *v1;

    uint8_t x, a, a1, a2;

    if (!filo_dequeue(filo, &u1))
        return -1;

    s = malloc(sizeof(dlist_t));
    if (s == NULL)
        return -1;

    dlist_init(s);

    s1 = (dlist_t *) u1;
    if (s1->nelm == 1) {
        dlist_del(s1, &v1);
        re_exchar_get(v1, &x, &a, &a1, &a2);
        if (x == RET_OPERAND_EPSILON) { /* <EPS>+ = <EPS> */
            re_append_epsilon(s);
        } else if (x == RET_OPERAND_OMEGA) {    /* <OMG>+ = <OMG> */
            re_append_omega(s);
        } else {
            dlist_add_tail(s, v1);
            re_append_operator(s, REO_PLUS);
        }
    } else {
        while (dlist_del(s1, &v1)) {
            dlist_add_tail(s, v1);
        }
        re_append_operator(s, REO_PLUS);
    }

    filo_enqueue(filo, s);
    free(s1);
    return 0;
}

static int postfix_simplify_quest(filo_t * filo)
{
    dlist_t *s, *s1;
    void *u1, *v1;

    uint8_t x, a, a1, a2;

    if (!filo_dequeue(filo, &u1))
        return -1;

    s = malloc(sizeof(dlist_t));
    if (s == NULL)
        return -1;

    dlist_init(s);

    s1 = (dlist_t *) u1;
    if (s1->nelm == 1) {
        dlist_del(s1, &v1);
        re_exchar_get(v1, &x, &a, &a1, &a2);
        if (x == RET_OPERAND_EPSILON) { /* <EPS>? = <EPS> */
            re_append_epsilon(s);
        } else if (x == RET_OPERAND_OMEGA) {    /* <OMG>? = <EPS> */
            re_append_epsilon(s);
        } else {
            dlist_add_tail(s, v1);
            re_append_operator(s, REO_QUEST);
        }
    } else {
        while (dlist_del(s1, &v1)) {
            dlist_add_tail(s, v1);
        }
        re_append_operator(s, REO_QUEST);
    }

    filo_enqueue(filo, s);
    free(s1);
    return 0;
}

static int postfix_simplify(dlist_t * compost, dlist_t * simpost)
{
    int ret;
    filo_t filo;
    dlist_t *s;
    void *v, *v1;
    uint8_t t, c, c1, c2;

    filo_init(&filo);
    while (dlist_del(compost, &v)) {
        re_exchar_get(v, &t, &c, &c1, &c2);
        if (t == RET_OPERATOR) {
            if (c == REO_ALT) {
                ret = postfix_simplify_alt(&filo);
                if (ret < 0)
                    goto error;
            } else if (c == REO_CAT) {
                ret = postfix_simplify_cat(&filo);
                if (ret < 0)
                    goto error;
            } else if (c == REO_STAR) {
                ret = postfix_simplify_star(&filo);
                if (ret < 0)
                    goto error;
            } else if (c == REO_PLUS) {
                ret = postfix_simplify_plus(&filo);
                if (ret < 0)
                    goto error;
            } else if (c == REO_QUEST) {
                ret = postfix_simplify_quest(&filo);
                if (ret < 0)
                    goto error;

            }
        } else if (RET_IS_OPERAND(t)) {
            s = malloc(sizeof(dlist_t));
            if (s == NULL)
                goto error;
            dlist_init(s);
            dlist_add_tail(s, v);
            filo_enqueue(&filo, s);
        }
    }

    if (filo_dequeue(&filo, &v)) {
        s = (dlist_t *) v;
        while (dlist_del(s, &v1)) {
            dlist_add_tail(simpost, v1);
        }
        free(s);
    }

    return 0;
error:
    while (filo_dequeue(&filo, &v)) {
        s = (dlist_t *) v;
        dlist_fini(s, NULL);
        free(s);
    }
    return -1;
}

static int regex2postfix(dlist_t *relist, dlist_t *postfix)
{
    filo_t filo;
    dlist_t compost;
    dlist_node_t *cur;
    dlist_node_t *next;
    dlist_node_t *head;

    void *v;
    uint8_t t, c, c1, c2;

    /**
     * we will enqueue the following exchar into filo.
     * - RET_PARENTHESIS_LEFT
     * - RET_OPERATOR
     * */
    filo_init(&filo);
    dlist_init(&compost);

    head = &relist->head;
    for (cur = relist->head.next; cur != head; cur = cur->next) {
        next = cur->next;
        re_exchar_get(cur->value, &t, &c, &c1, &c2);
        if (t == RET_PARENTHESIS_LEFT) {
            filo_enqueue(&filo, cur->value);
        } else if (t == RET_PARENTHESIS_RIGHT) {
            while (filo_dequeue(&filo, &v)) {
                re_exchar_get(v, &t, &c, &c1, &c2);
                if (t != RET_PARENTHESIS_LEFT) {
                    postfix_append_operator(&compost, v);
                } else {
                    break;
                }
            }
            if (next != head) {
                re_exchar_get(next->value, &t, &c, &c1, &c2);
                if (RET_IS_OPERAND(t)
                    || (t == RET_PARENTHESIS_LEFT)) {
                    /** )r and )( insert a CAT */
                    filo_enqueue_operator(&filo, &compost, REO_CAT);
                }
            }
        } else if (t == RET_OPERATOR) {
            if (c == REO_STAR) {
                filo_enqueue_operator(&filo, &compost, REO_STAR);
                if (next != head) {
                    re_exchar_get(next->value, &t, &c, &c1, &c2);
                    if (RET_IS_OPERAND(t)
                            || (t == RET_PARENTHESIS_LEFT)) {
                        /** *r and *( insert a CAT */
                        filo_enqueue_operator(&filo, &compost, REO_CAT);
                    }
                }
            } else if (c == REO_PLUS) {
                filo_enqueue_operator(&filo, &compost, REO_PLUS);
                if (next != head) {
                    re_exchar_get(next->value, &t, &c, &c1, &c2);
                    if (RET_IS_OPERAND(t)
                            || (t == RET_PARENTHESIS_LEFT)) {
                        /** +r and +( insert a CAT */
                        filo_enqueue_operator(&filo, &compost, REO_CAT);
                    }
                }
            } else if (c == REO_QUEST) {
                filo_enqueue_operator(&filo, &compost, REO_QUEST);
                if (next != head) {
                    re_exchar_get(next->value, &t, &c, &c1, &c2);
                    if (RET_IS_OPERAND(t)
                            || (t == RET_PARENTHESIS_LEFT)) {
                        /** ?r and ?( insert a CAT */
                        filo_enqueue_operator(&filo, &compost, REO_CAT);
                    }
                }
            } else if (c == REO_ALT) {
                filo_enqueue_operator(&filo, &compost, REO_ALT);
            }
        } else if (RET_IS_OPERAND(t)) {
            postfix_append_operand(&compost, cur->value);
            if (next != head) {
                re_exchar_get(next->value, &t, &c, &c1, &c2);
                if (RET_IS_OPERAND(t)
                        || (t == RET_PARENTHESIS_LEFT)) {
                    /** xr and x( insert a CAT */
                    filo_enqueue_operator(&filo, &compost, REO_CAT);
                }
            }
        }
    }

    while (filo_dequeue(&filo, &v)) {
        re_exchar_get(v, &t, &c, &c1, &c2);
        if (t == RET_PARENTHESIS_LEFT
            || t == RET_PARENTHESIS_RIGHT) {
            fprintf(stderr, "invalid operator in stack\n");
            filo_fini(&filo, NULL);
            return -1;
        }

        postfix_append_operator(&compost, v);
    }

    filo_fini(&filo, NULL);

#ifdef DEBUG
    printf("compost:\n");
    dlist_foreach(&compost, re_exchar_dump, NULL, 0);
    printf("\n");
#endif
    postfix_simplify(&compost, postfix);
#ifdef DEBUG
    printf("simpost:\n");
    dlist_foreach(postfix, re_exchar_dump, NULL, 0);
    printf("\n");
#endif

    return 0;
}

static nfa_state_t *nfa_state_create(uint8_t type, uint8_t c, uint8_t c1, uint8_t c2, nfa_state_t *out, nfa_state_t *out1)
{
    nfa_state_t *s;

    s = malloc(sizeof(nfa_state_t));
    if (s == NULL)
        return NULL;

    s->type = type;
    s->c = c;
    s->c1 = c1;
    s->c2 = c2;
    s->marked = 0;
    s->bol = 0;
    s->eol = 0;
    s->ml = 0;
    s->out = out;
    s->out1 = out1;

    return s;
}

static void nfa_state_destroy(void *ptr)
{
    nfa_state_t *s;

    s = (nfa_state_t *)ptr;
    if (s) {
        free(s);
    }
}

static void nfa_state_flush(void *ptr)
{
    nfa_state_t *s;

    s = (nfa_state_t *)ptr;
    s->marked = 0;
}

static nfa_outlist_t *nfa_outlist_create(nfa_state_t **ps)
{
    nfa_outlist_t *n;

    n = (nfa_outlist_t *)ps;
    n->next = NULL;
    return n;
}

static nfa_outlist_t *nfa_outlist_append(nfa_outlist_t *l1, nfa_outlist_t *l2)
{
    nfa_outlist_t *n;

    n = l1;
    while (n->next)
        n = n->next;
    n->next = l2;
    return l1;
}

static void nfa_outlist_patch(nfa_outlist_t *l, nfa_state_t *s)
{
    nfa_outlist_t *n, *t;

    n = l;
    while (n) {
        t = n->next;
        n->next = (nfa_outlist_t *)s;
        n = t;
    }
}

static nfa_fragment_t *nfa_fragment_create(nfa_state_t *s, nfa_outlist_t *l)
{
    nfa_fragment_t *f;

    f = malloc(sizeof(nfa_fragment_t));
    if (f == NULL)
        return NULL;

    f->start = s;
    f->outlist = l;

    return f;
}

static void nfa_fragment_destroy(void *ptr)
{
    nfa_fragment_t *f = (nfa_fragment_t *) ptr;

    if (f) {
        free(f);
    }
}

static void nfa_destroy(mdfa_t *prog)
{
    struct nfa_st *nfa = (struct nfa_st *)prog;

    if (nfa) {
        filo_t stack;
        fifo_t queue;
        void *v;
        nfa_state_t *s;

        if (nfa->charset
                && nfa->charset != g_dot_charset
                && nfa->charset != g_default_charset) {
            free(nfa->charset);
            nfa->charset = NULL;
            nfa->nchar = 0;
        }

        s = nfa->start;
        if (s == NULL)
            return;

        filo_init(&stack);
        fifo_init(&queue);

        filo_enqueue(&stack, s);
        while (filo_dequeue(&stack, &v)) {
            s = (nfa_state_t *)v;
            if (s->marked == 0) {
                if (s->out) {
                    filo_enqueue(&stack, s->out);
                }
                if (s->out1) {
                    filo_enqueue(&stack, s->out1);
                }
                fifo_enqueue(&queue, s);
                s->marked = 1;
            }
        }

        while (fifo_dequeue(&queue, &v)) {
            s = (nfa_state_t *)v;
            nfa_state_destroy(s);
        }

        nfa->start = NULL;
    }
}

/**
 * @brief create a fragment of c
 * <PRE>
 *       c
 * ->(O)--->out
 * </PRE>
 * @note a new state need to created
 */
static int fragment_process_literal(struct nfa_st *nfa, filo_t *frags, uint8_t c)
{
    nfa_state_t *s;
    nfa_outlist_t *l;
    nfa_fragment_t *f;

    s = nfa_state_create(NFA_STATE_TYPE_LITER, c, 0, 0, NULL, NULL);
    if (s == NULL)
        return -1;

    s->eol = !!(nfa->flags & REF_MATCHEOL);
    s->bol = !!(nfa->flags & REF_MATCHBOL);
    s->ml = !!(nfa->flags & REF_MULTILINE);
    s->pid = nfa->pid;
    nfa->nstate++;

    l = nfa_outlist_create(&s->out);
    if (l == NULL) {
        nfa_state_destroy(s);
        return -1;
    }

    f = nfa_fragment_create(s, l);
    if (f == NULL) {
        nfa_state_destroy(s);
        return -1;
    }

    return filo_enqueue(frags, f);
}

/**
 * @brief create a fragment of nocase char
 * <PRE>
 *       c
 * ->(O)--->out
 * </PRE>
 * @note a new state need to created
 */
static int fragment_process_nocase(struct nfa_st *nfa, filo_t *frags, uint8_t c)
{
    nfa_state_t *s;
    nfa_outlist_t *l;
    nfa_fragment_t *f;

    s = nfa_state_create(NFA_STATE_TYPE_NOCASE, c, 0, 0, NULL, NULL);
    if (s == NULL)
        return -1;

    s->eol = !!(nfa->flags & REF_MATCHEOL);
    s->bol = !!(nfa->flags & REF_MATCHBOL);
    s->ml = !!(nfa->flags & REF_MULTILINE);
    s->pid = nfa->pid;
    nfa->nstate++;

    l = nfa_outlist_create(&s->out);
    if (l == NULL) {
        nfa_state_destroy(s);
        return -1;
    }

    f = nfa_fragment_create(s, l);
    if (f == NULL) {
        nfa_state_destroy(s);
        return -1;
    }

    return filo_enqueue(frags, f);
}

/**
 * @brief create a fragment of dot char(not include newline)
 * <PRE>
 *       .
 * ->(O)--->out
 * </PRE>
 * @note a new state need to created
 */
static int fragment_process_dot(struct nfa_st *nfa, filo_t *frags)
{
    nfa_state_t *s;
    nfa_outlist_t *l;
    nfa_fragment_t *f;

    s = nfa_state_create(NFA_STATE_TYPE_DOTCH, 0, 0, 0, NULL, NULL);
    if (s == NULL)
        return -1;

    s->eol = !!(nfa->flags & REF_MATCHEOL);
    s->bol = !!(nfa->flags & REF_MATCHBOL);
    s->ml = !!(nfa->flags & REF_MULTILINE);
    s->pid = nfa->pid;
    nfa->nstate++;

    l = nfa_outlist_create(&s->out);
    if (l == NULL) {
        nfa_state_destroy(s);
        return -1;
    }

    f = nfa_fragment_create(s, l);
    if (f == NULL) {
        nfa_state_destroy(s);
        return -1;
    }

    return filo_enqueue(frags, f);
}

/**
 * @brief create a fragment of any char(include newline)
 * <PRE>
 *        x
 * -->(O)--->out
 * </PRE>
 * @note a new state need to created
 */
static int fragment_process_any(struct nfa_st *nfa, filo_t *frags)
{
    nfa_state_t *s;
    nfa_outlist_t *l;
    nfa_fragment_t *f;

    s = nfa_state_create(NFA_STATE_TYPE_ANYCH, 0, 0, 0, NULL, NULL);
    if (s == NULL)
        return -1;

    s->eol = !!(nfa->flags & REF_MATCHEOL);
    s->bol = !!(nfa->flags & REF_MATCHBOL);
    s->ml = !!(nfa->flags & REF_MULTILINE);
    s->pid = nfa->pid;
    nfa->nstate++;

    l = nfa_outlist_create(&s->out);
    if (l == NULL) {
        nfa_state_destroy(s);
        return -1;
    }

    f = nfa_fragment_create(s, l);
    if (f == NULL) {
        nfa_state_destroy(s);
        return -1;
    }

    return filo_enqueue(frags, f);
}

/**
 * @brief create a fragment of dual char(a or b)
 * <PRE>
 *        a|b
 * -->(O)----->out
 * </PRE>
 * @note a new state need to created
 */
static int fragment_process_dual(struct nfa_st *nfa, filo_t *frags, uint8_t c, uint8_t c1)
{
    nfa_state_t *s;
    nfa_outlist_t *l;
    nfa_fragment_t *f;

    s = nfa_state_create(NFA_STATE_TYPE_DUAL, c, c1, 0, NULL, NULL);
    if (s == NULL)
        return -1;

    s->eol = !!(nfa->flags & REF_MATCHEOL);
    s->bol = !!(nfa->flags & REF_MATCHBOL);
    s->ml = !!(nfa->flags & REF_MULTILINE);
    s->pid = nfa->pid;
    nfa->nstate++;

    l = nfa_outlist_create(&s->out);
    if (l == NULL) {
        nfa_state_destroy(s);
        return -1;
    }

    f = nfa_fragment_create(s, l);
    if (f == NULL) {
        nfa_state_destroy(s);
        return -1;
    }

    return filo_enqueue(frags, f);
}

/**
 * @brief create a fragment of triad char(a or b or c)
 * <PRE>
 *        a|b|c
 * -->(O)------->out
 * </PRE>
 * @note a new state need to created
 */
static int fragment_process_triad(struct nfa_st *nfa, filo_t *frags, uint8_t c, uint8_t c1, uint8_t c2)
{
    nfa_state_t *s;
    nfa_outlist_t *l;
    nfa_fragment_t *f;

    s = nfa_state_create(NFA_STATE_TYPE_TRIAD, c, c1, c2, NULL, NULL);
    if (s == NULL)
        return -1;

    s->eol = !!(nfa->flags & REF_MATCHEOL);
    s->bol = !!(nfa->flags & REF_MATCHBOL);
    s->ml = !!(nfa->flags & REF_MULTILINE);
    s->pid = nfa->pid;
    nfa->nstate++;

    l = nfa_outlist_create(&s->out);
    if (l == NULL) {
        nfa_state_destroy(s);
        return -1;
    }

    f = nfa_fragment_create(s, l);
    if (f == NULL) {
        nfa_state_destroy(s);
        return -1;
    }

    return filo_enqueue(frags, f);
}

/**
 * @brief create a fragment of range-char
 * <PRE>
 *        [a-b]
 * -->(O)------->out
 * </PRE>
 * @note a new state need to created
 */
static int fragment_process_range(struct nfa_st *nfa, filo_t *frags, uint8_t c , uint8_t c1)
{
    nfa_state_t *s;
    nfa_outlist_t *l;
    nfa_fragment_t *f;

    s = nfa_state_create(NFA_STATE_TYPE_RANGE, c, c1, 0, NULL, NULL);
    if (s == NULL)
        return -1;

    s->eol = !!(nfa->flags & REF_MATCHEOL);
    s->bol = !!(nfa->flags & REF_MATCHBOL);
    s->ml = !!(nfa->flags & REF_MULTILINE);
    s->pid = nfa->pid;
    nfa->nstate++;

    l = nfa_outlist_create(&s->out);
    if (l == NULL) {
        nfa_state_destroy(s);
        return -1;
    }

    f = nfa_fragment_create(s, l);
    if (f == NULL) {
        nfa_state_destroy(s);
        return -1;
    }

    return filo_enqueue(frags, f);
}

/**
 * @brief create a fragment of f1|f2
 * <PRE>
 *      --->[f1]--->
 *     |
 * -->(O)
 *     |
 *      --->[f2]--->
 * </PRE>
 * @note a new state need to created
 */
static int fragment_process_alt(struct nfa_st *nfa, filo_t *frags)
{
    void *v;
    nfa_state_t *s;
    nfa_fragment_t *f;
    nfa_fragment_t *f1, *f2;

    if(!filo_dequeue(frags, &v)) {
        fprintf(stderr, "fragment stack empty!\n");
        return -1;
    }

    f2 = (nfa_fragment_t *) v;
    if(!filo_dequeue(frags, &v)) {
        fprintf(stderr, "fragment stack empty!\n");
        return -1;
    }

    f1 = (nfa_fragment_t *) v;
    s = nfa_state_create(NFA_STATE_TYPE_SPLIT, 0, 0, 0, f1->start, f2->start);
    if (s == NULL)
        return -1;

    s->eol = !!(nfa->flags & REF_MATCHEOL);
    s->bol = !!(nfa->flags & REF_MATCHBOL);
    s->ml = !!(nfa->flags & REF_MULTILINE);
    s->pid = nfa->pid;
    nfa->nstate++;

    f = nfa_fragment_create(s, nfa_outlist_append(f1->outlist, f2->outlist));
    if (f == NULL) {
        nfa_state_destroy(s);
        fprintf(stderr, "fragment create failed!\n");
        return -1;
    }

    nfa_fragment_destroy(f2);
    nfa_fragment_destroy(f1);
    return filo_enqueue(frags, f);
}

/**
 * @brief create a fragment of f1f2.
 * <PRE>
 * ->[f1]->[f2]->
 * </PRE>
 * @note don't need to create a new state.
 */
static int fragment_process_cat(struct nfa_st *nfa, filo_t *frags)
{
    void *v;
    nfa_fragment_t *f;
    nfa_fragment_t *f1, *f2;

    if(!filo_dequeue(frags, &v)) {
        fprintf(stderr, "fragment stack empty!\n");
        return -1;
    }

    f2 = (nfa_fragment_t *) v;
    if(!filo_dequeue(frags, &v)) {
        fprintf(stderr, "fragment stack empty!\n");
        return -1;
    }

    f1 = (nfa_fragment_t *) v;
    nfa_outlist_patch(f1->outlist, f2->start);

    f = nfa_fragment_create(f1->start, f2->outlist);
    if (f == NULL) {
        fprintf(stderr, "fragment create failed!\n");
        return -1;
    }

    nfa_fragment_destroy(f2);
    nfa_fragment_destroy(f1);
    return filo_enqueue(frags, f);
}

/**
 * @brief create a fragment of f*.
 * <PRE>
 *      --->[f]---
 *     |         |
 * ---(O)<--------
 *     |
 *      --------->
 * </PRE>
 * @note need to create a new state.
 */
static int fragment_process_star(struct nfa_st *nfa, filo_t *frags)
{
    void *v;
    nfa_state_t *s;
    nfa_fragment_t *f;
    nfa_fragment_t *f1;

    if(!filo_dequeue(frags, &v)) {
        fprintf(stderr, "fragment stack empty!\n");
        return -1;
    }

    f1 = (nfa_fragment_t *) v;
    s = nfa_state_create(NFA_STATE_TYPE_SPLIT, 0, 0, 0, f1->start, NULL);
    if (s == NULL)
        return -1;

    s->eol = !!(nfa->flags & REF_MATCHEOL);
    s->bol = !!(nfa->flags & REF_MATCHBOL);
    s->ml = !!(nfa->flags & REF_MULTILINE);
    s->pid = nfa->pid;
    nfa->nstate++;

    nfa_outlist_patch(f1->outlist, s);
    f = nfa_fragment_create(s, nfa_outlist_create(&s->out1));
    if (f == NULL) {
        nfa_state_destroy(s);
        fprintf(stderr, "fragment create failed!\n");
        return -1;
    }

    nfa_fragment_destroy(f1);
    return filo_enqueue(frags, f);
}

/**
 * @brief create a fragment of f+.
 * <PRE>
 *    --------
 *   \|/     |
 * ->[f]--->(O)--->
 *
 * </PRE>
 * @note need to create a new state.
 */
static int fragment_process_plus(struct nfa_st *nfa, filo_t *frags)
{
    void *v;
    nfa_state_t *s;
    nfa_fragment_t *f;
    nfa_fragment_t *f1;

    if(!filo_dequeue(frags, &v)) {
        fprintf(stderr, "fragment stack empty!\n");
        return -1;
    }

    f1 = (nfa_fragment_t *) v;
    s = nfa_state_create(NFA_STATE_TYPE_SPLIT, 0, 0, 0, f1->start, NULL);
    if (s == NULL)
        return -1;

    s->eol = !!(nfa->flags & REF_MATCHEOL);
    s->bol = !!(nfa->flags & REF_MATCHBOL);
    s->ml = !!(nfa->flags & REF_MULTILINE);
    s->pid = nfa->pid;
    nfa->nstate++;

    nfa_outlist_patch(f1->outlist, s);
    f = nfa_fragment_create(f1->start, nfa_outlist_create(&s->out1));
    if (f == NULL) {
        nfa_state_destroy(s);
        fprintf(stderr, "fragment create failed!\n");
        return -1;
    }

    nfa_fragment_destroy(f1);
    return filo_enqueue(frags, f);
}

/**
 * @brief create a fragment of f?.
 * <PRE>
 *     --->[f]--->
 *    |
 * ->(O)
 *    |
 *     ---------->
 * </PRE>
 * @note need to create a new state.
 */
static int fragment_process_quest(struct nfa_st *nfa, filo_t *frags)
{
    void *v;
    nfa_state_t *s;
    nfa_outlist_t *l;
    nfa_fragment_t *f;
    nfa_fragment_t *f1;

    if(!filo_dequeue(frags, &v)) {
        fprintf(stderr, "fragment stack empty!\n");
        return -1;
    }

    f1 = (nfa_fragment_t *) v;
    s = nfa_state_create(NFA_STATE_TYPE_SPLIT, 0, 0, 0, f1->start, NULL);
    if (s == NULL)
        return -1;

    s->eol = !!(nfa->flags & REF_MATCHEOL);
    s->bol = !!(nfa->flags & REF_MATCHBOL);
    s->ml = !!(nfa->flags & REF_MULTILINE);
    s->pid = nfa->pid;
    nfa->nstate++;

    l = nfa_outlist_create(&s->out1);
    if (l == NULL) {
        nfa_state_destroy(s);
        return -1;
    }

    f = nfa_fragment_create(s, nfa_outlist_append(f1->outlist, l));
    if (f == NULL) {
        nfa_state_destroy(s);
        fprintf(stderr, "fragment create failed!\n");
        return -1;
    }

    nfa_fragment_destroy(f1);
    return filo_enqueue(frags, f);
}

/**
 * @brief create a fragment of match.
 * <PRE>
 * ->((O))
 * </PRE>
 * @note need to create a new state.
 */
static int fragment_process_match(struct nfa_st *nfa, filo_t *frags)
{
    void *v;
    nfa_state_t *s;
    nfa_fragment_t *f;

    s = nfa_state_create(NFA_STATE_TYPE_MATCH, 0, 0, 0, NULL, NULL);
    if (s == NULL)
        return -1;

    s->eol = !!(nfa->flags & REF_MATCHEOL);
    s->bol = !!(nfa->flags & REF_MATCHBOL);
    s->ml = !!(nfa->flags & REF_MULTILINE);
    s->pid = nfa->pid;
    nfa->nstate++;

    if(filo_dequeue(frags, &v)) {
        if (!filo_empty(frags)) {
            fprintf(stderr, "fragment stack not empty!\n");
            nfa_state_destroy(s);
            return -1;
        }

        f = (nfa_fragment_t *) v;

        nfa_outlist_patch(f->outlist, s);
        nfa->start = f->start;

        nfa_fragment_destroy(f);
    } else {
        nfa->start = s;
    }

    return 0;
}

static int postfix2nfa(int pid, dlist_t *postfix, struct nfa_st *nfa)
{
    int i;
    int ret;
    void *v;
    uint8_t t, c, c1, c2;

    char all_include_newline = 0;
    char all_exclude_newline = 0;
    uint8_t charmap[N_CHARSET] = {0, };

    filo_t frags;
    filo_init(&frags);

    nfa->pid = pid;
    nfa->nstate = 0;
    nfa->start = NULL;

    if (postfix->nelm == 0) {
        goto match;
    } else if (postfix->nelm == 1) {
        dlist_peek(postfix, &v);
        re_exchar_get(v, &t, &c, &c1, &c2);
        if (t == RET_OPERAND_EPSILON) {
            dlist_del(postfix, &v);
            nfa->flags |= REF_MATCHEPS;
            goto match;
        } else if (t == RET_OPERAND_OMEGA) {
            dlist_del(postfix, &v);
            nfa->flags |= REF_MATCHOMG;
            goto match;
        }
    }

    while (dlist_del(postfix, &v)) {
        re_exchar_get(v, &t, &c, &c1, &c2);
        if (t == RET_OPERAND_CHAR) {
            ret = fragment_process_literal(nfa, &frags, c);
            if (ret < 0)
                goto error;
            charmap[c] = 1;
        } else if (t == RET_OPERAND_NOCASECHAR) {
            ret = fragment_process_nocase(nfa, &frags, c);
            if (ret < 0)
                goto error;
            charmap[c] = 1;
            charmap[toupper(c)] = 1;
        } else if (t == RET_OPERAND_DOTCHAR) {
            ret = fragment_process_dot(nfa, &frags);
            if (ret < 0)
                goto error;
            all_exclude_newline = 1;
        } else if (t == RET_OPERAND_ANYCHAR) {
            ret = fragment_process_any(nfa, &frags);
            if (ret < 0)
                goto error;
            all_include_newline = 1;
        } else if (t == RET_OPERAND_RANGE) {
            ret = fragment_process_range(nfa, &frags, c, c1);
            if (ret < 0)
                goto error;
            for (i=c; i<=c1; i++) {
                charmap[i] = 1;
            }
        } else if (t == RET_OPERAND_DUALCHAR) {
            ret = fragment_process_dual(nfa, &frags, c, c1);
            if (ret < 0)
                goto error;
            charmap[c] = 1;
            charmap[c1] = 1;
        } else if (t == RET_OPERAND_TRIADCHAR) {
            ret = fragment_process_triad(nfa, &frags, c, c1, c2);
            if (ret < 0)
                goto error;
            charmap[c] = 1;
            charmap[c1] = 1;
            charmap[c2] = 1;
        } else if (t == RET_OPERATOR) {
            switch(c) {
            case REO_ALT:
                ret = fragment_process_alt(nfa, &frags);
                if (ret < 0)
                    goto error;
                break;
            case REO_CAT:
                ret = fragment_process_cat(nfa, &frags);
                if (ret < 0)
                    goto error;
                break;
            case REO_STAR:
                ret = fragment_process_star(nfa, &frags);
                if (ret < 0)
                    goto error;
                break;
            case REO_PLUS:
                ret = fragment_process_plus(nfa, &frags);
                if (ret < 0)
                    goto error;
                break;
            case REO_QUEST:
                ret = fragment_process_quest(nfa, &frags);
                if (ret < 0)
                    goto error;
                break;
            default:
                break;
            }
        }
    }

match:
    ret = fragment_process_match(nfa, &frags);
    if (ret < 0)
        goto error;

    if (all_include_newline) {
        nfa->nchar = N_CHARSET;
        nfa->charset = g_default_charset;
    } else if (all_exclude_newline) {
        if (charmap['\n']) {
            nfa->nchar = N_CHARSET;
            nfa->charset = g_default_charset;
        } else {
            nfa->nchar = N_CHARSET - 1;
            nfa->charset = g_dot_charset;
        }
    } else {
        nfa->nchar = 0;
        nfa->charset = malloc(N_CHARSET * sizeof(uint8_t));
        if (nfa->charset) {
            for (i=0; i<N_CHARSET; i++) {
                if (charmap[i]) {
                    nfa->charset[nfa->nchar++] = i;
                }
            }
        } else {
            nfa->nchar = N_CHARSET;
            nfa->charset = g_default_charset;
        }
    }

    filo_fini(&frags, nfa_fragment_destroy);
    return 0;
error:
    nfa_destroy(nfa);
    filo_fini(&frags, nfa_fragment_destroy);
    return -1;
}

static int nfa_compile(mdfa_t *prog, int pid, char *regex, int flags)
{
    int ret;
    dlist_t relist;
    dlist_t postfix;
    struct nfa_st *nfa = (struct nfa_st *)prog;

    if (nfa == NULL || regex == NULL)
        return -1;

    dlist_init(&relist);
    ret = regex_parse((uint8_t *)regex, &relist, &flags);
    if (ret < 0) {
        dlist_fini(&relist, NULL);
        return ret;
    }

#ifdef DEBUG
    printf("relist:\n");
    dlist_foreach(&relist, re_exchar_dump, NULL, 0);
    printf("\n");
#endif

    dlist_init(&postfix);
    ret = regex2postfix(&relist, &postfix);
    if (ret < 0) {
        dlist_fini(&relist, NULL);
        dlist_fini(&postfix, NULL);
        return ret;
    }

#ifdef DEBUG
    printf("postfix:\n");
    dlist_foreach(&postfix, re_exchar_dump, NULL, 0);
    printf("\n");
#endif

    nfa->flags = flags;
    ret = postfix2nfa(pid, &postfix, nfa);
    if (ret < 0) {
        fprintf(stderr, "postfix2nfa failed!\n");
        dlist_fini(&postfix, NULL);
        dlist_fini(&relist, NULL);
        nfa_destroy(nfa);
        return ret;
    }

    dlist_fini(&postfix, NULL);
    dlist_fini(&relist, NULL);
    return 0;
}

/**
 * prog may be same as one of n1 and n2.
 * please don't destroy n1 and n2 after combine.
 */
static int nfa_combine(mdfa_t *prog, void *n1, void *n2)
{
    int i;
    nfa_state_t *s;
    struct nfa_st *nfa = (struct nfa_st *)prog;
    struct nfa_st *nfa1 = (struct nfa_st *)n1;
    struct nfa_st *nfa2 = (struct nfa_st *)n2;
    uint8_t charmap[N_CHARSET] = {0, };

    if (nfa1->start == NULL
            || nfa2->start == NULL) {
        fprintf(stderr, "sorry, we can't combine an empty nfa\n");
        return -1;
    }

    nfa->pid = 0;
    nfa->flags = nfa1->flags | nfa2->flags;
    nfa->nstate = nfa1->nstate + nfa2->nstate;
    if (nfa1->nchar == N_CHARSET
            || nfa2->nchar == N_CHARSET) {
        if (nfa1->charset != g_default_charset
                && nfa1->charset != g_dot_charset) {
            free(nfa1->charset);
        }
        if (nfa2->charset != g_default_charset
                && nfa2->charset != g_dot_charset) {
            free(nfa2->charset);
        }
        nfa->nchar = N_CHARSET;
        nfa->charset = g_default_charset;
    } else if (nfa1->charset == g_dot_charset
            && nfa2->charset == g_dot_charset) {
        nfa->nchar = N_CHARSET - 1;
        nfa->charset = g_dot_charset;
    } else {
        if (nfa1->charset) {
            for (i=0; i<nfa1->nchar; i++) {
                charmap[nfa1->charset[i]] = 1;
            }
            if (nfa1->charset != g_default_charset
                    && nfa1->charset !=  g_dot_charset) {
                free(nfa1->charset);
            }
        }

        if (nfa2->charset) {
            for (i=0; i<nfa2->nchar; i++) {
                charmap[nfa2->charset[i]] = 1;
            }
            if (nfa2->charset != g_default_charset
                    && nfa2->charset !=  g_dot_charset) {
                free(nfa2->charset);
            }
        }

        nfa->charset = malloc(N_CHARSET * sizeof(uint8_t));
        if (nfa->charset) {
            nfa->nchar = 0;
            for (i=0; i<N_CHARSET; i++) {
                if (charmap[i]) {
                    nfa->charset[nfa->nchar++] = i;
                }
            }
        }
    }

    s = nfa_state_create(NFA_STATE_TYPE_SPLIT, 0, 0, 0, nfa1->start, nfa2->start);
    if (s == NULL)
        return -1;

    s->pid = nfa->pid;
    nfa->nstate++;
    nfa->start = s;

    return 0;
}

static dfa_state_t *dfa_state_create(void)
{
    int i;
    dfa_state_t *s;

    s = malloc(sizeof(dfa_state_t));
    if (s == NULL)
        return NULL;

    s->bol = 0;
    s->eol = 0;
    s->ml = 0;
    s->pid = -1;
    s->accept = 0;
    s->nfa_set = malloc(sizeof(slist_t));
    if (s->nfa_set == NULL) {
        free(s);
        return NULL;
    }
    filo_init(s->nfa_set);
    for (i=0; i<N_CHARSET; i++) {
        s->next[i] = NULL;
    }

    return s;
}

static void dfa_state_destroy(void *ptr, void (*data_fini)(void *ptr))
{
    dfa_state_t *s;

    s = (dfa_state_t *)ptr;
    if (s) {
        if (s->nfa_set) {
            slist_fini(s->nfa_set, data_fini);
            free(s->nfa_set);
        }
        free(s);
    }
}

void dfa_destroy(mdfa_t *prog)
{
    struct dfa_st *dfa = (struct dfa_st *)prog;

    if (dfa) {
        dfa_state_t *s;
        void *v;

        while (filo_dequeue(&dfa->states, &v)) {
            s = (dfa_state_t *)v;
            dfa_state_destroy(s, NULL);
        }

        free(dfa);
    }
}

static void dfa_destroy_nfa(mdfa_t *prog)
{
    struct dfa_st *dfa = (struct dfa_st *)prog;

    if (dfa) {
        dfa_state_t *s;
        slist_node_t *cur;

        for (cur = dfa->states.head; cur; cur = cur->next) {
            s = (dfa_state_t *)cur->value;
            filo_fini(s->nfa_set, NULL);
            free(s->nfa_set);
            s->nfa_set = NULL;
        }
    }
}

static dfa_state_t *dfa_state_find(slist_t *nfa_set, slist_t *dfa_states, slist_t *dfa_marked_states)
{
    slist_node_t *n;
    dfa_state_t *s;

    for (n = dfa_states->head; n; n = n->next) {
        s = (dfa_state_t *)n->value;
        if (slist_equal(nfa_set, s->nfa_set, NULL))
            return s;
    }

    for (n = dfa_marked_states->head; n; n = n->next) {
        s = (dfa_state_t *)n->value;
        if (slist_equal(nfa_set, s->nfa_set, NULL))
            return s;
    }

    return NULL;
}

static int nfa_eps_closure(struct nfa_st *nfa, slist_t *set, slist_t *closure, dfa_state_t *ds)
{
    void *v;
    nfa_state_t *s;

    while (slist_del(set, &v)) {
        s = (nfa_state_t *)v;
        if (s->marked == 0) {
            if (s->type == NFA_STATE_TYPE_SPLIT) {
                if (s->out) {
                    slist_add(set, s->out);
                }
                if (s->out1) {
                    slist_add(set, s->out1);
                }
            }
            slist_add_sort(closure, s, NULL);
            s->marked = 1;
            if (s->type == NFA_STATE_TYPE_MATCH) {
                ds->accept = 1;
                if (ds->pid > s->pid) {
                    ds->pid = s->pid;
                    ds->bol = s->bol;
                    ds->eol = s->eol;
                    ds->ml = s->ml;
                }
            }
        }
    }

    return ds->accept;
}

static int nfa_closure_flush(slist_t *closure)
{
    slist_node_t *cur;
    nfa_state_t *s;

    for (cur = closure->head; cur; cur = cur->next) {
        s = (nfa_state_t *)cur->value;
        s->marked = 0;
    }

    return 0;
}

static int nfa_move(struct nfa_st *nfa, slist_t *closure, uint8_t c, slist_t *set)
{
    slist_node_t *cur;
    nfa_state_t *s;

    for (cur = closure->head; cur; cur = cur->next) {
        s = (nfa_state_t *)cur->value;
        if (s->type == NFA_STATE_TYPE_LITER) {
            if (c == s->c) {
                slist_add_tail(set, s->out);
            }
        } else if (s->type == NFA_STATE_TYPE_NOCASE) {
            if (c == s->c || tolower(c) == s->c) {
                slist_add_tail(set, s->out);
            }
        } else if (s->type == NFA_STATE_TYPE_DOTCH) {
            if (c != '\n') {
                slist_add_tail(set, s->out);
            }
        } else if (s->type == NFA_STATE_TYPE_ANYCH) {
            slist_add_tail(set, s->out);
        } else if (s->type == NFA_STATE_TYPE_DUAL) {
            if (c == s->c || c == s->c1) {
                slist_add_tail(set, s->out);
            }
        } else if (s->type == NFA_STATE_TYPE_TRIAD) {
            if (c == s->c || c == s->c1 || c == s->c2) {
                slist_add_tail(set, s->out);
            }
        } else if (s->type == NFA_STATE_TYPE_RANGE) {
            if (c >= s->c && c <= s->c1) {
                slist_add_tail(set, s->out);
            }
        }
        s->marked = 0;
    }

    return 0;
}

static int nfa2dfa(struct nfa_st *nfa, struct dfa_st *dfa)
{
    int i;
    slist_t set;
    slist_t dfa_states;

    void *v;
    int n_dfa_states = 0;
    dfa_state_t *T, *U, *W;

    int nchar = N_CHARSET;
    uint8_t *charset = g_default_charset;

    dfa->flags = nfa->flags;
    if (nfa->start == NULL) {
        dfa->start = NULL;
        return 0;
    }

    slist_init(&set);
    slist_init(&dfa_states);

    if (nfa->charset) {
        nchar = nfa->nchar;
        charset = nfa->charset;
    }

    T = dfa_state_create();
    n_dfa_states++;

    slist_add(&set, nfa->start);
    nfa_eps_closure(nfa, &set, T->nfa_set, T);
    dfa->start = T;

    if (nchar == 0) {
        slist_add_tail(&dfa->states,  T);
        nfa_closure_flush(T->nfa_set);
        return 0;
    }

    slist_add(&dfa_states,  T);

    while (slist_del(&dfa_states, &v)) {
        T = (dfa_state_t *)v;
        slist_add_tail(&dfa->states,  T);
        for (i=0; i<nchar; i++) {
            nfa_move(nfa, T->nfa_set, charset[i], &set);
            if (slist_empty(&set))
                continue;

            U = dfa_state_create();
            nfa_eps_closure(nfa, &set, U->nfa_set, U);
            W = dfa_state_find(U->nfa_set, &dfa_states, &dfa->states);
            if (W == NULL) {
                n_dfa_states++;
                slist_add_tail(&dfa_states,  U);
                nfa_closure_flush(U->nfa_set);
            } else {
                dfa_state_destroy(U, nfa_state_flush);
                U = W;
            }
            T->next[charset[i]] = U;
        }
    }

    dfa_destroy_nfa(dfa);
    return n_dfa_states;
}

void *dfa_compile(char *regex, int flags)
{
    struct nfa_st nfa;
    struct dfa_st *dfa;

    dfa = malloc(sizeof(*dfa));
    if (dfa == NULL) {
        return NULL;
    }

    dfa->start = NULL;
    filo_init(&dfa->states);

    if (nfa_compile(&nfa, 0, regex, flags) < 0) {
        free(dfa);
        return NULL;
    }

    nfa2dfa(&nfa, dfa);
    nfa_destroy(&nfa);

    return dfa;
}

void *mdfa_compile(int n, char **regex, int *flags)
{
    int i;
    struct nfa_st *nfa;
    struct dfa_st *dfa;

    dfa = malloc(sizeof(*dfa));
    if (dfa == NULL) {
        return NULL;
    }

    dfa->start = NULL;
    filo_init(&dfa->states);

    nfa = malloc(n * sizeof(*nfa));
    if (nfa == NULL) {
        free(dfa);
        return NULL;
    }

    if (nfa_compile(nfa, 0, regex[0], flags[0]) < 0) {
        fprintf(stderr, "compile regex 0 /%s/%x failed\n", regex[0], flags[0]);
        goto out;
    }

    for (i=1; i<n; i++) {
        if (nfa_compile(nfa + i, i, regex[i], flags[i]) < 0) {
            fprintf(stderr, "compile regex %d /%s/%x failed\n", i, regex[i], flags[i]);
            nfa_destroy(nfa); // nfa[0] had combined nfa[1]...nfa[i-1]
            goto out;
        }

        if (nfa_combine(nfa, nfa + i, nfa) < 0) {
            fprintf(stderr, "combine regex %d /%s/%x failed\n", i, regex[i], flags[i]);
            nfa_destroy(nfa); // nfa[0] had combined nfa[1]...nfa[i-1]
            nfa_destroy(nfa + i);
            goto out;
        }
    }

    nfa2dfa(nfa, dfa);
    dfa->flags |= REF_MULTIREG;
    nfa_destroy(nfa);
    free(nfa);
    return dfa;
out:
    free(nfa);
    free(dfa);
    return NULL;
}

int dfa_match(mdfa_t *prog, uint8_t *text, int len, int *start, int *end, int flags)
{
    int cur_pos;
    int start_pos;
    int match_pos;

    dfa_state_t *cur_state;
    dfa_state_t *tmp_state;
    dfa_state_t *match_state;

    struct dfa_st *dfa = (struct dfa_st *)prog;

    if (dfa == NULL || text == NULL || len == 0)
        return -1;

    /** process the special case */
    if (dfa->flags & REF_MATCHOMG) {
        return -1;
    }

    if (dfa->flags & REF_MATCHEPS) {
        *start = *end = 0;
        return 0;
    }

    /** process the generic case */
    cur_pos = -1;
    start_pos = 0;
    cur_state = dfa->start;
    do {
        if (cur_state == NULL)
            return -1;

        if (cur_state->accept) {
            match_state = cur_state;
            if (match_state->bol) {
                if (match_state->ml) {
                    if (start_pos >= 1 && text[start_pos - 1] != '\n') {
                        do {
                            cur_pos = ++start_pos;
                            if (cur_pos >= len)
                                return -1;
                            cur_state = dfa->start->next[text[cur_pos]];
                        } while (cur_state == NULL);
                        continue;
                    }
                } else {
                    if (start_pos != 0) {
                        if (!(dfa->flags & REF_MULTIREG))
                            return -1;

                        do {
                            cur_pos = ++start_pos;
                            if (cur_pos >= len)
                                return -1;
                            cur_state = dfa->start->next[text[cur_pos]];
                        } while (cur_state == NULL);
                        continue;
                    }
                }
            }

            /** until to the longest match pos */
            match_pos = cur_pos;
            tmp_state = cur_state;
            while (cur_pos + 1 < len) {
                tmp_state = tmp_state->next[text[cur_pos + 1]];
                if (tmp_state) {
                    cur_state = tmp_state;
                    cur_pos = cur_pos + 1;
                    if (cur_state->accept) {
                        match_state = cur_state;
                        match_pos = cur_pos;
                    }
                } else {
                    break;
                }
            }

            if (!cur_state->accept)
                cur_pos = match_pos;

            if (match_state->eol) {
                if (match_state->ml) {
                    if (!(cur_pos + 1 == len || text[cur_pos + 1] == '\n')) {
                        do {
                            cur_pos = ++start_pos;
                            if (cur_pos >= len)
                                return -1;
                            cur_state = dfa->start->next[text[cur_pos]];
                        } while (cur_state == NULL);
                        continue;
                    }
                    if (match_state->bol) {
                        if (start_pos >= 1 && text[start_pos - 1] != '\n') {
                            do {
                                cur_pos = ++start_pos;
                                if (cur_pos >= len)
                                    return -1;
                                cur_state = dfa->start->next[text[cur_pos]];
                            } while (cur_state == NULL);
                            continue;
                        }
                    }
                } else {
                    if (!(cur_pos + 1 == len
                                || (cur_pos + 1 == len - 1
                                    && text[cur_pos + 1] == '\n'))) {
                        do {
                            cur_pos = ++start_pos;
                            if (cur_pos >= len) {
                                if (dfa->states.nelm == 1
                                        && match_state->bol == 0) {
                                    *start = len;
                                    *end = len;
                                    return match_state->pid;
                                }

                                return -1;
                            }
                            cur_state = dfa->start->next[text[cur_pos]];
                        } while (cur_state == NULL);
                        if (dfa->states.nelm == 1
                                && match_state->bol == 0
                                && cur_state == NULL) {
                            /**
                             * special case for "$" matched with "aaaa"
                             * and "a*$" matched with "baaaab".
                             */
                            *start = len;
                            *end = len;
                            return match_state->pid;
                        }
                        continue;
                    }
                }
            }

            *start = start_pos;
            *end = cur_pos + 1;
            return match_state->pid;
        } else {
            if (cur_pos + 1 >= len)
                return -1;

            cur_state = cur_state->next[text[cur_pos + 1]];
            if (cur_state == NULL) {
                if ((dfa->flags & REF_MULTIREG) == 0) {
                    if ((dfa->flags & REF_MULTILINE) == 0) {
                        if (dfa->flags & REF_MATCHBOL) {
                            return -1;
                        }
                    }
                }

                while (cur_state == NULL) {
                    cur_pos = ++start_pos;
                    if (cur_pos >= len)
                        return -1;
                    cur_state = dfa->start->next[text[cur_pos]];
                }
            } else {
                cur_pos++;
            }
        }
    } while (cur_pos < len);

    return -1;
}

int dfa_nstate(mdfa_t *prog)
{
    struct dfa_st *dfa = prog;

    if (dfa == NULL) {
        return 0;
    }

    return dfa->states.nelm;
}

