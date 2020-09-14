#include "gbs_abbr.h"
#include "libstring.h"

static char *abbr[] = {
#include "gbs_abbr.lst"
};

tree_node_t *tree_create(char *word, char *lowercase, char *meaning)
{
    tree_node_t *node = malloc(sizeof(tree_node_t));
    if (node == NULL)
        return NULL;

    node->left = NULL;
    node->right = NULL;
    node->word = strdup(word);
    node->lowercase = strdup(lowercase);
    node->meaning = strdup(meaning);

    return node;
}

void tree_insert_word(tree_node_t * tree, char *word, char *lowercase, char *meaning)
{
    int cmp;
    tree_node_t *p, *q;

    p = q = tree;
    while (p) {
        q = p;
        cmp = strcmp(lowercase, p->lowercase);
        if (cmp < 0) {
            p = p->left;
        } else if (cmp > 0) {
            p = p->right;
        } else {
            break;
        }
    }

    cmp = strcmp(lowercase, q->lowercase);
    if (cmp == 0) {
        if (!strstr(q->meaning, meaning)) {
            strappendfmt(&q->meaning, " | %s", meaning);
        }
    } else if (cmp < 0) {
        q->left = tree_create(word, lowercase, meaning);
    } else {
        q->right = tree_create(word, lowercase, meaning);
    }
}

void tree_insert_array(tree_node_t *tree, char **array, int N)
{
    int i;
    char *line, *word, *lowercase, *meaning, *pos;

    for (i = 0; i < N; i++) {
        line = strdup(array[i]);

        stripwhite(line);

        if (line[0] == '\0')
            continue;

        pos = strchr(line, '\t');
        if (pos) {
            word = strndup(line, pos - line);
            meaning = strdup(pos + 1);
        } else {
            word = strdup(line);
            meaning = strdup("");
        }

        lowercase = strdup(word);
        strtolower(lowercase);

        tree_insert_word(tree, word, lowercase, meaning);

        free(word); free(lowercase); free(meaning);
        word = lowercase = meaning = NULL;

        free(line);
    }

    return;
}

void tree_insert_file(tree_node_t *tree, char *filename)
{
    FILE *fp;
    char line[1024] = {0, };
    char *word, *lowercase, *meaning, *pos;

    fp = fopen(filename, "rt");
    if (fp == NULL) {
        return;
    }

    while (!feof(fp)) {
        if(!fgets(line, 1024, fp))
            break;

        stripwhite(line);

        if (line[0] == '\0')
            continue;

        if (line[0] == '#')
            continue;

        pos = strchr(line, '\t');
        if (pos) {
            word = strndup(line, pos - line);
            meaning = strdup(pos + 1);
        } else {
            word = strdup(line);
            meaning = strdup("");
        }

        lowercase = strdup(word);
        strtolower(lowercase);

        tree_insert_word(tree, word, lowercase, meaning);

        free(word); free(lowercase); free(meaning);
        word = lowercase = meaning = NULL;
    }

    fclose(fp);
    return;
}

tree_node_t *tree_search(tree_node_t *tree, char *word)
{
    int cmp;
    char *lowercase;
    tree_node_t *q;

    lowercase = strdup(word);
    strtolower(lowercase);

    q = tree;
    while (q) {
        cmp = strcmp(lowercase, q->lowercase);
        if (cmp < 0)
            q = q->left;
        else if (cmp > 0)
            q = q->right;
        if (cmp == 0)
            break;
    }

    free(lowercase);
    return q;
}

void tree_dump(tree_node_t *tree)
{
    tree_node_t *q;

    q = tree;
    if (q) {
        if (strcasecmp(q->word, q->lowercase) == 0
                && strcmp(q->lowercase, "and")) {
            printf("\"%s\t%s\",\n", q->word, q->meaning);
        }
        tree_dump(q->left);
        tree_dump(q->right);
    }
}

void tree_destroy(tree_node_t *tree)
{
    tree_node_t *q = tree;

    if (q) {
        if (q->left)
            tree_destroy(q->left);
        if (q->right)
            tree_destroy(q->right);
        free(q->word); free(q->lowercase); free(q->meaning);
        free(q);
    }
}

static tree_node_t *abbrtree = NULL;

int dict_init(void)
{
    abbrtree = tree_create("and", "and", "don't capitalize it");

    tree_insert_file(abbrtree, "abbr.txt");
    tree_insert_array(abbrtree, abbr, sizeof(abbr)/sizeof(abbr[0]));

    tree_insert_word(abbrtree, "First", "1st", "First");
    tree_insert_word(abbrtree, "Second", "2nd", "Second");
    tree_insert_word(abbrtree, "Third", "3rd", "Third");
    tree_insert_word(abbrtree, "Fourth", "4th", "Fourth");
    tree_insert_word(abbrtree, "Fifth", "5th", "Fifth");
    tree_insert_word(abbrtree, "Sixth", "6th", "Sixth");
    tree_insert_word(abbrtree, "Seventh", "7th", "Seventh");
    tree_insert_word(abbrtree, "Eighth", "8th", "Eighth");
    tree_insert_word(abbrtree, "Ninth", "9th", "Ninth");
    tree_insert_word(abbrtree, "C++", "cplusplus", "C++");
    tree_insert_word(abbrtree, "VisualBasic", "visualbasic", "Visual Basic");
    tree_insert_word(abbrtree, "VisualBasic", "vb", "Visual Basic");
    tree_insert_word(abbrtree, "PowerBuilder", "pb", "Visual Basic");
    tree_insert_word(abbrtree, "Visual C++", "vc", "Visual C++");
    tree_insert_word(abbrtree, "Edition", "ed", "Edition");
    tree_insert_word(abbrtree, "Windows XP", "winxp", "Windows XP");
    tree_insert_word(abbrtree, "Windows XP", "windowsxp", "Windows XP");
    tree_insert_word(abbrtree, "ActionScript3", "as3", "ActionScript3");
    tree_insert_word(abbrtree, "Introduction", "intro", "Introduction");
    tree_insert_word(abbrtree, "WiFi", "wi-fi", "WiFi");
    tree_insert_word(abbrtree, "Chinese", "cn", "Chinese");
    tree_insert_word(abbrtree, "English", "en", "English");
    tree_insert_word(abbrtree, "Microsoft", "ms", "Microsoft");

    return 0;
}

void dict_dump(void)
{
    tree_dump(abbrtree);
}

void dict_fini(void)
{
    tree_destroy(abbrtree);
}

char *dict_search(char *word)
{
    tree_node_t *node;

    node = tree_search(abbrtree, word);
    if (node) {
        return node->word;
    }

    char *c;
    int len = 0;
    int nalpha = 0;
    int ndigit = 0;
    for (c=word; *c; c++) {
        if (isdigit((int)*c)) {
            ndigit++;
        } else if (isalpha((int)*c)) {
            nalpha++;
        }
        len++;
    }

    if (len < 4) {
        if (len == 1) {
            if (strcasecmp(word, "a") == 0) {
                return word;
            } else {
                return capitalize(word);
            }
        } else if (len == 2) {
            if (!ndigit) {
                return word;
            } else {
                return strtoupper(word);
            }
        } else {
            if (!ndigit) {
                return capitalize(word);
            } else {
                return strtoupper(word);
            }
        }
    } else {
        if (!ndigit) {
            return capitalize(word);
        } else {
            if (ndigit > nalpha) {
                return strtoupper(word);
            } else {
                return capitalize(word);
            }
        }
    }
}
