#include "gbookshelf.h"

/**
 * gbs_book table management
 */

static unsigned int g_book_cnt;
static struct list_head g_book_list;
static gbs_book_hash_t gbs_book_htable[GBS_HASH_SIZE];

unsigned int gbs_book_hash_by_md5(gbs_book_t * book)
{
    unsigned int hval;

    hval = DJBHash(book->md5, mbslen(book->md5));

    return hval % GBS_HASH_SIZE;
}

unsigned int gbs_book_hash_by_title(gbs_book_t * book)
{
    unsigned int hval;

    hval = FNVHash(book->title, mbslen(book->title));

    return hval % GBS_HASH_SIZE;
}

int gbs_book_table_insert(gbs_book_t * book)
{
    unsigned int hval;
    gbs_book_hash_t *h;
    gbs_book_t *cur_book;

    if (book->md5 == NULL || book->title == NULL)
        return -GBS_ERROR_INVAL;

    hval = gbs_book_hash_by_md5(book);
    h = gbs_book_htable + hval;
    list_for_each_entry(cur_book, &h->md5_head, md5_node) {
        if (!mbscmp(cur_book->md5, book->md5)) {
            return -GBS_ERROR_EXIST;
        }
    }

    list_add_tail(&book->md5_node, &h->md5_head);

    hval = gbs_book_hash_by_title(book);
    h = gbs_book_htable + hval;
    list_add_tail(&book->title_node, &h->title_head);

    list_add_tail(&book->node, &g_book_list);
    g_book_cnt++;
    return 0;
}

gbs_book_t *gbs_book_table_find(gbs_book_t * user)
{
    unsigned int hval;
    gbs_book_hash_t *h;
    gbs_book_t *cur_book;

    if (user->md5) {
        hval = gbs_book_hash_by_md5(user);
        h = gbs_book_htable + hval;
        list_for_each_entry(cur_book, &h->md5_head, md5_node) {
            if (!mbscmp(cur_book->md5, user->md5)) {
                return cur_book;
            }
        }
    }

    if (user->title) {
        hval = gbs_book_hash_by_title(user);
        h = gbs_book_htable + hval;
        list_for_each_entry(cur_book, &h->title_head, title_node) {
            if (!mbscmp(cur_book->title, user->title)) {
                return cur_book;
            }
        }
    }

    return NULL;
}

int gbs_book_table_delete(gbs_book_t * user)
{
    gbs_book_t *cur_book;

    cur_book = gbs_book_table_find(user);
    if (cur_book) {
        list_del(&cur_book->node);
        list_del(&cur_book->md5_node);
        list_del(&cur_book->title_node);
        gbs_book_destroy(cur_book);
        g_book_cnt--;
        return 0;
    }

    return -GBS_ERROR_NOT_EXIST;
}

int gbs_book_table_find_cb(gbs_book_t * user,
    int (*match_func)(gbs_book_t * cur,
        gbs_book_t * user),
    void(*callback)(gbs_book_t * cur, void *data), void *data)
{
    int mc = 0;
    gbs_book_t *cur_book;

    list_for_each_entry(cur_book, &g_book_list, node) {
        if (match_func(cur_book, user) == 1) {
            callback(cur_book, data);
            mc++;
        }
    }

    return mc;
}

/**
 * gbs_book api
 */

int gbs_book_new(gbs_book_t ** book)
{
    gbs_book_t *nbook;

    nbook = malloc(sizeof(gbs_book_t));
    if (nbook == NULL)
        return -GBS_ERROR_NOMEM;

    memset(nbook, 0, sizeof(gbs_book_t));
    *book = nbook;
    return 0;
}

int gbs_book_add_author(gbs_book_t * book, char *author)
{
    mbs_t *nauthor;

    if (book->authors) {
        book->nauthor++;
        book->authors =
            realloc(book->authors, book->nauthor * sizeof(mbs_t));
        if (book->authors == NULL)
            return -GBS_ERROR_NOMEM;
        nauthor = &book->authors[book->nauthor - 1];
        memset(nauthor, 0, sizeof(mbs_t)); /**< must set zero!! */
        nauthor = mbsnewescapesqlite(author);
    } else {
        book->nauthor = 1;
        book->authors = malloc(book->nauthor * sizeof(mbs_t));
        if (book->authors == NULL)
            return -GBS_ERROR_NOMEM;

        nauthor = &book->authors[book->nauthor - 1];
        memset(nauthor, 0, sizeof(mbs_t));
        nauthor = mbsnewescapesqlite(author);
    }
    return 0;
}

