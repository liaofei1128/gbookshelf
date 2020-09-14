#include "libcmd.h"
#include "libstring.h"
#include "private.h"

static int do_create(app_t *app, cmdline_t *cmdline)
{
    int ret = -1;
    char **output = NULL;

    output = app_param_get(app, "create");
    if (!output) {
        fprintf(stderr, "parse database filename failed\n");
        goto out;
    }

    ret = db_create(*output);

out:
    app_param_destroy(output);
    return ret;
}

static int do_insert(app_t *app, cmdline_t *cmdline)
{
    int ret = -1;
    char **output = NULL,
         **dirname = NULL,
         **filename = NULL,
         **filelist = NULL,
         **extensions = NULL;

    output = app_param_get(app, "insert");
    if (!output) {
        fprintf(stderr, "parse database filename failed\n");
        goto out;
    }

    extensions = app_param_get(app, "t");
    if (extensions) {
        extset(*extensions);
    }

    dirname = app_param_get(app, "d");
    if (dirname) {
        ret = rcd_insert_directory(*output, *dirname);
        goto out;
    }

    filename = app_param_get(app, "f");
    if (filename) {
        ret = rcd_insert_file(*output, *filename);
        goto out;
    }

    filelist = app_param_get(app, "l");
    if (filelist) {
        ret = rcd_insert_list(*output, *filelist);
        goto out;
    }

out:
    app_param_destroy(output);
    app_param_destroy(dirname);
    app_param_destroy(filename);
    app_param_destroy(filelist);
    app_param_destroy(extensions);
    return ret;
}

static int do_delete(app_t *app, cmdline_t *cmdline)
{
    int i;
    int ret = -1;
    var_range_t *id = NULL;
    char **input = NULL, **filename = NULL;

    input = app_param_get(app, "i");
    filename = app_param_get(app, "f");
    if (filename) {
        ret = rcd_delete_by_title(*input, *filename);
        goto out;
    }

    while (1) {
        id = app_param_get(app, "x");
        if (!id) {
            break;
        }

        for (i=id->start; i<=id->end; i++) {
            ret = rcd_delete_by_id(*input, i);
            if (ret < 0) {
                goto out;
            }
        }

        app_param_destroy(id);
    }

out:
    app_param_destroy(id);
    app_param_destroy(input);
    app_param_destroy(filename);
    return ret;
}

static int do_list(app_t *app, cmdline_t *cmdline)
{
    int i;
    int ret = -1;
    var_range_t *id = NULL;
    char **input = NULL, **filename = NULL;
    char **keyname = NULL;
    char *displays = NULL;
    char *orderconditions = NULL;

    keyname = app_param_get(app, "p");
    if (keyname) {
        do {
            strappendfmt(&displays, "%s,", *keyname);
            app_param_destroy(keyname);
        } while ((keyname = app_param_get(app, "p")) != NULL);
    }

    if (displays) {
        strtrim(displays, ",");
    } else {
        displays = strdup("id,size,title");
    }

    keyname = app_param_get(app, "m");
    if (keyname) {
        do {
            strappendfmt(&orderconditions, "%s ASC,", *keyname);
            app_param_destroy(keyname);
        } while ((keyname = app_param_get(app, "m")) != NULL);
    }

    keyname = app_param_get(app, "w");
    if (keyname) {
        do {
            strappendfmt(&orderconditions, "%s DESC,", *keyname);
            app_param_destroy(keyname);
        } while ((keyname = app_param_get(app, "w")) != NULL);
    }

    if (orderconditions) {
        strtrim(orderconditions, ",");
    } else {
        orderconditions = strdup("extension ASC, size DESC");
    }

    input = app_param_get(app, "i");
    filename = app_param_get(app, "f");
    if (filename) {
        ret = rcd_list_by_title(*input, *filename, orderconditions, displays);
        app_param_destroy(filename);
        goto out;
    }

    id = app_param_get(app, "x");
    if (id) {
        do {
            for (i=id->start; i<=id->end; i++) {
                ret = rcd_list_by_id(*input, i, orderconditions, displays);
                if (ret < 0) {
                    goto out;
                }
            }

            app_param_destroy(id);
        } while ((id = app_param_get(app, "x")) != NULL);
        goto out;
    }

    ret = rcd_list_by_all(*input, orderconditions, displays);

out:
    free(displays);
    free(orderconditions);
    app_param_destroy(input);
    return ret;
}

static int do_test(app_t *app, cmdline_t *cmdline)
{
    int ret = -1;
    char **input = NULL, **dirname = NULL, **filename = NULL, **filelist = NULL;

    input = app_param_get(app, "i");
    if (input) {
        dirname = app_param_get(app, "d");
        if (dirname) {
            ret = rcd_test_directory(*input, *dirname);
            app_param_destroy(dirname);
            app_param_destroy(input);
            goto out;
        }

        filename = app_param_get(app, "f");
        if (filename) {
            ret = rcd_test_file(*input, *filename);
            app_param_destroy(filename);
            app_param_destroy(input);
            goto out;
        }

        filelist = app_param_get(app, "l");
        if (filelist) {
            ret = rcd_test_list(*input, *filelist);
            app_param_destroy(filelist);
            app_param_destroy(input);
            goto out;
        }

        app_param_destroy(input);
        goto out;
    }

    dirname = app_param_get(app, "d");
    if (dirname) {
        ret = rcd_uniform_directory(NULL, *dirname);
        app_param_destroy(dirname);
        goto out;
    }

    filename = app_param_get(app, "f");
    if (filename) {
        ret = rcd_uniform_file(NULL, *filename);
        app_param_destroy(filename);
        goto out;
    }

    filelist = app_param_get(app, "l");
    if (filelist) {
        ret = rcd_uniform_list(NULL, *filelist);
        app_param_destroy(filelist);
        goto out;
    }

out:
    return ret;
}

