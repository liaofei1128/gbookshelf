#include "gbookshelf.h"

sqlite3 *g_db_ctx = NULL;

static char *g_sql_tables[] = {
    "CREATE TABLE if not exists gbs_format (id INTEGER AUTOINCREMENT, format TEXT NOT NULL, description TEXT, PRIMARY KEY(id))",
    "CREATE TABLE if not exists gbs_language (id INTEGER AUTOINCREMENT, language TEXT NOT NULL, description TEXT, PRIMARY KEY(id))",
    "CREATE TABLE if not exists gbs_publisher (id INTEGER AUTOINCREMENT, publisher TEXT NOT NULL, website TEXT, description TEXT, PRIMARY KEY(id))",
    "CREATE TABLE if not exists gbs_genre (id INTEGER AUTOINCREMENT, path TEXT NOT NULL, genre TEXT NOT NULL, keywords TEXT NOT NULL, PRIMARY KEY(id))",
    "CREATE TABLE if not exists gbs_book (id INTEGER AUTOINCREMENT, md5 TEXT NOT NULL, title TEXT NOT NULL, subtitle TEXT, "
        "isbn TEXT, format TEXT NOT NULL, genre TEXT NOT NULL, subgenre TEXT, "
        "language TEXT, date TEXT, version TEXT, series TEXT, volume TEXT,"
        "publisher TEXT, path TEXT, contents TEXT, introduction TEXT, "
        "pages INT, size INT, scaned INT, years INT, popular INT, price DOUBLE, "
        "authors TEXT, keywords TEXT, urls TEXT, customs TEXT, repository TEXT, "
        "libgenid TEXT, doi TEXT, quality INT, ctime INT, mtime INT, PRIMARY KEY(id))",

    NULL
};

int db_open(char *filename)
{
    int i;
    int exist = 0;
    char *msg = NULL;

    if (g_db_ctx != NULL) {
        gbs_error("db conTEXT NOT NULL!\n");
        return -EINVAL;
    }

    if (access(filename, F_OK) == 0) {
        exist = 1;
    }

    if (sqlite3_open(filename, &g_db_ctx) != 0) {
        gbs_error("Error: open db %s failed\n", filename);
        return -GBS_ERROR_DB;
    }

    i = 0;
    while (g_sql_tables[i]) {
        if (sqlite3_exec(db, g_sql_tables[i], NULL, NULL, &msg) != SQLITE_OK) {
            gbs_error("sqlite3_exec: %s failed, msg %s\n", g_sql_create[i], msg);
            sqlite3_close(g_db_ctx);
            sqlite3_free(msg);
            g_db_ctx = NULL;
            return -GBS_ERROR_DB;
        }
        i++;
    }

    if (!exist) {
        gbs_language_default_init();
        gbs_format_default_init();
        gbs_genre_default_init();
    }

    return 0;
}

int get_last_rowid(void *arg, int argc, char **argv, char **colname)
{
    int *prowid = (int *)arg;
    if (prowid) {
        *prowid = strtol(argv[0], NULL, 0);
    }

    return 0;
}

int db_format_insert(char *format, char *description)
{
    int ret = 0;
    char *msg = NULL;
    mbs_t sql = NULL;

    if (g_db_ctx == NULL) {
        gbs_error("db context is null.\n");
        return -EINVAL;
    }

    mbs_t format = mbsnewescapesqlite(format);
    mbs_t description = mbsnewescapesqlite(description);
    mbscatfmt(&sql, "INSERT INTO gbs_format(format, description) VALUES ('%s', '%s'); SELECT LAST_INSERT_ROWID() FROM gbs_format;", format, description);
    mbsfree(format);mbsfree(description);
    if (sqlite3_exec(g_db_ctx, sql, get_last_rowid, &ret, &msg) != SQLITE_OK) {
        gbs_error("sqlite3_exec: %s failed, msg %s\n", sql, msg);
        sqlite3_free(msg);
        ret = -GBS_ERROR_DB;
    }

    gbs_debug("%s: %d\n", sql, ret);
    mbsfree(sql);
    return ret;
}

