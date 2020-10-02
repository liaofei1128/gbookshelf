#ifndef _GBOOKSHELF_H_
#define _GBOOKSHELF_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <gtk/gtk.h>
#include <pango/pango.h>
#include <gdk/gdkkeysyms.h>
#include "gbs_icons.h"

#include "libmbs.h"
#include "libdpa.h"
#include "libhash.h"
#include "liblist.h"
#include "libstring.h"
#include "libmdfa.h"
#include "sqlite3.h"

#define GBS_AUTHOR              "liaofei1128@gmail.com"
#define GBS_WEBSITE             "http://www.gbookshelf.com"
#define GBS_DATABASE_NAME       "gbs.db"

#define ARRAY_SIZE(x)           (sizeof(x) / sizeof((x)[0]))
#define str_empty(x)            (x[0] == '\0')
#define str_equal(x, y)         (strcmp(x, y) == 0)
#define md5_equal(x, y)         (memcmp(x, y, 16) == 0)

#define GBS_ID_SIZE                 2*1024*1024 /*< I can hold 2M books at most */
#define GBS_HASH_SIZE               1024
#define GBS_DEBUG(str, args...)     printf("[%s][%s][%d]"str, __FILE__, __FUNCTION__, __LINE__, ##args)

#define gbs_print(str, args...) fprintf(stdout, str, ##args)
#define gbs_error(str, args...) fprintf(stderr, str, ##args)

#ifdef GBS_DEBUG_ENABLE
#define gbs_debug(str, args...) fprintf(stderr, "[%s][%d]"str, __FILE__, __LINE__, ##args)
#else
#define gbs_debug(str, args...)
#endif

enum {
    GBS_OK = 0,
    GBS_ERROR_NOMEM,
    GBS_ERROR_INVAL,
    GBS_ERROR_FILE,
    GBS_ERROR_DB,

    GBS_ERROR_EXIST,
    GBS_ERROR_NOT_EXIST,
};

static inline char *gbs_err(int err)
{
    switch (err) {
        case -GBS_ERROR_NOMEM: return "not enough malloc.";
        case -GBS_ERROR_INVAL: return "not invalid argument.";
        case -GBS_ERROR_FILE: return "failed when oprate file.";
        case -GBS_ERROR_DB: return "failed when oprate database.";
        case -GBS_ERROR_EXIST: return "entry was exist.";
        case -GBS_ERROR_NOT_EXIST: return "entry was not exist.";
    }

    return "unknow error!";
}

typedef struct gbs_genre_name_st {
    char *path;
    char *genre;
    char *keywords; // we can guess the genre of the input file by its filename.
} gbs_genre_name_t;


typedef struct gbs_genre_st {
    int id;
    mbs_t path;
    mbs_t genre;
    mbs_t parent;
    mbs_t fullpath;
    mbs_t keywords; // we can guess the genre of the input file by its filename.
    mdfa_t *pattern;
    struct list_head node;
} gbs_genre_t;

typedef struct gbs_publisher_name_st {
    char *publisher;
    char *website;
    char *description;
} gbs_publisher_name_t;

typedef struct gbs_publisher_st {
    int id;
    mbs_t publisher;
    mbs_t website;
    mbs_t description;
    struct list_head node;
} gbs_publisher_t;

typedef struct gbs_format_st {
    int id;
    mbs_t format;
    mbs_t description;
    struct list_head node;
} gbs_format_t;

typedef struct gbs_language_name_st {
    char *language;
    char *description;
} gbs_language_name_t;

typedef struct gbs_language_st {
    int id;
    mbs_t language;
    mbs_t description;
    struct list_head node;
} gbs_language_t;

typedef struct gbs_keyval_st {
    mbs_t key;
    mbs_t value;
} gbs_keyval_t;

