#include "gbookshelf.h"

unsigned int g_language_cnt;
struct list_head g_language_list;

gbs_language_t *gbs_language_alloc(void)
{
    gbs_language_t *language = malloc(sizeof(gbs_language_t));
    if (language == NULL)
        return NULL;

    memset(language, 0, sizeof(gbs_language_t));
    return language;
}

void gbs_language_free(gbs_language_t * language)
{
    if (language) {
        mbsfree(language->language);
        mbsfree(language->description);
        free(language);
    }
}

gbs_language_t *gbs_language_find(char *name)
{
    gbs_language_t *language;

    list_for_each_entry(language, &g_language_list, node) {
        if (!strcmp(language->language, name)) {
            return language;
        }
    }

    return NULL;
}

int gbs_language_insert(char *name, char *description)
{
    gbs_language_t *language;

    language = gbs_language_find(name);
    if (language)
        return -GBS_ERROR_EXIST;

    language = gbs_language_alloc();
    if (language == NULL)
        return -GBS_ERROR_NOMEM;

    language->language = mbsnewescapesqlite(name);
    language->description = mbsnewescapesqlite(description);
    list_add_tail(&language->node, &g_language_list);
    g_language_cnt++;
    return 0;
}

static gbs_language_name_t g_default_languages[] = {
    { "English", "" },
    { "Chinese", "" },
    { "Japanese", "" },
    { "Korean", "" },
    { "Russian", "" },
    { "French", "" },

    { NULL, NULL },
};

int gbs_language_default_init(void)
{
    gbs_language_name_t *lang;

    for (lang = g_default_languages; lang->language; lang++) {
        gbs_language_insert((char *)lang->language, (char *)lang->description);
    }

    return 0;
}

int gbs_language_foreach_write_db(sqlite3 *db, int (*insert)(sqlite3 *db, gbs_language_t *data))
{
    int ret;
    gbs_language_t *lang;

    list_for_each_entry(lang, &g_language_list, node) {
        ret = insert(db, lang);
        if (ret < 0)
            return ret;
    }
    return 0;
}

int gbs_language_init(void)
{
    INIT_LIST_HEAD(&g_language_list);
    gbs_language_default_init();
    return 0;
}

void gbs_language_fini(void)
{
    gbs_language_t *lang, *next_language;

    list_for_each_entry_safe(lang, next_language, &g_language_list, node) {
        list_del(&lang->node);
        gbs_language_free(lang);
        g_language_cnt--;
    }

    return;
}
