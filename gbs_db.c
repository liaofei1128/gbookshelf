#include "gbookshelf.h"

sqlite3 *g_db_ctx = NULL;

static char *g_sql_create[] = {
    "CREATE TABLE if not exists gbs_format (format text not null, description text, primary key(format))",
    "CREATE TABLE if not exists gbs_language (language text not null, description text, primary key(language))",
    "CREATE TABLE if not exists gbs_publisher (publisher text not null, website text, description text, primary key(publisher))",
    "CREATE TABLE if not exists gbs_genre (genre text not null, subgenre text not null, aliases text, primary key(genre, subgenre))",
    "CREATE TABLE if not exists gbs_book (id text not null, md5 text not null, title text not null, subtitle text, "
        "isbn text, format text not null, genre text not null, subgenre text, "
        "language text, date text, version text, series text, volume text,"
        "publisher text, path text, contents text, introduction text, "
        "pages int, size int, scaned int, years int, popular int, price double, "
        "authors text, keywords text, urls text, customs text, repository text, "
        "libgenid text, doi text, quality int, ctime int, mtime int, primary key(id))",

    NULL
};

int gbs_db_create(sqlite3 *db)
{
    int i;
    char *msg;

    i = 0;
    while (g_sql_create[i]) {
        if (sqlite3_exec(db, g_sql_create[i], NULL, NULL, &msg) !=
            SQLITE_OK) {
            gbs_error("sqlite3_exec: %s failed, msg %s\n", g_sql_create[i],
                msg);
            sqlite3_free(msg);
            return -GBS_ERROR_DB;
        }
        i++;
    }

    return 0;
}

int gbs_db_insert_format(sqlite3 *db, gbs_format_t *fmt)
{
    int ret = 0;
    char *msg = NULL;
    mbs_t sql = NULL;

    mbscatfmt(&sql, "INSERT INTO gbs_format(format, description) VALUES ('%s', '%s')", fmt->format, fmt->description);
    gbs_debug("%s\n", sql);
    if (sqlite3_exec(db, sql, NULL, NULL, &msg) != SQLITE_OK) {
        gbs_error("sqlite3_exec: %s failed, msg %s\n", sql, msg);
        sqlite3_free(msg);
        ret = -GBS_ERROR_DB;
    }

    mbsfree(sql);
    return ret;
}

int gbs_db_insert_language(sqlite3 *db, gbs_language_t *lang)
{
    int ret = 0;
    char *msg = NULL;
    mbs_t sql = NULL;

    mbscatfmt(&sql, "INSERT INTO gbs_language(language, description) VALUES l('%s', '%s')", lang->language, lang->description);
    gbs_debug("%s\n", sql);
    if (sqlite3_exec(db, sql, NULL, NULL, &msg) != SQLITE_OK) {
        gbs_error("sqlite3_exec: %s failed, msg %s\n", sql, msg);
        sqlite3_free(msg);
        ret = -GBS_ERROR_DB;
    }

    mbsfree(sql);
    return ret;
}

int gbs_db_insert_publisher(sqlite3 *db, gbs_publisher_t *pub)
{
    int ret = 0;
    char *msg = NULL;
    mbs_t sql = NULL;

    mbscatfmt(&sql, "INSERT INTO gbs_publisher(publisher, website, description) VALUES ('%s', '%s', '%s')", pub->publisher, pub->website, pub->description);
    gbs_debug("%s\n", sql);
    if (sqlite3_exec(db, sql, NULL, NULL, &msg) != SQLITE_OK) {
        gbs_error("sqlite3_exec: %s failed, msg %s\n", sql, msg);
        sqlite3_free(msg);
        ret = -GBS_ERROR_DB;
    }

    mbsfree(sql);
    return ret;
}

int gbs_db_insert_genre(sqlite3 *db, gbs_genre_t *gen)
{
    int ret = 0;
    char *msg = NULL;
    mbs_t sql = NULL;

    mbscatfmt(&sql, "INSERT INTO gbs_genre(genre, subgenre, aliases) VALUES ('%s', '%s', '%s')", 
        gen->genre, gen->subgenre, gen->aliases);
    gbs_debug("%s\n", sql);
    if (sqlite3_exec(db, sql, NULL, NULL, &msg) != SQLITE_OK) {
        gbs_error("sqlite3_exec: %s failed, msg %s\n", sql, msg);
        sqlite3_free(msg);
        ret = -GBS_ERROR_DB;
    }

    mbsfree(sql);
    return ret;
}

int gbs_db_insert_book(sqlite3 *db, gbs_book_t *book)
{
    int ret = 0;
    char *msg = NULL;
    mbs_t sql = NULL;

    mbscatfmt(&sql,
        "INSERT INTO gbs_book(md5, title, subtitle, isbn, format, genre, subgenre, language, date, version, series, publisher, customs, path, contents, introduction, authors, keywords, urls, pages, size, scaned, years, popular, price, quality, doi, libgenid, repository, ctime, mtime)"
        "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%d', '%d', '%d', '%d', '%d', '%.4f', '%d', '%s', '%s', '%s', '%ld', '%ld')",
        book->md5, book->title, book->subtitle, book->isbn, book->format, book->genre, book->subgenre, book->language, book->date,
        book->version, book->series, book->publisher, book->customs, book->path, book->contents, book->introduction, book->authors,
        book->keywords, book->urls, book->pages, book->size, book->scaned, book->years, book->popular, book->price, book->quality,
        book->doi, book->libgenid, book->repository, book->ctime, book->mtime);
    gbs_debug("%s\n", sql);
    if (sqlite3_exec(db, sql, NULL, NULL, &msg) != SQLITE_OK) {
        gbs_error("sqlite3_exec: %s failed, msg %s\n", sql, msg);
        sqlite3_free(msg);
        ret = -GBS_ERROR_DB;
    }

    mbsfree(sql);
    return 0;
}

int gbs_db_write(char *filename)
{
    int ret = 0;
    sqlite3 *db = NULL;

    remove(filename);
    if (sqlite3_open(filename, &db) != 0) {
        gbs_error("Error: %s\n", sqlite3_errmsg(db));
        return -GBS_ERROR_DB;
    }

    ret = gbs_db_create(db);
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