enum {
    GBS_BOOK_COLUMN_SCANED,
    GBS_BOOK_COLUMN_FORMAT,
    GBS_BOOK_COLUMN_TITLE,
    GBS_BOOK_COLUMN_AUTHOR,
    GBS_BOOK_COLUMN_PUBLISHER,
    GBS_BOOK_COLUMN_LANGUAGE,
    GBS_BOOK_COLUMN_VERSION,
    GBS_BOOK_COLUMN_MAX,
};

typedef struct gbs_book_st {
    int id;
    int size;
    int pages;
    int scaned;                 /*< is this book scaned? */
    int years;                  /*< the eras of this book */
    int popular;                /*< how popular this book */
    int quality;
    time_t ctime;
    time_t mtime;
    double price;

    struct list_head node;            /*< linked in the global double list */
    struct list_head md5_node;        /*< linked in the md5 hash table */
    struct list_head title_node;      /*< linked in the title hash table */

    mbs_t md5;
    mbs_t isbn;
    mbs_t format;
    mbs_t genre;
    mbs_t subgenre;
    mbs_t language;

    mbs_t date;             /*< release time */
    mbs_t version;
    mbs_t series;
    mbs_t title;
    mbs_t subtitle;
    mbs_t publisher;

    mbs_t path;
    mbs_t contents;
    mbs_t introduction;

    mbs_t doi;
    mbs_t libgenid;
    mbs_t repository;

    mbs_t urls;
    mbs_t authors;
    mbs_t keywords;
    mbs_t customs;

    dpa_t _urls;
    dpa_t _authors;
    dpa_t _keywords;
    dpa_t _customs;
} gbs_book_t;

typedef struct gbs_book_hash_st {
    struct list_head md5_head;
    struct list_head title_head;
} gbs_book_hash_t;