static int do_modify(app_t *app, cmdline_t *cmdline)
{
    int ret = -1;
    char **input = NULL;
    var_range_t *id = NULL;
    char **keyname = NULL;
    char **value = NULL;

    input = app_param_get(app, "i");
    id = app_param_get(app, "x");
    keyname = app_param_get(app, "k");
    value = app_param_get(app, "v");

    if (!input || !id || !keyname || !value) {
        goto out;
    }

    if (id->start != id->end) {
        goto out;
    }

    ret = rcd_update_by_id(*input, id->start, *keyname, *value);

out:
    app_param_destroy(value);
    app_param_destroy(keyname);
    app_param_destroy(id);
    app_param_destroy(input);
    return ret;
}

static int do_dump_abbr(app_t *app, cmdline_t *cmdline)
{
    dict_dump();

    return 0;
}

int parse_keyname(char *arg, char **keyname)
{
    int i;
    char *keyarray[] = {
        "id", "title", "extension", "path", "orig", "isbn",
        "author", "publisher", "date", "genre", "size", "md5"
    };

    for (i=0; i<sizeof(keyarray) / sizeof(keyarray[0]); i++) {
        if (strcmp(arg, keyarray[i]) == 0) {
            *keyname = keyarray[i];
            return 0;
        }
    }

    fprintf(stderr, "unknown key name! you can input the following key names:\n");
    for (i=0; i<sizeof(keyarray) / sizeof(keyarray[0]); i++) {
        fprintf(stderr, " %s", keyarray[i]);
    }
    fprintf(stderr, "\n");

    return -1;
}

int main(int argc, char *argv[])
{
    int ret;
    app_t *gbsmgr;

    dict_init();
    gbsmgr = app_create("gbsmgr",
            "0.0.1",
            "liaoxf<liaofei1128@gmail.com>",
            "Copyright (C) liaoxf 2011-2012",
            "the util to manage the resources in your gds database.");

    app_add_ptype(gbsmgr, "keyname", sizeof(char *), (ptype_parser_t) parse_keyname, "the key names in the gbs book table");

    app_add_option(gbsmgr, 'i', "input", "string", 0, "the path of input gbs database");
    app_add_option(gbsmgr, 'o', "output", "string", 0, "the path of output gbs database");
    app_add_option(gbsmgr, 'f', "filename", "string", 0, "the name of input resource file");
    app_add_option(gbsmgr, 'l', "filelist", "string", 0, "the name of input list file");
    app_add_option(gbsmgr, 'd', "directory", "string", 0, "the name of directory to scan");
    app_add_option(gbsmgr, 'x', "id", "range", 1, "the id of resource");
    app_add_option(gbsmgr, 'm', "ascend", "keyname", 1, "sort the result in ascending by these key names");
    app_add_option(gbsmgr, 'w', "descend", "keyname", 1, "sort the result in descending by these key names");
    app_add_option(gbsmgr, 'p', "display", "keyname", 1, "only display these key names");
    app_add_option(gbsmgr, 'k', "keyname", "keyname", 0, "the key name to update");
    app_add_option(gbsmgr, 'v', "value", "string", 0, "the value to update");
    app_add_option(gbsmgr, 't', "extname", "string", 0, "the name of acceptable extensions");

    app_add_option(gbsmgr, 'C', "create", "string", 0, "create one gbs database");
    app_add_option(gbsmgr, 'I', "insert", NULL, 0, "insert one resource into gbs database");
    app_add_option(gbsmgr, 'D', "delete", NULL, 0, "delete one resource from gbs database");
    app_add_option(gbsmgr, 'L', "list", NULL, 0, "list the resources from gbs database");
    app_add_option(gbsmgr, 'T', "test", NULL, 0, "test the resources if in the gbs database");
    app_add_option(gbsmgr, 'M', "modify", NULL, 0, "update the key value of resource in database");
    app_add_option(gbsmgr, 'P', "abbr", NULL, 0, "dump the whole abbreviations we know, you can write your own abbreviations in dict.txt");

    app_add_cmdline(gbsmgr, "create", do_create, "create one gbs database");
    app_add_cmdline(gbsmgr, "insert,filename", do_insert, "insert the resource by filename into gbs database");
    app_add_cmdline(gbsmgr, 'I', "od[t]", do_insert, "insert the resource by directory into gbs database");
    app_add_cmdline(gbsmgr, 'I', "ol[t]", do_insert, "insert the resource in list into gbs database");
    app_add_cmdline(gbsmgr, 'D', "ix", do_delete, "delete the resource by id from gbs database");
    app_add_cmdline(gbsmgr, 'D', "if", do_delete, "delete the resource by filename from gbs database");
    app_add_cmdline(gbsmgr, 'L', "i[wmp]", do_list, "list the resource from gbs database");
    app_add_cmdline(gbsmgr, 'L', "ix[wmp]", do_list, "list the resource by id from gbs database");
    app_add_cmdline(gbsmgr, 'L', "if[wmp]", do_list, "list the resource by filename from gbs database");
    app_add_cmdline(gbsmgr, 'T', "i[fdl]", do_test, "test the resources if in the gbs database");
    app_add_cmdline(gbsmgr, 'T', "[fdl]", do_test, "test the uniform filenames of resources");
    app_add_cmdline(gbsmgr, 'M', "ixkv", do_modify, "modify the resource by id with key to value");
    app_add_cmdline(gbsmgr, 'P', NULL, do_dump_abbr, "dump the whole abbreviations we know");

    ret = app_run(gbsmgr, argc, argv);

    app_destroy(gbsmgr);
    dict_fini();
    return ret;
}
