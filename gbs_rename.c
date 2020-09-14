#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>

#include "dict.h"
#include "libstring.h"

enum {
    TOK_WORD, // linux, J2EE, 2nd, 3G, JPEG2000, I'd etc
    TOK_NUMBER, // 2007, 1.09, etc
    TOK_SPACE, // [._ ]+
    TOK_GROUP, // '[', '(', '{', '<', ']', ')', '}', '>'
    TOK_PUNCT, // '&', '+', '=', ',' exclude '#', such as c# is a word.
    TOK_STRIKE, // '-'
    TOK_UTF8, // UTF-8
    TOK_OTHERS, // others
    TOK_EOF, // '\0'

    TOK_SKIP
};

typedef struct token_st {
    int type;
    char *value;
} token_t;

static inline int iswordchar(unsigned char c)
{
    switch (c) {
    case 'a' ... 'z':
    case 'A' ... 'Z':
    case '\'':
        return 1;
    default:
        return 0;
    }
}

static inline int isspacechar(unsigned char c)
{
    switch (c) {
    case '.':
    case '_':
    case ' ':
        return 1;
    default:
        return 0;
    }
}

static inline int isleftchar(unsigned char c)
{
    switch (c) {
    case '(':
    case '[':
    case '{':
    case '<':
        return 1;
    default:
        return 0;
    }
}

static inline unsigned char getrightchar(unsigned char c)
{
    switch (c) {
    case '(': return ')';
    case '[': return ']';
    case '{': return '}';
    case '<': return '>';
    default:
        return 0;
    }
}

static inline int ispunctchar(unsigned char c)
{
    switch (c) {
    case '&':
    case '+':
    case '-':
    case '=':
    case ',':
    case ';':
        return 1;
    default:
        return 0;
    }
}