typedef struct tGbsAddBookWindow {
    GtkWidget *AddBookWindow;
    GtkWidget *AddBookMainVBox;
    GtkWidget *AddBookEditMainVBox;
    GtkWidget *AddBookEditTopHBox;
    GtkWidget *AddBookEditTopPreview;
    GtkWidget *AddBookEditTopRightTable;
    GtkWidget *AddBookTitleLabel;
    GtkWidget *AddBookTitleEntry;
    GtkWidget *AddBookTitleButton;
    GtkWidget *AddBookScanedToggle;
    GtkWidget *AddBookSubtitleLabel;
    GtkWidget *AddBookSubtitleEntry;
    GtkWidget *AddBookPriceLabel;
    GtkWidget *AddBookPriceEntry;
    GtkWidget *AddBookEditTopRightSep;
    GtkWidget *AddBookSizeLabel;
    GtkWidget *AddBookSizeEntry;
    GtkWidget *AddBookMd5Label;
    GtkWidget *AddBookMd5Entry;
    GtkWidget *AddBookPathLabel;
    GtkWidget *AddBookPathEntry;
    GtkWidget *AddBookPageLabel;
    GtkWidget *AddBookPageEntry;
    GtkWidget *AddBookGenreLabel;
    GtkWidget *AddBookGenreComboBoxEntry;
    GtkWidget *AddBookSubgenreLabel;
    GtkWidget *AddBookSubgenreComboBoxEntry;
    GtkWidget *AddBookPopularLabel;
    GtkWidget *AddBookPopularSpinButton;
    GtkWidget *AddBookIsbnLabel;
    GtkWidget *AddBookIsbnEntry;
    GtkWidget *AddBookLanguageLabel;
    GtkWidget *AddBookLanguageComboBoxEntry;
    GtkWidget *AddBookFormatLabel;
    GtkWidget *AddBookFormatComboBoxEntry;
    GtkWidget *AddBookPublisherLabel;
    GtkWidget *AddBookPublisherComboBoxEntry;
    GtkWidget *AddBookDateLabel;
    GtkWidget *AddBookDateEntry;
    GtkWidget *AddBookVersionLabel;
    GtkWidget *AddBookVersionComboBoxEntry;
    GtkWidget *AddBookEditMiddleSep;
    GtkWidget *AddBookEditMiddleHBox;

    GtkWidget *AddBookAuthorsFrame;
    GtkWidget *AddBookAuthorsHBox;
    GtkWidget *AddBookAuthorsHBoxVBoxLeft;
    GtkWidget *AddBookAuthorsAddWidgetEventBox;
    GtkWidget *AddBookAuthorsAddWidget;
    GtkWidget *AddBookAuthorsDelWidgetEventBox;
    GtkWidget *AddBookAuthorsDelWidget;
    GtkWidget *AddBookAuthorsClrWidgetEventBox;
    GtkWidget *AddBookAuthorsClrWidget;
    GtkWidget *AddBookAuthorsHBoxVBoxRight;
    GtkWidget *AddBookAuthorsTreeviewScrollWindow;
    GtkWidget *AddBookAuthorsTreeview;

    GtkWidget *AddBookKeywordsFrame;
    GtkWidget *AddBookKeywordsHBox;
    GtkWidget *AddBookKeywordsHBoxVBoxLeft;
    GtkWidget *AddBookKeywordsAddWidgetEventBox;
    GtkWidget *AddBookKeywordsAddWidget;
    GtkWidget *AddBookKeywordsDelWidgetEventBox;
    GtkWidget *AddBookKeywordsDelWidget;
    GtkWidget *AddBookKeywordsClrWidgetEventBox;
    GtkWidget *AddBookKeywordsClrWidget;
    GtkWidget *AddBookKeywordsHBoxVBoxRight;
    GtkWidget *AddBookKeywordsTreeviewScrollWindow;
    GtkWidget *AddBookKeywordsTreeview;

    GtkWidget *AddBookEditMoreExpander;
    GtkWidget *AddBookEditMoreVbox;
    GtkWidget *AddBookEditMoreVboxHbox1;
    GtkWidget *AddBookEditMoreSep;
    GtkWidget *AddBookEditMoreVboxHbox2;
    GtkWidget *AddBookSeriesLabel;
    GtkWidget *AddBookSeriesEntry;
    GtkWidget *AddBookEditMoreVboxHbox3;
    GtkWidget *AddBookWebsiteFrame;
    GtkWidget *AddBookWebsiteHBox;
    GtkWidget *AddBookWebsiteHBoxVBoxLeft;
    GtkWidget *AddBookWebsiteAddWidgetEventBox;
    GtkWidget *AddBookWebsiteAddWidget;
    GtkWidget *AddBookWebsiteDelWidgetEventBox;
    GtkWidget *AddBookWebsiteDelWidget;
    GtkWidget *AddBookWebsiteClrWidgetEventBox;
    GtkWidget *AddBookWebsiteClrWidget;
    GtkWidget *AddBookWebsiteHBoxVBoxRight;
    GtkWidget *AddBookWebsiteTreeviewScrollWindow;
    GtkWidget *AddBookWebsiteTreeview;
    GtkWidget *AddBookCustomFrame;
    GtkWidget *AddBookCustomHBox;
    GtkWidget *AddBookCustomHBoxVBoxLeft;
    GtkWidget *AddBookCustomAddWidgetEventBox;
    GtkWidget *AddBookCustomAddWidget;
    GtkWidget *AddBookCustomDelWidgetEventBox;
    GtkWidget *AddBookCustomDelWidget;
    GtkWidget *AddBookCustomClrWidgetEventBox;
    GtkWidget *AddBookCustomClrWidget;
    GtkWidget *AddBookCustomHBoxVBoxRight;
    GtkWidget *AddBookCustomTreeviewScrollWindow;
    GtkWidget *AddBookCustomTreeview;

    GtkWidget *AddBookEditMoreVboxHbox4;
    GtkWidget *AddBookIntroductionFrame;
    GtkWidget *AddBookIntroductionScrollWindow;
    GtkWidget *AddBookIntroductionTextview;

    GtkWidget *AddBookEditMoreVboxHbox5;
    GtkWidget *AddBookContentsFrame;
    GtkWidget *AddBookContentsScrollWindow;
    GtkWidget *AddBookContentsTextview;

    GtkWidget *AddBookActionButtonBox;
    GtkWidget *AddBookActionButtonReset;
    GtkWidget *AddBookActionButtonAddContinue;
    GtkWidget *AddBookActionButtonAddQuit;
    GtkWidget *AddBookActionButtonCancel;
} GbsAddBookWindow;