int db_language_insert(char *language, char *description)
{
    int ret = 0;
    char *msg = NULL;
    mbs_t sql = NULL;

    if (g_db_ctx == NULL) {
        gbs_error("db context is null.\n");
        return -EINVAL;
    }

    mbs_t language = mbsnewescapesqlite(language);
    mbs_t description = mbsnewescapesqlite(description);
    mbscatfmt(&sql, "INSERT INTO gbs_language(language, description) VALUES ('%s', '%s'); SELECT LAST_INSERT_ROWID() FROM gbs_format;",
        language, description);
    mbsfree(language);mbsfree(description);
    gbs_debug("%s\n", sql);
    if (sqlite3_exec(g_db_ctx, sql, get_last_rowid, &ret, &msg) != SQLITE_OK) {
        gbs_error("sqlite3_exec: %s failed, msg %s\n", sql, msg);
        sqlite3_free(msg);
        ret = -GBS_ERROR_DB;
    }

    mbsfree(sql);
    return ret;
}

int db_publisher_insert(char *publisher, char *website, char *description)
{
    int ret = 0;
    char *msg = NULL;
    mbs_t sql = NULL;

    if (g_db_ctx == NULL) {
        gbs_error("db context is null.\n");
        return -EINVAL;
    }

    mbs_t publisher = mbsnewescapesqlite(publisher);
    mbs_t website = mbsnewescapesqlite(website);
    mbs_t description = mbsnewescapesqlite(description);
    mbscatfmt(&sql, "INSERT INTO gbs_publisher(publisher, website, description) VALUES ('%s', '%s', '%s'); SELECT LAST_INSERT_ROWID() FROM gbs_format;",
        publisher, website, description);
    mbsfree(publisher); mbsfree(website);mbsfree(description);
    gbs_debug("%s\n", sql);
    if (sqlite3_exec(g_db_ctx, sql, get_last_rowid, &ret, &msg) != SQLITE_OK) {
        gbs_error("sqlite3_exec: %s failed, msg %s\n", sql, msg);
        sqlite3_free(msg);
        ret = -GBS_ERROR_DB;
    }

    mbsfree(sql);
    return ret;
}

int db_genre_insert(char *path, char *genre, char *keywords)
{
    int ret = 0;
    char *msg = NULL;
    mbs_t sql = NULL;

    if (g_db_ctx == NULL) {
        gbs_error("db context is null.\n");
        return -EINVAL;
    }

    mbs_t path = mbsnewescapesqlite(path);
    mbs_t genre = mbsnewescapesqlite(genre);
    mbs_t keywords = mbsnewescapesqlite(keywords);
    mbscatfmt(&sql, "INSERT INTO gbs_genre(path, genre, keywords) VALUES ('%s', '%s', '%s'); SELECT LAST_INSERT_ROWID() FROM gbs_format;", path, genre, keywords);
    mbsfree(path);mbsfree(genre);mbsfree(keywords);

    if (sqlite3_exec(g_db_ctx, sql, get_last_rowid, &ret, &msg) != SQLITE_OK) {
        gbs_error("sqlite3_exec: %s failed, msg %s\n", sql, msg);
        sqlite3_free(msg);
        ret = -GBS_ERROR_DB;
    }

    gbs_debug("%s\n", sql);
    mbsfree(sql);
    return ret;
}

int db_genre_delete(char *path, char *genre)
{
    int ret = 0;
    char *msg = NULL;
    mbs_t sql = NULL;

    if (g_db_ctx == NULL) {
        gbs_error("db context is null.\n");
        return -EINVAL;
    }

    mbs_t path = mbsnewescapesqlite(path);
    mbs_t genre = mbsnewescapesqlite(genre);
    mbscatfmt(&sql, "DELETE FROM gbs_genre WHERE path = '%s' AND genre = '%s';", path, genre);
    mbsfree(path);mbsfree(genre);

    if (sqlite3_exec(g_db_ctx, sql, NULL, NULL, &msg) != SQLITE_OK) {
        gbs_error("sqlite3_exec: %s failed, msg %s\n", sql, msg);
        sqlite3_free(msg);
        ret = -GBS_ERROR_DB;
    }

    gbs_debug("%s\n", sql);
    mbsfree(sql);
    return ret;
}