static token_t *get_token(char **start)
{
    int flags = 0;
    char *mark, *pos, *yapos, *yyapos;
    token_t *token = malloc(sizeof(token_t));
    if (token == NULL) {
        return NULL;
    }

    token->type = TOK_SKIP;
    token->value = NULL;

    pos = (*start);
    switch ((unsigned char)*pos) {
    case '\0':
        token->type = TOK_EOF;
        break;
    case 'a' ... 'z':
    case 'A' ... 'Z':
        token->type = TOK_WORD;
        if (*pos == 'c' || *pos == 'C') {
            if (*(pos + 1) == '+' && *(pos + 2) == '+') {
                token->value = strdup("c++");
                pos += 3;
                break;
            } else
            if (*(pos + 1) == '-' && *(pos + 2) == '-') {
                token->value = strdup("c--");
                pos += 3;
                break;
            }
        }

        for (mark = pos; iswordchar(*pos); pos++);
        for (yapos = pos; ((*yapos) >= '0' && (*yapos) <= '9'); yapos++);
        if (*yapos == '.') {
            token->value = strndup(mark, pos - mark);
        } else {
            for (yyapos = yapos; iswordchar(*yyapos); yyapos++);
            if (yapos != pos) {
                if (yyapos != yapos) {
                    token->value = strndup(mark, yyapos - mark);
                    pos = yyapos;
                } else {
                    token->value = strndup(mark, yapos - mark);
                    pos = yapos;
                }
            } else {
                token->value = strndup(mark, pos - mark);
            }
        }

        if (strcasecmp(token->value, "wi") == 0) {
            if (strncasecmp(pos, "-fi", 3) == 0) {
                if (!iswordchar(*(pos + 3))) {
                    strappend(&token->value, "-fi");
                    pos += 3;
                }
            }
        }
        break;
    case '0' ... '9':
        flags = 0;
        for (mark = pos; ((*pos) >= '0' && (*pos) <= '9') || (*pos) == '.'; pos++) {
            if (*pos == '.') flags = 1;
        }
        if (!flags) {
            for (yapos = pos; iswordchar(*yapos); yapos++);
            if (yapos != pos) {
                token->type = TOK_WORD;
                token->value = strndup(mark, yapos - mark);
                pos = yapos;
            } else {
                if (pos - mark >= 8) {
                    token->type = TOK_SPACE;
                    token->value = strdup(" ");
                } else {
                    token->type = TOK_NUMBER;
                    token->value = strndup(mark, pos - mark);
                }
            }
        } else {
            token->type = TOK_NUMBER;
            if (*(pos - 1) == '.') {
                token->value = strndup(mark, pos - mark - 1);
                pos--;
            } else {
                token->value = strndup(mark, pos - mark);
            }
        }
        break;
    case '.':
        {
            if (!strncasecmp(pos, ".net", 4)) {
                if (!iswordchar(*(pos + 4))) {
                    token->type = TOK_WORD;
                    token->value = strdup(".net");
                    pos += 4;
                    break;
                }
            }
        }
    case '_':
    case ' ':
        {
            if (!strncasecmp(pos + 1, ".net", 4)) {
                if (!iswordchar(*(pos + 5))) {
                    token->type = TOK_WORD;
                    token->value = strdup(".NET");
                    pos += 5;
                    break;
                }
            }
        }
        for (mark = pos; isspacechar(*pos); pos++);
        if (*pos != '-') {
            token->type = TOK_SPACE;
            token->value = strdup(" ");
        } else {
            token->type = TOK_SKIP;
        }
        break;
    case '(':
    case '[':
    case '{':
    case '<':
        token->type = TOK_GROUP;
        mark = pos++;
        for (; *pos && (*pos != getrightchar(*mark)); pos++);
        if (*pos == getrightchar(*mark)) pos++;
        token->value = strndup(mark, pos - mark);
        break;
    case '&':
        if (!strncasecmp(pos, "&amp;", 5)) {
            token->type = TOK_WORD;
            token->value = strdup("and");
            pos += 5;
            break;
        }
        if (!strncasecmp(pos, "&nbsp;", 6)) {
            token->type = TOK_SPACE;
            token->value = strdup(" ");
            pos += 6;
            break;
        }
        if (!strncasecmp(pos, "&quot;", 6)) {
            token->type = TOK_PUNCT;
            token->value = strdup("\"");
            pos += 6;
            break;
        }
        if (!strncasecmp(pos, "&trade;", 7)) {
            token->type = TOK_WORD;
            token->value = strdup("TM");
            pos += 7;
            break;
        }

        token->type = TOK_WORD;
        token->value = strdup("and");
        pos += 1;
        break;
    case ',':
        token->type = TOK_PUNCT;
        token->value = strdup(",");
        pos += 1;
        break;
    case '-':
        token->type = TOK_STRIKE;
        token->value = strdup("-");
        pos += 1;
        break;
    case '=':
    case '+':
    case ';':
        token->type = TOK_SPACE;
        for (mark = pos; ispunctchar(*pos); pos++);
        token->value = strdup(" ");
        break;
    case 0x80 ... 0xFF:
        token->type = TOK_UTF8;
        for (mark = pos; *pos >= 0x80 && *pos <= 0xFF; pos++);
        token->value = strndup(mark, pos - mark);
        break;
    default:
        token->type = TOK_OTHERS;
        mark = pos++;
        token->value = strappendchar(&token->value, *mark);
        break;
    }

    *start = pos;
    return token;
}