int gbs_book_clr_author(gbs_book_t * book)
{
    int i;
    for (i = 0; i < book->nauthor; i++) {
        mbsfree(book->authors[i]);
    }
    free(book->authors);
    book->authors = NULL;
    book->nauthor = 0;
    return 0;
}

int gbs_book_add_keyword(gbs_book_t * book, char *keyword)
{
    mbs_t *nkeyword;

    if (book->keywords) {
        book->nkeyword++;
        book->keywords =
            malloc(book->keywords,
            book->nkeyword * sizeof(mbs_t));
        if (book->keywords == NULL)
            return -GBS_ERROR_NOMEM;
        nkeyword = &book->keywords[book->nkeyword - 1];
        memset(nkeyword, 0, sizeof(mbs_t)); /**< must set zero!! */
        nkeyword = mbsnewescapesqlite(keyword);
    } else {
        book->nkeyword = 1;
        book->keywords = malloc(book->nkeyword * sizeof(mbs_t));
        if (book->keywords == NULL)
            return -GBS_ERROR_NOMEM;

        nkeyword = &book->keywords[book->nkeyword - 1];
        memset(nkeyword, 0, sizeof(mbs_t));
        nkeyword = mbsnewescapesqlite(keyword);
    }
    return 0;
}

int gbs_book_clr_keyword(gbs_book_t * book)
{
    int i;
    for (i = 0; i < book->nkeyword; i++) {
        mbsfree(book->keywords[i]);
    }
    free(book->keywords);
    book->keywords = NULL;
    book->nkeyword = 0;
    return 0;
}

int gbs_book_add_url(gbs_book_t * book, char *url)
{
    mbs_t *nurl;

    if (book->urls) {
        book->nurl++;
        book->urls =
            realloc(book->urls, book->nurl * sizeof(mbs_t));
        if (book->urls == NULL)
            return -GBS_ERROR_NOMEM;
        nurl = &book->urls[book->nurl - 1];
        memset(nurl, 0, sizeof(mbs_t)); /**< must set zero!! */
        nurl = mbsnewescapesqlite(url);
    } else {
        book->nurl = 1;
        book->urls = malloc(book->nurl * sizeof(mbs_t));
        if (book->urls == NULL)
            return -GBS_ERROR_NOMEM;

        nurl = &book->urls[book->nurl - 1];
        memset(nurl, 0, sizeof(mbs_t));
        nurl = mbsnewescapesqlite(url);
    }
    return 0;
}

int gbs_book_clr_url(gbs_book_t * book)
{
    int i;
    for (i = 0; i < book->nurl; i++) {
        mbsfree(book->urls[i]);
    }
    free(book->urls);
    book->urls = NULL;
    book->nurl = 0;
    return 0;
}

int gbs_book_add_custom(gbs_book_t * book, char *key, char *value)
{
    gbs_keyval_t *ncustom;

    gbs_debug("key=%s,value=%s\n", key, value);
    if (book->customs) {
        book->ncustom++;
        book->customs =
            realloc(book->customs,
            book->ncustom * sizeof(gbs_keyval_t));
        if (book->customs == NULL)
            return -GBS_ERROR_NOMEM;
        ncustom = &book->customs[book->ncustom - 1];
        memset(ncustom, 0, sizeof(gbs_keyval_t)); /**< must set zero!! */
        ncustom->key = mbsnewescapesqlite(key);
        ncustom->value = mbsnewescapesqlite(value);
    } else {
        book->ncustom = 1;
        book->customs = malloc(book->ncustom * sizeof(gbs_keyval_t));
        if (book->customs == NULL)
            return -GBS_ERROR_NOMEM;

        ncustom = &book->customs[book->ncustom - 1];
        memset(ncustom, 0, sizeof(gbs_keyval_t));
        ncustom->key = mbsnewescapesqlite(key);
        ncustom->value = mbsnewescapesqlite(value);
    }
    return 0;
}