int db_book_insert(gbs_book_t *book)
{
    int ret = 0;
    char *msg = NULL;
    mbs_t sql = NULL;

    if (g_db_ctx == NULL) {
        gbs_error("db context is null.\n");
        return -EINVAL;
    }

    mbscatfmt(&sql,
        "INSERT INTO gbs_book(md5, title, subtitle, isbn, format, genre, subgenre, language, date, version, series, publisher, customs, path, contents, introduction, authors, keywords, urls, pages, size, scaned, years, popular, price, quality, doi, libgenid, repository, ctime, mtime)"
        "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%d', '%d', '%d', '%d', '%d', '%.4f', '%d', '%s', '%s', '%s', '%ld', '%ld')",
        book->md5, book->title, book->subtitle, book->isbn, book->format, book->genre, book->subgenre, book->language, book->date,
        book->version, book->series, book->publisher, book->customs, book->path, book->contents, book->introduction, book->authors,
        book->keywords, book->urls, book->pages, book->size, book->scaned, book->years, book->popular, book->price, book->quality,
        book->doi, book->libgenid, book->repository, book->ctime, book->mtime);
    gbs_debug("%s\n", sql);
    if (sqlite3_exec(g_db_ctx, sql, NULL, NULL, &msg) != SQLITE_OK) {
        gbs_error("sqlite3_exec: %s failed, msg %s\n", sql, msg);
        sqlite3_free(msg);
        ret = -GBS_ERROR_DB;
    }

    mbsfree(sql);
    return 0;
}

int db_write(char *filename)
{
    int ret = 0;
    sqlite3 *db = NULL;

    remove(filename);
    if (sqlite3_open(filename, &db) != 0) {
        gbs_error("Error: %s\n", sqlite3_errmsg(db));
        return -GBS_ERROR_DB;
    }

    ret = db_create(db);
    if (ret < 0)
        goto out;
    ret = gbs_formart_foreach_write_db(db, gbs_db_insert_format);
    if (ret < 0)
        goto out;
    ret = gbs_language_foreach_write_db(db, gbs_db_insert_language);
    if (ret < 0)
        goto out;
    ret = gbs_genre_foreach_write_db(db, gbs_db_insert_genre);
    if (ret < 0)
        goto out;
    ret = gbs_publisher_foreach_write_db(db, gbs_db_insert_publisher);
    if (ret < 0)
        goto out;
    ret = gbs_book_foreach_write_db(db, gbs_db_insert_book);
    if (ret < 0)
        goto out;
out:
    sqlite3_close(db);
    return ret;
}