static inline int parse_dpa(char *str, char *dem, dpa_t *dpa)
{
    int i;
    int ret;
    char **wordlist = NULL;

    ret = parse_wordlist(str, dem, &wordlist);
    if (ret < 0) {
        return ret;
    }

    dpa_init(dpa, ret);
    for (i=0; i<ret; i++) {
        dpa->array[i] = wordlist[i];
    }

    free(wordlist);
    return ret;
}

/* gbs_book.c */
extern unsigned int gbs_book_hash_by_md5(gbs_book_t *book);
extern unsigned int gbs_book_hash_by_title(gbs_book_t *book);
extern int gbs_book_table_insert(gbs_book_t *book);
extern gbs_book_t *gbs_book_table_find(gbs_book_t *user);
extern int gbs_book_table_delete(gbs_book_t *user);
extern int gbs_book_table_find_cb(gbs_book_t *user, int (*match_func)(gbs_book_t *cur, gbs_book_t *user), void (*callback)(gbs_book_t *cur, void *data), void *data);
extern int gbs_book_new(gbs_book_t **book);
extern int gbs_book_add_author(gbs_book_t *book, char *author);
extern int gbs_book_clr_author(gbs_book_t * book);
extern int gbs_book_add_keyword(gbs_book_t *book, char *keyword);
extern int gbs_book_clr_keyword(gbs_book_t * book);
extern int gbs_book_add_url(gbs_book_t *book, char *url);
extern int gbs_book_clr_url(gbs_book_t * book);
extern int gbs_book_add_custom(gbs_book_t * book, char *key, char *value);
extern int gbs_book_clr_custom(gbs_book_t * book);
extern void gbs_book_destroy(gbs_book_t *book);
extern int gbs_book_copy(gbs_book_t *dst, gbs_book_t *src);
extern int gbs_book_match_by_author(gbs_book_t *cur, gbs_book_t *user);
extern int gbs_book_match_by_publisher(gbs_book_t *cur, gbs_book_t *user);
extern int gbs_book_match_by_keyword(gbs_book_t *cur, gbs_book_t *user);
extern int gbs_book_match_by_date(gbs_book_t *cur, gbs_book_t *user);
extern int gbs_book_match_by_genre(gbs_book_t *cur, gbs_book_t *user);
extern int gbs_book_match_by_subgenre(gbs_book_t *cur, gbs_book_t *user);
extern int gbs_book_foreach_write_db(sqlite3 *db, int (*insert)(sqlite3 *db, void *data));
extern void gbs_book_console_show(gbs_book_t *book, void *data);
extern int gbs_book_init(void);
extern void gbs_book_fini(void);

/* gbs_book_ui.c */
extern GtkTreeModel *gbs_book_create_model(void);
extern void gbs_book_update_model_first(GtkListStore *treestore, int page_size);
extern void gbs_book_update_model_next(GtkListStore * liststore, int page_size);
extern void gbs_book_update_model_prev(GtkListStore *treestore, int page_size);
extern void gbs_book_update_model_last(GtkListStore *treestore, int page_size);
extern void gbs_gtk_book_add(void);
extern void gbs_gtk_book_del(void);