int token_match_publisher(char *token)
{
    int i;
    char *publishers[] = {
        "wrox", "wiley", "wesley", "oreilly", "reilly", "o'reilly",
        "addison", "springer", "prentice", "syngress", "sams",
        "cambridge", "mcgraw"
    };

    for (i=0; i<sizeof(publishers)/sizeof(publishers[0]); i++) {
        if (strcmp(token, publishers[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

int uniform(char **input)
{
    token_t *token;
    int last_type = TOK_SKIP;
    char *output = NULL;
    char *pos = (*input);
    char *uniform;

    token = get_token(&pos);
    while (token->type != TOK_EOF) {
        //printf("get_token:[%d][%s]\n", token->type, token->value);
        switch (token->type) {
        case TOK_WORD:
            strtolower(token->value);
            if (token_match_publisher(token->value)) {
                if (strncasecmp(pos + 1, "press", 5) == 0) {
                    pos += 6;
                } else if (strcmp(token->value, "mcgraw") == 0) {
                    if (strncasecmp(pos + 1, "hill", 4) == 0) {
                        pos += 5;
                    }
                }
                break;
            }
            if (strstr(token->value, "ebook")) {
                if (strlen(token->value) <= 7) {
                    break;
                }
            }
            if (strcmp(token->value, "ddu") == 0) {
                if (strlen(token->value) <= 7) {
                    break;
                }
            }
            if (strcmp(token->value, "bbl") == 0) {
                if (strlen(token->value) <= 7) {
                    break;
                }
            }
            if (strcmp(token->value, "supplex") == 0) {
                if (strlen(token->value) <= 7) {
                    break;
                }
            }
            if (strcmp(token->value, "lib") == 0) {
                if (strlen(token->value) <= 7) {
                    break;
                }
            }
            if (strcmp(token->value, "pdf") == 0) {
                if (strlen(pos) <= 3) {
                    break;
                }
            }
            if (strcmp(token->value, "html") == 0) {
                if (strlen(pos) <= 3) {
                    break;
                }
            }
            if (strcmp(token->value, "djvu") == 0) {
                if (strlen(pos) <= 3) {
                    break;
                }
            }
            if (strcmp(token->value, "pdg") == 0) {
                if (strlen(pos) <= 3) {
                    break;
                }
            }

            if (strcmp(token->value, ".net") == 0) {
                if (last_type != TOK_SPACE
                        && last_type != TOK_PUNCT) {
                    strappend(&output, " ");
                }
            }

            uniform = dict_search(token->value);
            if (last_type == TOK_SKIP
                || last_type == TOK_STRIKE) {
                switch (strlen(uniform)) {
                case 1:
                    capitalize(uniform);
                    break;
                case 2:
                    if (islower((int)uniform[0])
                            || islower((int)uniform[1])) {
                        capitalize(uniform);
                    }
                    break;
                default:
                    break;
                }
            }

            strappend(&output, uniform);
            if (strcmp(token->value, "2nd") == 0
                || strcmp(token->value, "3rd") == 0) {
                if (!strstr(pos, "ed")) {
                    strappend(&output, " Edition");
                }
            }
            last_type = token->type;
            break;
        case TOK_NUMBER:
            if (last_type == TOK_WORD) {
                if (strlen(token->value) >= 3) {
                    if (output[strlen(output) - 1] != 'V') {
                        strappend(&output, " ");
                    }
                }
            }
            strappend(&output, token->value);
            last_type = token->type;
            break;
        case TOK_SPACE:
            if (last_type == TOK_PUNCT
                    || last_type == TOK_GROUP
                    || last_type == TOK_STRIKE
                    || last_type == TOK_SKIP
                    || last_type == TOK_SPACE) {
                break;
            }
            strappend(&output, token->value);
            last_type = token->type;
            break;
        case TOK_GROUP:
            if (((double)strlen(token->value) / (double)strlen(*input)) >= 0.60) {
                strappend(&output, token->value);
                last_type = token->type;
            }
            break;
        case TOK_STRIKE:
            if (last_type == TOK_SKIP) {
                break;
            }
            strappend(&output, token->value);
            last_type = token->type;
            break;
        case TOK_PUNCT:
            if (last_type == TOK_SKIP) {
                break;
            } else if (last_type == TOK_SPACE) {
                if (strcmp(token->value, ",") == 0) {
                    output[strlen(output) - 1] = '\0';
                }
            }
            strappend(&output, token->value);
            last_type = token->type;
            if (strcmp(token->value, ",") == 0) {
                strappend(&output, " ");
                last_type = TOK_SPACE;
            }
            break;
        case TOK_UTF8:
            strappend(&output, token->value);
            last_type = token->type;
            break;
        case TOK_OTHERS:
            strappend(&output, token->value);
            last_type = token->type;
            break;
        default:
            break;
        }

        free(token->value);
        free(token);

        token = get_token(&pos);
    }

    free(token);

    if (output == NULL) {
        return 0;
    }

    strtrim(output, " \r\n\t-,@#$%^&*()_{}:\";',./<>?!~\\");
    free(*input);
    *input = output;
    return 1;
}