static int gbs_db_load_format_cb(void *NotUsed, int argc, char **argv,
    char **azColName)
{
#ifdef GBS_DUMP_DATABASE
    int i;
    for (i = 0; i < argc; i++) {
        gbs_debug("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
#else
    if (argc != 2)
        return -GBS_ERROR_DB;

    gbs_format_insert(argv[0], argv[1]);
#endif
    return 0;
}

static int gbs_db_load_language_cb(void *NotUsed, int argc, char **argv,
    char **azColName)
{
#ifdef GBS_DUMP_DATABASE
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
#else
    if (argc != 2)
        return -GBS_ERROR_DB;

    gbs_language_insert(argv[0], argv[1]);
#endif
    return 0;
}

static int gbs_db_load_publisher_cb(void *NotUsed, int argc, char **argv,
    char **azColName)
{
#ifdef GBS_DUMP_DATABASE
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
#else
    if (argc != 3)
        return -GBS_ERROR_DB;

    gbs_publisher_insert(argv[0], argv[1], argv[2]);
#endif
    return 0;
}

static int gbs_db_load_genre_cb(void *NotUsed, int argc, char **argv,
    char **azColName)
{
#ifdef GBS_DUMP_DATABASE
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
#else
    if (argc != 3)
        return -GBS_ERROR_DB;

    gbs_genre_insert(argv[0], argv[1], argv[2]);
#endif
    return 0;
}

static int gbs_db_load_book_cb(void *NotUsed, int argc, char **argv,
    char **azColName)
{
    int ret;
    gbs_book_t *nbook;

#ifdef GBS_DUMP_DATABASE
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
#else
    if (argc != 25)
        return -GBS_ERROR_DB;

    ret = gbs_book_new(&nbook);
    if (ret < 0)
        return ret;

    gbs_book_set_md5(nbook, argv[0]);
    gbs_book_set_title(nbook, argv[1]);
    gbs_book_set_subtitle(nbook, argv[2]);
    gbs_book_set_isbn(nbook, argv[3]);
    gbs_book_set_format(nbook, argv[4]);
    gbs_book_set_genre(nbook, argv[5]);
    gbs_book_set_subgenre(nbook, argv[6]);
    gbs_book_set_language(nbook, argv[7]);
    gbs_book_set_date(nbook, argv[8]);
    gbs_book_set_version(nbook, argv[9]);
    gbs_book_set_series(nbook, argv[10]);
    gbs_book_set_publisher(nbook, argv[11]);
    gbs_book_set_path(nbook, argv[12]);
    gbs_book_set_contents(nbook, argv[13]);
    gbs_book_set_introduction(nbook, argv[14]);
    gbs_book_set_pages(nbook, atoi(argv[15]));
    gbs_book_set_size(nbook, atoi(argv[16]));
    gbs_book_set_scaned(nbook, atoi(argv[17]));
    gbs_book_set_years(nbook, atoi(argv[18]));
    gbs_book_set_popular(nbook, atoi(argv[19]));
    gbs_book_set_price(nbook, atof(argv[20]));
    gbs_book_add_author(nbook, argv[21]);
    gbs_book_add_keyword(nbook, argv[22]);
    gbs_book_add_url(nbook, argv[23]);
    //gbs_book_add_custom(nbook, argv[24]);

    ret = gbs_book_table_insert(nbook);
    if (ret < 0) {
        free(nbook);
        return ret;
    }
#endif

    return 0;
}

int gbs_db_read(char *filename)
{
    sqlite3 *db;
    char *errmsg;

    char *format_sql = "select * from gbs_format";
    char *language_sql = "select * from gbs_language";
    char *publisher_sql = "select * from gbs_publisher";
    char *genre_sql = "select * from gbs_genre";
    char *book_sql = "select * from gbs_book";

    if (sqlite3_open(filename, &db) != 0) {
        gbs_error("Error: %s\n", sqlite3_errmsg(db));
        return -GBS_ERROR_DB;
    }

    if (sqlite3_exec(db, format_sql, gbs_db_load_format_cb, 0,
            &errmsg) != SQLITE_OK) {
        gbs_error("Error: %s\n", errmsg);
        sqlite3_free(errmsg);
    }

    if (sqlite3_exec(db, language_sql, gbs_db_load_language_cb, 0,
            &errmsg) != SQLITE_OK) {
        gbs_error("Error: %s\n", errmsg);
        sqlite3_free(errmsg);
    }

    if (sqlite3_exec(db, publisher_sql, gbs_db_load_publisher_cb, 0,
            &errmsg) != SQLITE_OK) {
        gbs_error("Error: %s\n", errmsg);
        sqlite3_free(errmsg);
    }

    if (sqlite3_exec(db, genre_sql, gbs_db_load_genre_cb, 0,
            &errmsg) != SQLITE_OK) {
        gbs_error("Error: %s\n", errmsg);
        sqlite3_free(errmsg);
    }

    if (sqlite3_exec(db, book_sql, gbs_db_load_book_cb, 0,
            &errmsg) != SQLITE_OK) {
        gbs_error("Error: %s\n", errmsg);
        sqlite3_free(errmsg);
    }

    sqlite3_close(db);
    return 0;
}