/* gbs_genre.c */
extern dpa_t g_main_genres;
extern unsigned int g_genre_cnt;
extern struct list_head g_genre_list;
extern gbs_genre_t *gbs_genre_alloc(void);
extern void gbs_genre_free(gbs_genre_t *genre);
extern int gbs_genre_insert(char *path, char *genre, char *keywords);
extern int gbs_genre_delete(char *genre, char *subgenre);
extern int gbs_genre_default_init(void);
extern void gbs_genre_dump(void);
extern int gbs_genre_foreach_write_db(sqlite3 *db, int (*insert)(sqlite3 *db, gbs_genre_t *gen));
extern int gbs_genre_init(void);
extern void gbs_genre_fini(void);
/* gbs_genre_ui.c */
extern GtkTreeModel *gbs_genre_create_model(void);
extern GtkTreeModel *gbs_subgenre_create_model(char *name);
extern void gbs_genre_update_tree_model(GtkTreeStore *treestore);
extern void gbs_genre_combox_append(GtkComboBox *combo);
extern void gbs_subgenre_combox_append(GtkComboBox *combo, char *name);
extern void gbs_gtk_genre_add(void);
extern void gbs_gtk_genre_del(void);
/* gbs_publisher.c */
extern gbs_publisher_t *gbs_publisher_alloc(void);
extern void gbs_publisher_free(gbs_publisher_t *publisher);
extern gbs_publisher_t *gbs_publisher_find(char *name);
extern int gbs_publisher_insert(char *name, char *website, char *description);
extern int gbs_publisher_delete(char *name);
extern int gbs_publisher_default_init(void);
extern int gbs_publisher_foreach_write_db(sqlite3 *db, int (*insert)(sqlite3 *db, gbs_publisher_t *data));
extern int gbs_publisher_init(void);
extern void gbs_publisher_fini(void);
/* gbs_publisher_ui.c */
extern GtkTreeModel *gbs_publisher_create_model(void);
extern void gbs_publisher_update_tree_model(GtkTreeStore *treestore);
extern void gbs_publisher_combox_append(GtkComboBox *combo);
void gbs_gtk_publisher_add(void);
void gbs_gtk_publisher_del(void);
/* gbs_format.c */
extern unsigned int g_format_cnt;
extern struct list_head g_format_list;
extern gbs_format_t *gbs_format_alloc(void);
extern void gbs_format_free(gbs_format_t *format);
extern gbs_format_t *gbs_format_find(char *name);
extern int gbs_format_insert(char *name, char *description);
extern int gbs_format_default_init(void);
extern int gbs_formart_foreach_write_db(sqlite3 *db, int (*insert)(sqlite3 *db, gbs_format_t *data));
extern int gbs_format_init(void);
extern void gbs_format_fini(void);
/* gbs_format_ui.c */
extern GtkTreeModel *gbs_format_create_model(void);
extern void gbs_format_update_tree_model(GtkTreeStore *treestore);
extern void gbs_format_combox_append(GtkComboBox *combo);
extern int gbs_format_combox_insert_new(GtkComboBox *combo, char *suffix);
extern GdkPixbuf *gbs_format_get_pixbuf(char *format);
/* gbs_language.c */
extern unsigned int g_language_cnt;
extern struct list_head g_language_list;
extern gbs_language_t *gbs_language_alloc(void);
extern void gbs_language_free(gbs_language_t *language);
extern gbs_language_t *gbs_language_find(char *name);
extern int gbs_language_insert(char *name, char *description);
extern int gbs_language_default_init(void);
extern int gbs_language_foreach_write_db(sqlite3 *db, int (*insert)(sqlite3 *db, gbs_language_t *data));
extern int gbs_language_init(void);
extern void gbs_language_fini(void);
/* gbs_language_ui.c */
extern GtkTreeModel *gbs_language_create_model(void);
extern void gbs_language_update_tree_model(GtkTreeStore *treestore);
extern void gbs_language_combox_append(GtkComboBox *combo);
/* gbs_db.c */
extern int db_create(sqlite3 *db);
extern int db_format_insert(sqlite3 *db, char *format, char *description);
extern int db_language_insert(sqlite3 *db, char *language, char *description);
extern int db_publisher_insert(sqlite3 *db, char *publisher, char *website, char *description);
extern int db_genre_insert(sqlite3 *db, char *path, char *genre, char *keywords);
extern int db_book_insert(sqlite3 *db, gbs_book_t *book);
extern int db_write(char *filename);
extern int gbs_db_read(char *filename);

#endif