int gbs_book_clr_custom(gbs_book_t * book)
{
    int i;
    gbs_keyval_t *ncustom;
    for (i = 0; i < book->ncustom; i++) {
        ncustom = &book->customs[i];
        mbsfree(ncustom->key);
        mbsfree(ncustom->value);
    }
    free(book->customs);
    book->customs = NULL;
    book->ncustom = 0;
    return 0;
}

void gbs_book_destroy(gbs_book_t * book)
{
    return;
}

int gbs_book_copy(gbs_book_t * dst, gbs_book_t * src)
{
    return 0;
}

int gbs_book_match_by_author(gbs_book_t * cur, gbs_book_t * user)
{
    return 0;
}

int gbs_book_match_by_publisher(gbs_book_t * cur, gbs_book_t * user)
{
    return !mbscmp(cur->publisher, user->publisher);
}

int gbs_book_match_by_keyword(gbs_book_t * cur, gbs_book_t * user)
{
    return 0;
}

int gbs_book_match_by_date(gbs_book_t * cur, gbs_book_t * user)
{
    return 0;
}

int gbs_book_match_by_genre(gbs_book_t * cur, gbs_book_t * user)
{
    return !mbscmp(cur->genre, user->genre);
}

int gbs_book_match_by_subgenre(gbs_book_t * cur, gbs_book_t * user)
{
    return !mbscmp(cur->subgenre, user->subgenre);
}

int gbs_book_foreach_write_db(void *db,
    int (*insert)(void *db, void *data))
{
    int ret;
    gbs_book_t *book;

    list_for_each_entry(book, &g_book_list, node) {
        ret = insert(db, book);
        if (ret < 0)
            return ret;
    }
    return 0;
}

void gbs_book_console_show(gbs_book_t * book, void *data)
{
    int i;
    printf("%-15s: %s\n", "title", book->title);
    printf("%-15s: %s\n", "subtitle", book->subtitle);
    printf("%-15s: %s\n", "md5", book->md5.data);
    printf("%-15s: %s\n", "path", book->path);
    printf("%-15s: %s\n", "language", book->language);
    printf("%-15s: %s\n", "isbn", book->isbn);
    printf("%-15s: %s\n", "format", book->format);
    printf("%-15s: %s\n", "genre", book->genre);
    printf("%-15s: %s\n", "subgenre", book->subgenre);
    printf("%-15s: %s\n", "date", book->date);
    printf("%-15s: %s\n", "version", book->version);
    printf("%-15s: %s\n", "series", book->series);
    printf("%-15s: %s\n", "publisher", book->publisher);
    printf("%-15s: %s\n", "contents", book->contents);
    printf("%-15s: %s\n", "introduction", book->introduction);
    printf("%-15s: %d\n", "size", book->size);
    printf("%-15s: %d\n", "pages", book->pages);
    printf("%-15s: %d\n", "years", book->years);
    printf("%-15s: %d\n", "popular", book->popular);
    printf("%-15s: %f\n", "price", book->price);
    printf("%-15s: ", "authors");
    for (i = 0; i < book->nauthor; i++) {
        printf("%s,", book->authors[i]);
    }
    printf("\n");
    printf("%-15s: ", "keywords");
    for (i = 0; i < book->nkeyword; i++) {
        printf("%s,", book->keywords[i]);
    }
    printf("\n");
}

static gbs_book_t *g_cur_page_first_pos;
static gbs_book_t *g_cur_page_last_pos;

int gbs_book_init(void)
{
    int i;
    gbs_book_hash_t *h;

    g_cur_page_first_pos = NULL;
    g_cur_page_last_pos = NULL;

    INIT_LIST_HEAD(&g_book_list);
    for (i = 0; i < GBS_HASH_SIZE; i++) {
        h = gbs_book_htable + i;
        INIT_LIST_HEAD(&h->md5_head);
        INIT_LIST_HEAD(&h->title_head);
    }

    return 0;
}

void gbs_book_fini(void)
{
    gbs_book_t *cur_book, *next_book;

    list_for_each_entry_safe(cur_book, next_book, &g_book_list, node) {
        list_del(&cur_book->node);
        list_del(&cur_book->md5_node);
        list_del(&cur_book->title_node);
        gbs_book_destroy(cur_book);
        g_book_cnt--;
    }

    return;
}


