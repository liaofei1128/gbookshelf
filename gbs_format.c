#include "gbookshelf.h"

unsigned int g_format_cnt;
struct list_head g_format_list;

gbs_format_t *gbs_format_alloc(void)
{
    gbs_format_t *format = malloc(sizeof(gbs_format_t));
    if (format == NULL)
        return NULL;

    memset(format, 0, sizeof(gbs_format_t));
    return format;
}

void gbs_format_free(gbs_format_t * format)
{
    if (format) {
        mbsfree(format->format);
        mbsfree(format->description);
        free(format);
    }
}

gbs_format_t *gbs_format_find(char *name)
{
    gbs_format_t *format;

    list_for_each_entry(format, &g_format_list, node) {
        if (!strcmp(format->format, name)) {
            return format;
        }
    }

    return NULL;
}

int gbs_format_insert(char *name, char *description)
{
    gbs_format_t *format = NULL;

    format = gbs_format_find(name);
    if (format)
        return -GBS_ERROR_EXIST;

    format = gbs_format_alloc();
    if (format == NULL)
        return -GBS_ERROR_NOMEM;

    format->format = mbsnewescapesqlite(name);
    format->description = mbsnewescapesqlite(description);
    list_add_tail(&format->node, &g_format_list);
    g_format_cnt++;
    return 0;
}

static gbs_format_t g_default_formats[] = {
    { "AZW", "" },
    { "AZW3", "" },
    { "BIN", "" },
    { "BZ2", "" },
    { "CBR", "" },
    { "CBZ", "" },
    { "CHM", "" },
    { "DJVU", "" },
    { "DOC", "" },
    { "DOCX", "" },
    { "DVI", "" },
    { "EPS", "" },
    { "EPUB", "" },
    { "HTML", "" },
    { "ISO", "" },
    { "MHT", "" },
    { "ODT", "" },
    { "PDF", "" },
    { "PDG", "" },
    { "PS", "" },
    { "RAR", "" },
    { "RBMAKE", "" },
    { "RTF", "" },
    { "TAR", "" },
    { "TEX", "" },
    { "TGZ", "" },
    { "TXT", "" },
    { "ZIP", "" },

    { NULL, NULL },
};

int gbs_format_default_init(void)
{
    gbs_format_t *fmt;

    for (fmt = g_default_formats; fmt->format; fmt++) {
        gbs_format_insert(fmt->format, fmt->description);
    }

    return 0;
}

int gbs_formart_foreach_write_db(sqlite3 *db, int (*insert)(sqlite3 *db, gbs_format_t *fmt))
{
    int ret = 0;
    gbs_format_t *fmt;
    list_for_each_entry(fmt, &g_format_list, node) {
        ret = insert(db, fmt);
        if (ret < 0)
            return ret;
    }

    return 0;
}

int gbs_format_init(void)
{
    INIT_LIST_HEAD(&g_format_list);
    gbs_format_default_init();
    return 0;
}

void gbs_format_fini(void)
{
    gbs_format_t *fmt, *next_format;

    list_for_each_entry_safe(fmt, next_format, &g_format_list, node) {
        list_del(&fmt->node);
        gbs_format_free(fmt);
        g_format_cnt--;
    }

    return;
}


