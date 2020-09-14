#include "gbookshelf.h"

GtkTreeModel *gbs_book_create_model(void)
{
    gbs_book_t *book;
    GtkTreeIter bookIter;
    GtkListStore *liststore;

    liststore = gtk_list_store_new(GBS_BOOK_COLUMN_MAX, G_TYPE_BOOLEAN,
                                                   /**< scaned? */
        GDK_TYPE_PIXBUF,                             /**< format */
        G_TYPE_STRING,                             /**< title */
        G_TYPE_STRING,                             /**< first author */
        G_TYPE_STRING,                             /**< publisher */
        G_TYPE_STRING,                             /**< version */
        G_TYPE_STRING                              /**< language */
        );

    list_for_each_entry(book, &g_book_list, node) {
        gtk_list_store_append(liststore, &bookIter);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_SCANED,
            book->scaned, -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_FORMAT,
            gbs_format_get_pixbuf(book->format), -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_TITLE,
            book->title, -1);
        if (book->nauthor) {
            gtk_list_store_set(liststore, &bookIter,
                GBS_BOOK_COLUMN_AUTHOR, book->authors[0], -1);
        }
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_PUBLISHER,
            book->publisher, -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_VERSION,
            book->version, -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_LANGUAGE,
            book->language, -1);
    }

    return GTK_TREE_MODEL(liststore);
}

void gbs_book_update_model_first(GtkListStore * liststore, int page_size)
{
    int num;
    gbs_book_t *book;
    GtkTreeIter bookIter;

    num = 0;
    g_cur_page_first_pos =
        list_entry(&g_book_list.next, gbs_book_t, node);
    list_for_each_entry(book, &g_book_list, node) {
        if (page_size != -1 && num == page_size) {
            g_cur_page_last_pos = book;
            break;
        }
        gtk_list_store_append(liststore, &bookIter);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_SCANED,
            book->scaned, -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_FORMAT,
            gbs_format_get_pixbuf(book->format), -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_TITLE,
            book->title, -1);
        if (book->nauthor) {
            gtk_list_store_set(liststore, &bookIter,
                GBS_BOOK_COLUMN_AUTHOR, book->authors[0], -1);
        }
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_PUBLISHER,
            book->publisher, -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_VERSION,
            book->version, -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_LANGUAGE,
            book->language, -1);
        num++;
    }

    return;
}

void gbs_book_update_model_next(GtkListStore * liststore, int page_size)
{
    int num;
    gbs_book_t *book;
    struct list_head *start;
    GtkTreeIter bookIter;

    num = 0;
    start =
        g_cur_page_last_pos ? &g_cur_page_last_pos->node : &g_book_list;
    list_for_each_entry(book, start, node) {
        if (list_is_last(&book->node, &g_book_list))
            break;

        if (page_size != -1 && num == page_size) {
            g_cur_page_first_pos = g_cur_page_last_pos;
            g_cur_page_last_pos = book;
            break;
        }

        gtk_list_store_append(liststore, &bookIter);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_SCANED,
            book->scaned, -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_FORMAT,
            gbs_format_get_pixbuf(book->format), -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_TITLE,
            book->title, -1);
        if (book->nauthor) {
            gtk_list_store_set(liststore, &bookIter,
                GBS_BOOK_COLUMN_AUTHOR, book->authors[0], -1);
        }
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_PUBLISHER,
            book->publisher, -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_VERSION,
            book->version, -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_LANGUAGE,
            book->language, -1);
        num++;
    }

    return;
}

void gbs_book_update_model_prev(GtkListStore * liststore, int page_size)
{
    int num;
    gbs_book_t *book;
    struct list_head *start;
    GtkTreeIter bookIter;

    num = 0;
    start =
        g_cur_page_first_pos ? &g_cur_page_first_pos->node : &g_book_list;
    list_for_each_entry_reverse(book, start, node) {
        if (list_is_first(&book->node, &g_book_list))
            break;

        if (page_size != -1 && num == page_size) {
            g_cur_page_last_pos = g_cur_page_first_pos;
            g_cur_page_first_pos = book;
            break;
        }

        gtk_list_store_prepend(liststore, &bookIter);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_SCANED,
            book->scaned, -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_FORMAT,
            gbs_format_get_pixbuf(book->format), -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_TITLE,
            book->title, -1);
        if (book->nauthor) {
            gtk_list_store_set(liststore, &bookIter,
                GBS_BOOK_COLUMN_AUTHOR, book->authors[0], -1);
        }
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_PUBLISHER,
            book->publisher, -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_VERSION,
            book->version, -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_LANGUAGE,
            book->language, -1);
        num++;
    }

    return;
}

void gbs_book_update_model_last(GtkListStore * liststore, int page_size)
{
    int num;
    gbs_book_t *book;
    GtkTreeIter bookIter;

    num = 0;
    g_cur_page_last_pos =
        list_entry(&g_book_list.prev, gbs_book_t, node);
    list_for_each_entry_reverse(book, &g_book_list, node) {
        if (list_is_first(&book->node, &g_book_list))
            break;

        if (page_size != -1 && num == page_size) {
            g_cur_page_first_pos = book;
            break;
        }

        gtk_list_store_prepend(liststore, &bookIter);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_SCANED,
            book->scaned, -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_FORMAT,
            gbs_format_get_pixbuf(book->format), -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_TITLE,
            book->title, -1);
        if (book->nauthor) {
            gtk_list_store_set(liststore, &bookIter,
                GBS_BOOK_COLUMN_AUTHOR, book->authors[0], -1);
        }
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_PUBLISHER,
            book->publisher, -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_VERSION,
            book->version, -1);
        gtk_list_store_set(liststore, &bookIter, GBS_BOOK_COLUMN_LANGUAGE,
            book->language, -1);
        num++;
    }

    return;
}


void gbs_gtk_book_add_browse(GtkWidget * widget, gpointer data)
{
    int ret;
    GtkWidget *dialog;

    dialog = gtk_file_chooser_dialog_new("Choose ebook file", GTK_WINDOW(GTK_WINDOW(gAddBook.AddBookWindow)), 
        GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
    if (gAddBookOldDir == NULL) {
        gchar *pwd = g_get_current_dir();
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), pwd);
        g_free(pwd);
    } else {
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), gAddBookOldDir);
    }
run:
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        gchar *filename;
        char *dir;
        char *file;
        char *suffix;

        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        ret = parse_realname(filename, &dir, &file, &suffix);
        if (ret < 0) {
            gbs_message_dialog (GTK_MESSAGE_WARNING, "Warning, invalid filename!", "gBookshelf can't resolve neither <i>path</i> nor <i>suffix</i>:\n<b>%s</b>", filename);
            g_free(filename);
            goto run;
        }

        gbs_debug("dir:[%s],file:[%s],suffix:[%s]\n", dir, file, suffix);
        gtk_entry_set_text(GTK_ENTRY(gAddBook.AddBookTitleEntry), file);
        //gtk_widget_set_sensitive(gAddBook.AddBookTitleEntry, FALSE);
        gtk_entry_set_text(GTK_ENTRY(gAddBook.AddBookPathEntry), filename);
        //gtk_widget_set_sensitive(gAddBook.AddBookPathEntry, FALSE);
        ret = gbs_format_combox_insert_new(GTK_COMBO_BOX(gAddBook.AddBookFormatComboBoxEntry), suffix);
        //gtk_widget_set_sensitive(gAddBook.AddBookFormatComboBoxEntry, FALSE);
        g_modify_flag = ret;

        if(g_file_test(filename, G_FILE_TEST_EXISTS)) {
            gchar *file_md5_str;
            gchar *file_length_str;
            gsize file_length;
            gchar *file_content;
            GError *error = NULL;
            g_file_get_contents(filename, &file_content, &file_length, &error);
            if(error == NULL) {
                file_length_str = g_strdup_printf("%d", file_length);
                file_md5_str = g_compute_checksum_for_data(G_CHECKSUM_MD5, (guchar *)file_content, file_length);
                gtk_entry_set_text(GTK_ENTRY(gAddBook.AddBookSizeEntry), file_length_str);
                //gtk_widget_set_sensitive(gAddBook.AddBookSizeEntry, FALSE);
                gtk_entry_set_text(GTK_ENTRY(gAddBook.AddBookMd5Entry), file_md5_str);
                //gtk_widget_set_sensitive(gAddBook.AddBookMd5Entry, FALSE);
                g_free(file_length_str);
                g_free(file_md5_str);
                g_free(file_content);
            }else {
                gbs_message_dialog (GTK_MESSAGE_INFO, "Sorry, gBookshelf can't find this book!",
                    "and so gBookshelf can't figure out the <i>size</i> and <i>md5</i> yet:\n<b>%s</b>", error->message);
                g_clear_error(&error);
            }
        }

        if (gAddBookOldDir)
            g_free(gAddBookOldDir);
        gAddBookOldDir = dir;

        g_free(filename);
        free(file);
        free(suffix);
    }

    gtk_widget_destroy(dialog);
}

void gbs_gtk_book_add_reset(GtkWidget * widget, gpointer data)
{
    gtk_entry_set_text(GTK_ENTRY(gAddBook.AddBookTitleEntry), "");
    gtk_entry_set_text(GTK_ENTRY(gAddBook.AddBookSubtitleEntry), "");
    gtk_entry_set_text(GTK_ENTRY(gAddBook.AddBookSizeEntry), "");
    gtk_entry_set_text(GTK_ENTRY(gAddBook.AddBookMd5Entry), "");
    gtk_entry_set_text(GTK_ENTRY(gAddBook.AddBookPathEntry), "");

    gtk_entry_set_text(GTK_ENTRY(gAddBook.AddBookPriceEntry), "");
    gtk_entry_set_text(GTK_ENTRY(gAddBook.AddBookDateEntry), "");
    gtk_entry_set_text(GTK_ENTRY(gAddBook.AddBookIsbnEntry), "");
    gtk_entry_set_text(GTK_ENTRY(gAddBook.AddBookSeriesEntry), "");

    gtk_combo_box_set_active(GTK_COMBO_BOX(gAddBook.AddBookGenreComboBoxEntry), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(gAddBook.AddBookSubgenreComboBoxEntry), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(gAddBook.AddBookPublisherComboBoxEntry), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(gAddBook.AddBookFormatComboBoxEntry), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(gAddBook.AddBookVersionComboBoxEntry), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(gAddBook.AddBookLanguageComboBoxEntry), -1);
}

void gbs_gtk_book_add_cancel(GtkWidget * widget, gpointer data)
{
    gtk_widget_destroy(gAddBook.AddBookWindow);
}

void gbs_gtk_book_add_expander(GtkWidget * widget, gpointer data)
{
    int x, y;

    gtk_window_get_position(GTK_WINDOW(gAddBook.AddBookWindow), &x, &y);
    gbs_debug("current position x=%d, y=%d\n", x, y);
    gtk_window_move(GTK_WINDOW(gAddBook.AddBookWindow), x, 20);
}

/* add/del/clr author */

void gbs_gtk_book_add_author_add(GtkWidget * widget, gpointer data)
{
    GtkListStore *store;
    GtkTreeIter iter;
    GtkTreeViewColumn *col;
    GtkTreePath *path;

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(gAddBook.AddBookAuthorsTreeview)));

    gtk_list_store_append(store, &iter);
    col = gtk_tree_view_get_column(GTK_TREE_VIEW(gAddBook.AddBookAuthorsTreeview), 0);
    path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &iter);
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(gAddBook.AddBookAuthorsTreeview), path, col, TRUE);
}

void gbs_gtk_book_add_author_add_motion_notify(GtkWidget * widget, gpointer data)
{
    gtk_widget_set_tooltip_markup(gAddBook.AddBookAuthorsAddWidgetEventBox, "click me to add an <b>author</b>");
}

void gbs_gtk_book_add_author_del(GtkWidget * widget, gpointer data)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;

    GtkTreeViewColumn *col;
    GtkTreePath *path;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(gAddBook.AddBookAuthorsTreeview));
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(gAddBook.AddBookAuthorsTreeview));
    col = gtk_tree_view_get_column(GTK_TREE_VIEW(gAddBook.AddBookAuthorsTreeview), 0);

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        path = gtk_tree_model_get_path(model, &iter);
        if(gtk_tree_path_prev(path)){
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(gAddBook.AddBookAuthorsTreeview), path, col, TRUE);
        }
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    }
}

void gbs_gtk_book_add_author_del_motion_notify(GtkWidget * widget, gpointer data)
{
    gtk_widget_set_tooltip_markup(gAddBook.AddBookAuthorsDelWidgetEventBox, "click me to delete an <b>author</b>");
}

void gbs_gtk_book_add_author_clr(GtkWidget * widget, gpointer data)
{
    GtkTreeModel *model;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(gAddBook.AddBookAuthorsTreeview));
    gtk_list_store_clear(GTK_LIST_STORE(model));
}

void gbs_gtk_book_add_author_clr_motion_notify(GtkWidget * widget, gpointer data)
{
    gtk_widget_set_tooltip_markup(gAddBook.AddBookAuthorsClrWidgetEventBox, "click me to delete all <b>authors</b>");
}

/* add/del/clr keyword */

void gbs_gtk_book_add_keyword_add(GtkWidget * widget, gpointer data)
{
    GtkListStore *store;
    GtkTreeIter iter;
    GtkTreeViewColumn *col;
    GtkTreePath *path;

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(gAddBook.AddBookKeywordsTreeview)));

    gtk_list_store_append(store, &iter);
    col = gtk_tree_view_get_column(GTK_TREE_VIEW(gAddBook.AddBookKeywordsTreeview), 0);
    path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &iter);
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(gAddBook.AddBookKeywordsTreeview), path, col, TRUE);
}

void gbs_gtk_book_add_keyword_add_motion_notify(GtkWidget * widget, gpointer data)
{
    gtk_widget_set_tooltip_markup(gAddBook.AddBookKeywordsAddWidgetEventBox, "click me to add an <b>keyword</b>");
}

void gbs_gtk_book_add_keyword_del(GtkWidget * widget, gpointer data)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;

    GtkTreeViewColumn *col;
    GtkTreePath *path;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(gAddBook.AddBookKeywordsTreeview));
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(gAddBook.AddBookKeywordsTreeview));
    col = gtk_tree_view_get_column(GTK_TREE_VIEW(gAddBook.AddBookKeywordsTreeview), 0);

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        path = gtk_tree_model_get_path(model, &iter);
        if(gtk_tree_path_prev(path)){
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(gAddBook.AddBookKeywordsTreeview), path, col, TRUE);
        }
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    }
}

void gbs_gtk_book_add_keyword_del_motion_notify(GtkWidget * widget, gpointer data)
{
    gtk_widget_set_tooltip_markup(gAddBook.AddBookKeywordsDelWidgetEventBox, "click me to delete an <b>keyword</b>");
}

void gbs_gtk_book_add_keyword_clr(GtkWidget * widget, gpointer data)
{
    GtkTreeModel *model;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(gAddBook.AddBookKeywordsTreeview));
    gtk_list_store_clear(GTK_LIST_STORE(model));
}

void gbs_gtk_book_add_keyword_clr_motion_notify(GtkWidget * widget, gpointer data)
{
    gtk_widget_set_tooltip_markup(gAddBook.AddBookKeywordsClrWidgetEventBox, "click me to delete all <b>keywords</b>");
}

/* add/del/clr website */

void gbs_gtk_book_add_website_add(GtkWidget * widget, gpointer data)
{
    GtkListStore *store;
    GtkTreeIter iter;
    GtkTreeViewColumn *col;
    GtkTreePath *path;

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(gAddBook.AddBookWebsiteTreeview)));

    gtk_list_store_append(store, &iter);
    col = gtk_tree_view_get_column(GTK_TREE_VIEW(gAddBook.AddBookWebsiteTreeview), 0);
    path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &iter);
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(gAddBook.AddBookWebsiteTreeview), path, col, TRUE);
}

void gbs_gtk_book_add_website_add_motion_notify(GtkWidget * widget, gpointer data)
{
    gtk_widget_set_tooltip_markup(gAddBook.AddBookWebsiteAddWidgetEventBox, "click me to add an <b>website</b>");
}

void gbs_gtk_book_add_website_del(GtkWidget * widget, gpointer data)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;

    GtkTreeViewColumn *col;
    GtkTreePath *path;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(gAddBook.AddBookWebsiteTreeview));
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(gAddBook.AddBookWebsiteTreeview));
    col = gtk_tree_view_get_column(GTK_TREE_VIEW(gAddBook.AddBookWebsiteTreeview), 0);

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        path = gtk_tree_model_get_path(model, &iter);
        if(gtk_tree_path_prev(path)){
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(gAddBook.AddBookWebsiteTreeview), path, col, TRUE);
        }
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    }
}

void gbs_gtk_book_add_website_del_motion_notify(GtkWidget * widget, gpointer data)
{
    gtk_widget_set_tooltip_markup(gAddBook.AddBookWebsiteDelWidgetEventBox, "click me to delete an <b>website</b>");
}

void gbs_gtk_book_add_website_clr(GtkWidget * widget, gpointer data)
{
    GtkTreeModel *model;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(gAddBook.AddBookWebsiteTreeview));
    gtk_list_store_clear(GTK_LIST_STORE(model));
}

void gbs_gtk_book_add_website_clr_motion_notify(GtkWidget * widget, gpointer data)
{
    gtk_widget_set_tooltip_markup(gAddBook.AddBookWebsiteClrWidgetEventBox, "click me to delete all <b>websites</b>");
}

/* add/del/clr custom */

void gbs_gtk_book_add_custom_add(GtkWidget * widget, gpointer data)
{
    GtkListStore *store;
    GtkTreeIter iter;
    GtkTreeViewColumn *col;
    GtkTreePath *path;

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(gAddBook.AddBookCustomTreeview)));

    gtk_list_store_append(store, &iter);
    col = gtk_tree_view_get_column(GTK_TREE_VIEW(gAddBook.AddBookCustomTreeview), 0);
    path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &iter);
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(gAddBook.AddBookCustomTreeview), path, col, TRUE);
}

void gbs_gtk_book_add_custom_add_motion_notify(GtkWidget * widget, gpointer data)
{
    gtk_widget_set_tooltip_markup(gAddBook.AddBookCustomAddWidgetEventBox, "click me to add an <b>custom</b>");
}

void gbs_gtk_book_add_custom_del(GtkWidget * widget, gpointer data)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;

    GtkTreeViewColumn *col;
    GtkTreePath *path;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(gAddBook.AddBookCustomTreeview));
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(gAddBook.AddBookCustomTreeview));
    col = gtk_tree_view_get_column(GTK_TREE_VIEW(gAddBook.AddBookCustomTreeview), 0);

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        path = gtk_tree_model_get_path(model, &iter);
        if(gtk_tree_path_prev(path)){
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(gAddBook.AddBookCustomTreeview), path, col, TRUE);
        }
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    }
}

void gbs_gtk_book_add_custom_del_motion_notify(GtkWidget * widget, gpointer data)
{
    gtk_widget_set_tooltip_markup(gAddBook.AddBookCustomDelWidgetEventBox, "click me to delete an <b>custom</b>");
}

void gbs_gtk_book_add_custom_clr(GtkWidget * widget, gpointer data)
{
    GtkTreeModel *model;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(gAddBook.AddBookCustomTreeview));
    gtk_list_store_clear(GTK_LIST_STORE(model));
}

void gbs_gtk_book_add_custom_clr_motion_notify(GtkWidget * widget, gpointer data)
{
    gtk_widget_set_tooltip_markup(gAddBook.AddBookCustomClrWidgetEventBox, "click me to delete all <b>customs</b>");
}


GtkTreeModel *gbs_gtk_booklist_update_model_first_page(void)
{
    GtkTreeModel *model;
    GtkListStore *liststore;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(g_book_treeview));
    liststore = GTK_LIST_STORE(model);

    gtk_list_store_clear(liststore);
    gbs_book_update_model_first(liststore, g_book_pagesize);
    return GTK_TREE_MODEL(liststore);
}

GtkTreeModel *gbs_gtk_booklist_update_model_next_page(void)
{
    GtkTreeModel *model;
    GtkListStore *liststore;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(g_book_treeview));
    liststore = GTK_LIST_STORE(model);

    gtk_list_store_clear(liststore);
    gbs_book_update_model_next(liststore, g_book_pagesize);
    return GTK_TREE_MODEL(liststore);
}

GtkTreeModel *gbs_gtk_booklist_update_model_prev_page(void)
{
    GtkTreeModel *model;
    GtkListStore *liststore;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(g_book_treeview));
    liststore = GTK_LIST_STORE(model);

    gtk_list_store_clear(liststore);
    gbs_book_update_model_prev(liststore, g_book_pagesize);
    return GTK_TREE_MODEL(liststore);
}

GtkTreeModel *gbs_gtk_booklist_update_model_last_page(void)
{
    GtkTreeModel *model;
    GtkListStore *liststore;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(g_book_treeview));
    liststore = GTK_LIST_STORE(model);

    gtk_list_store_clear(liststore);
    gbs_book_update_model_last(liststore, g_book_pagesize);
    return GTK_TREE_MODEL(liststore);
}

void gbs_gtk_book_add_insert_authors(GtkTreeModel *model, gbs_book_t *book)
{
    GtkTreeIter iter;
    gboolean valid;

    valid = gtk_tree_model_get_iter_first(model, &iter);
    while(valid){
        gchar *author;
        gtk_tree_model_get(model, &iter, 0, &author, -1);
        gbs_book_add_author(book, author);
        valid = gtk_tree_model_iter_next(model, &iter);
        g_free(author);
    }
}

void gbs_gtk_book_add_insert_keywords(GtkTreeModel *model, gbs_book_t *book)
{
    GtkTreeIter iter;
    gboolean valid;

    valid = gtk_tree_model_get_iter_first(model, &iter);
    while(valid){
        gchar *keyword;
        gtk_tree_model_get(model, &iter, 0, &keyword, -1);
        gbs_book_add_keyword(book, keyword);
        valid = gtk_tree_model_iter_next(model, &iter);
        g_free(keyword);
    }
}

void gbs_gtk_book_add_insert_urls(GtkTreeModel *model, gbs_book_t *book)
{
    GtkTreeIter iter;
    gboolean valid;

    valid = gtk_tree_model_get_iter_first(model, &iter);
    while(valid){
        gchar *url;
        gtk_tree_model_get(model, &iter, 0, &url, -1);
        gbs_book_add_url(book, url);
        valid = gtk_tree_model_iter_next(model, &iter);
        g_free(url);
    }
}

void gbs_gtk_book_add_insert_customs(GtkTreeModel *model, gbs_book_t *book)
{
    GtkTreeIter iter;
    gboolean valid;

    valid = gtk_tree_model_get_iter_first(model, &iter);
    while(valid){
        gchar *key, *value;
        gtk_tree_model_get(model, &iter, 0, &key, 1, &value, -1);
        gbs_book_add_custom(book, key, value);
        valid = gtk_tree_model_iter_next(model, &iter);
        g_free(key);
        g_free(value);
    }
}

int gbs_gtk_book_add_insert(GtkWidget * widget, gpointer data)
{
    int ret;
    gbs_book_t *nbook;

    ret = gbs_book_new(&nbook);
    if (ret < 0)
        return ret;

    /* mandatory */
    char *title = (char *)gtk_entry_get_text(GTK_ENTRY(gAddBook.AddBookTitleEntry));
    if (title[0] == '\0') {
        gbs_message_dialog (GTK_MESSAGE_WARNING, "Warning, the entries label with asterisk are mandatory!", "Please input the Title entry");
        return -GBS_ERROR_INVAL;
    }

    char *md5 = (char *)gtk_entry_get_text(GTK_ENTRY(gAddBook.AddBookMd5Entry));
    if (md5[0] == '\0') {
        gbs_message_dialog (GTK_MESSAGE_WARNING, "Warning, the entries label with asterisk are mandatory!", "Please input the MD5 entry");
        return -GBS_ERROR_INVAL;
    }

    char *path = (char *)gtk_entry_get_text(GTK_ENTRY(gAddBook.AddBookPathEntry));
    char *size_str = (char *)gtk_entry_get_text(GTK_ENTRY(gAddBook.AddBookSizeEntry));
    char *price_str = (char *)gtk_entry_get_text(GTK_ENTRY(gAddBook.AddBookPriceEntry));
    char *page_str = (char *)gtk_entry_get_text(GTK_ENTRY(gAddBook.AddBookPageEntry));
    int scaned = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gAddBook.AddBookScanedToggle));
    char *subtitle = (char *)gtk_entry_get_text(GTK_ENTRY(gAddBook.AddBookSubtitleEntry));
    char *isbn = (char *)gtk_entry_get_text(GTK_ENTRY(gAddBook.AddBookIsbnEntry));
    char *genre = (char *)gtk_combo_box_get_active_text(GTK_COMBO_BOX(gAddBook.AddBookGenreComboBoxEntry));
    char *subgenre = (char *)gtk_combo_box_get_active_text(GTK_COMBO_BOX(gAddBook.AddBookSubgenreComboBoxEntry));
    char *format = (char *)gtk_combo_box_get_active_text(GTK_COMBO_BOX(gAddBook.AddBookFormatComboBoxEntry));
    char *version = (char *)gtk_combo_box_get_active_text(GTK_COMBO_BOX(gAddBook.AddBookVersionComboBoxEntry));
    char *language = (char *)gtk_combo_box_get_active_text(GTK_COMBO_BOX(gAddBook.AddBookLanguageComboBoxEntry));
    char *publisher = (char *)gtk_combo_box_get_active_text(GTK_COMBO_BOX(gAddBook.AddBookPublisherComboBoxEntry));
    char *date = (char *)gtk_entry_get_text(GTK_ENTRY(gAddBook.AddBookDateEntry));
    int popular = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gAddBook.AddBookPopularSpinButton));
    char *series = (char *)gtk_entry_get_text(GTK_ENTRY(gAddBook.AddBookSeriesEntry));

    gbs_book_set_title(nbook, title);
    gbs_book_set_md5(nbook, md5);
    gbs_book_set_path(nbook, path);
    gbs_book_set_size(nbook, atoi(size_str));
    gbs_book_set_pages(nbook, atoi(page_str));
    gbs_book_set_price(nbook, atof(price_str));
    gbs_book_set_scaned(nbook, scaned);
    gbs_book_set_subtitle(nbook, subtitle);
    gbs_book_set_isbn(nbook, isbn);
    gbs_book_set_genre(nbook, genre);
    gbs_book_set_subgenre(nbook, subgenre);
    gbs_book_set_format(nbook, format);
    gbs_book_set_version(nbook, version);
    gbs_book_set_language(nbook, language);
    gbs_book_set_publisher(nbook, publisher);
    gbs_book_set_date(nbook, date);
    gbs_book_set_popular(nbook, popular);
    gbs_book_set_series(nbook, series);

    gbs_gtk_book_add_insert_authors(gtk_tree_view_get_model(GTK_TREE_VIEW(gAddBook.AddBookAuthorsTreeview)), nbook);
    gbs_gtk_book_add_insert_keywords(gtk_tree_view_get_model(GTK_TREE_VIEW(gAddBook.AddBookKeywordsTreeview)), nbook);
    gbs_gtk_book_add_insert_urls(gtk_tree_view_get_model(GTK_TREE_VIEW(gAddBook.AddBookWebsiteTreeview)), nbook);
    gbs_gtk_book_add_insert_customs(gtk_tree_view_get_model(GTK_TREE_VIEW(gAddBook.AddBookCustomTreeview)), nbook);

    GtkTextIter start, end;
    GtkTextBuffer *intro_buffer;
    GtkTextBuffer *contents_buffer;

    intro_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gAddBook.AddBookIntroductionTextview));
    gtk_text_buffer_get_bounds(intro_buffer, &start, &end);
    char *intro = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(intro_buffer), &start, &end, FALSE);
    gbs_book_set_introduction(nbook, intro);

    contents_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gAddBook.AddBookIntroductionTextview));
    gtk_text_buffer_get_bounds(contents_buffer, &start, &end);
    char *contents = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(contents_buffer), &start, &end, FALSE);
    gbs_book_set_contents(nbook, contents);

    g_free(genre);
    g_free(subgenre);
    g_free(format);
    g_free(version);
    g_free(language);
    g_free(publisher);
    g_free(intro);
    g_free(contents);

    gbs_book_console_show(nbook, NULL);
    ret = gbs_book_table_insert(nbook);
    if (ret < 0) {
        gbs_message_dialog (GTK_MESSAGE_ERROR, "Error, insert ebook failed!", "gbs_book_table_insert hitted error: %s.", gbs_err(ret));
        return ret;
    }

    gbs_gtk_booklist_update_model_first_page();
    g_modify_flag = 1;
    return 0;
}

void gbs_gtk_book_add_quit(GtkWidget * widget, gpointer data)
{
    int ret;
    ret = gbs_gtk_book_add_insert(widget, data);
    if (ret < 0)
        return;

    gtk_widget_destroy(gAddBook.AddBookWindow);
}

void gbs_gtk_book_add_continue(GtkWidget * widget, gpointer data)
{
    int ret;
    ret = gbs_gtk_book_add_insert(widget, data);
    if (ret < 0)
        return;
    gbs_gtk_book_add_reset(NULL, NULL);
}

static void gbs_gtk_book_add_window_init(void)
{
    gAddBook.AddBookWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_modal(GTK_WINDOW(gAddBook.AddBookWindow), TRUE);
    gtk_window_set_title(GTK_WINDOW(gAddBook.AddBookWindow), "Add book");
    gtk_window_set_policy(GTK_WINDOW(gAddBook.AddBookWindow), FALSE, FALSE, TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(gAddBook.AddBookWindow), GTK_WINDOW(g_window));
    gtk_window_set_position(GTK_WINDOW(gAddBook.AddBookWindow), GTK_WIN_POS_CENTER_ON_PARENT);

    gAddBook.AddBookMainVBox = gtk_vbox_new(FALSE, 2);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookWindow), gAddBook.AddBookMainVBox);

    gAddBook.AddBookEditMainVBox = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookMainVBox), gAddBook.AddBookEditMainVBox, FALSE, FALSE, 0);

    gAddBook.AddBookEditTopHBox = gtk_hbox_new(FALSE, 4);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditMainVBox), gAddBook.AddBookEditTopHBox, FALSE, FALSE, 0);
    //gAddBook.AddBookEditTopPreview = gtk_image_new();
    gAddBook.AddBookEditTopPreview = gtk_image_new_from_file("preview.png");
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditTopHBox), gAddBook.AddBookEditTopPreview, TRUE, TRUE, 0);

    gAddBook.AddBookEditTopRightTable = gtk_table_new(8, 6, FALSE);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditTopHBox), gAddBook.AddBookEditTopRightTable, TRUE, TRUE, 0);
    gtk_table_set_row_spacings(GTK_TABLE(gAddBook.AddBookEditTopRightTable), 4);
    gtk_table_set_col_spacings(GTK_TABLE(gAddBook.AddBookEditTopRightTable), 2);

    gAddBook.AddBookTitleLabel = gtk_label_new("Title*");
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookTitleLabel, 0,1,0,1);
    gAddBook.AddBookTitleEntry = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookTitleEntry, 1,4,0,1);

    gAddBook.AddBookTitleButton = gtk_button_new_with_mnemonic("_Browse");
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookTitleButton, 4,5,0,1);

    gAddBook.AddBookScanedToggle = gtk_toggle_button_new_with_mnemonic("_Share it!");
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookScanedToggle, 5,6,0,1);

    gAddBook.AddBookPathLabel = gtk_label_new("Path");
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookPathLabel, 0,1,1,2);
    gAddBook.AddBookPathEntry = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookPathEntry, 1,4,1,2);

    gAddBook.AddBookPageLabel = gtk_label_new("Pages");
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookPageLabel, 4,5,1,2);
    gAddBook.AddBookPageEntry = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookPageEntry, 5,6,1,2);

    gAddBook.AddBookEditTopRightSep = gtk_hseparator_new();
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookEditTopRightSep, 0,6,2,3);

    gAddBook.AddBookMd5Label = gtk_label_new("MD5*");
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookMd5Label, 0,1,3,4);
    gAddBook.AddBookMd5Entry = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookMd5Entry, 1,4,3,4);

    gAddBook.AddBookSizeLabel = gtk_label_new("Size");
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookSizeLabel, 4,5,3,4);
    gAddBook.AddBookSizeEntry = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookSizeEntry, 5,6,3,4);

    gAddBook.AddBookSubtitleLabel = gtk_label_new("Subtitle");
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookSubtitleLabel, 0,1,4,5);
    gAddBook.AddBookSubtitleEntry = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookSubtitleEntry, 1,4,4,5);

    gAddBook.AddBookPriceLabel = gtk_label_new("Price");
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookPriceLabel, 4,5,4,5);
    gAddBook.AddBookPriceEntry = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookPriceEntry, 5,6,4,5);

    gAddBook.AddBookGenreLabel = gtk_label_new("Genre");
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookGenreLabel, 0,1,5,6);
    gAddBook.AddBookGenreComboBoxEntry = gtk_combo_box_new_text();
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookGenreComboBoxEntry, 1,2,5,6);

    gAddBook.AddBookSubgenreLabel = gtk_label_new("Subgenre");
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookSubgenreLabel, 2,3,5,6);
    gAddBook.AddBookSubgenreComboBoxEntry = gtk_combo_box_new_text();
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookSubgenreComboBoxEntry, 3,4,5,6);

    gAddBook.AddBookPopularLabel = gtk_label_new("Popular");
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookPopularLabel, 2,3,6,7);
    gAddBook.AddBookPopularSpinButton = gtk_spin_button_new_with_range(0, 10, 1);
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookPopularSpinButton, 3,4,6,7);

    gAddBook.AddBookIsbnLabel = gtk_label_new("ISBN");
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookIsbnLabel, 0,1,6,7);
    gAddBook.AddBookIsbnEntry = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookIsbnEntry, 1,2,6,7);

    gAddBook.AddBookLanguageLabel = gtk_label_new("Language");
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookLanguageLabel, 4,5,5,6);
    gAddBook.AddBookLanguageComboBoxEntry = gtk_combo_box_new_text();
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookLanguageComboBoxEntry, 5,6,5,6);

    gAddBook.AddBookFormatLabel = gtk_label_new("Format");
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookFormatLabel, 4,5,6,7);
    gAddBook.AddBookFormatComboBoxEntry = gtk_combo_box_new_text();
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookFormatComboBoxEntry, 5,6,6,7);

    gAddBook.AddBookPublisherLabel = gtk_label_new("Publisher");
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookPublisherLabel, 0,1,7,8);
    gAddBook.AddBookPublisherComboBoxEntry = gtk_combo_box_new_text();
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookPublisherComboBoxEntry, 1,2,7,8);

    gAddBook.AddBookDateLabel = gtk_label_new("Date");
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookDateLabel, 2,3,7,8);
    gAddBook.AddBookDateEntry = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookDateEntry, 3,4,7,8);

    gAddBook.AddBookVersionLabel = gtk_label_new("Version");
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookVersionLabel, 4,5,7,8);
    gAddBook.AddBookVersionComboBoxEntry = gtk_combo_box_new_text();
    gtk_table_attach_defaults(GTK_TABLE(gAddBook.AddBookEditTopRightTable), gAddBook.AddBookVersionComboBoxEntry, 5,6,7,8);

    gAddBook.AddBookEditMiddleSep = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditMainVBox), gAddBook.AddBookEditMiddleSep, FALSE, FALSE, 4);

    gAddBook.AddBookEditMiddleHBox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditMainVBox), gAddBook.AddBookEditMiddleHBox, FALSE, FALSE, 0);

    gAddBook.AddBookAuthorsFrame = gtk_frame_new("Authors:");
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditMiddleHBox), gAddBook.AddBookAuthorsFrame, TRUE, TRUE, 0);
    gAddBook.AddBookAuthorsHBox = gtk_hbox_new(FALSE, 4);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookAuthorsFrame), gAddBook.AddBookAuthorsHBox);
    gAddBook.AddBookAuthorsHBoxVBoxLeft = gtk_vbox_new(FALSE, 4);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookAuthorsHBox), gAddBook.AddBookAuthorsHBoxVBoxLeft, FALSE, FALSE, 0);
    gAddBook.AddBookAuthorsAddWidgetEventBox = gtk_event_box_new();
    gtk_widget_set_events(gAddBook.AddBookAuthorsAddWidgetEventBox, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookAuthorsHBoxVBoxLeft), gAddBook.AddBookAuthorsAddWidgetEventBox, FALSE, FALSE, 0);
    gAddBook.AddBookAuthorsAddWidget = gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookAuthorsAddWidgetEventBox), gAddBook.AddBookAuthorsAddWidget);
    gAddBook.AddBookAuthorsDelWidgetEventBox = gtk_event_box_new();
    gtk_widget_set_events(gAddBook.AddBookAuthorsDelWidgetEventBox, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookAuthorsHBoxVBoxLeft), gAddBook.AddBookAuthorsDelWidgetEventBox, FALSE, FALSE, 0);
    gAddBook.AddBookAuthorsDelWidget = gtk_image_new_from_stock(GTK_STOCK_DELETE, GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookAuthorsDelWidgetEventBox), gAddBook.AddBookAuthorsDelWidget);
    gAddBook.AddBookAuthorsClrWidgetEventBox = gtk_event_box_new();
    gtk_widget_set_events(gAddBook.AddBookAuthorsClrWidgetEventBox, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookAuthorsHBoxVBoxLeft), gAddBook.AddBookAuthorsClrWidgetEventBox, FALSE, FALSE, 0);
    gAddBook.AddBookAuthorsClrWidget = gtk_image_new_from_stock(GTK_STOCK_CLEAR, GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookAuthorsClrWidgetEventBox), gAddBook.AddBookAuthorsClrWidget);
    gAddBook.AddBookAuthorsHBoxVBoxRight = gtk_vbox_new(FALSE, 4);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookAuthorsHBox), gAddBook.AddBookAuthorsHBoxVBoxRight, TRUE, TRUE, 0);
    gAddBook.AddBookAuthorsTreeviewScrollWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER(gAddBook.AddBookAuthorsTreeviewScrollWindow), 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(gAddBook.AddBookAuthorsTreeviewScrollWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookAuthorsHBoxVBoxRight), gAddBook.AddBookAuthorsTreeviewScrollWindow, FALSE, FALSE, 0);
    gAddBook.AddBookAuthorsTreeview = gtk_tree_view_new();
    gtk_widget_set_size_request(gAddBook.AddBookAuthorsTreeview, -1, 80);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookAuthorsTreeviewScrollWindow), gAddBook.AddBookAuthorsTreeview);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(gAddBook.AddBookAuthorsTreeview), FALSE);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(gAddBook.AddBookAuthorsTreeview), TRUE);

    gAddBook.AddBookKeywordsFrame = gtk_frame_new("Keywords:");
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditMiddleHBox), gAddBook.AddBookKeywordsFrame, TRUE, TRUE, 0);
    gAddBook.AddBookKeywordsHBox = gtk_hbox_new(FALSE, 4);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookKeywordsFrame), gAddBook.AddBookKeywordsHBox);
    gAddBook.AddBookKeywordsHBoxVBoxLeft = gtk_vbox_new(FALSE, 4);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookKeywordsHBox), gAddBook.AddBookKeywordsHBoxVBoxLeft, FALSE, FALSE, 0);
    gAddBook.AddBookKeywordsAddWidgetEventBox = gtk_event_box_new();
    gtk_widget_set_events(gAddBook.AddBookKeywordsAddWidgetEventBox, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookKeywordsHBoxVBoxLeft), gAddBook.AddBookKeywordsAddWidgetEventBox, FALSE, FALSE, 0);
    gAddBook.AddBookKeywordsAddWidget = gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookKeywordsAddWidgetEventBox), gAddBook.AddBookKeywordsAddWidget);
    gAddBook.AddBookKeywordsDelWidgetEventBox = gtk_event_box_new();
    gtk_widget_set_events(gAddBook.AddBookKeywordsDelWidgetEventBox, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookKeywordsHBoxVBoxLeft), gAddBook.AddBookKeywordsDelWidgetEventBox, FALSE, FALSE, 0);
    gAddBook.AddBookKeywordsDelWidget = gtk_image_new_from_stock(GTK_STOCK_DELETE, GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookKeywordsDelWidgetEventBox), gAddBook.AddBookKeywordsDelWidget);
    gAddBook.AddBookKeywordsClrWidgetEventBox = gtk_event_box_new();
    gtk_widget_set_events(gAddBook.AddBookKeywordsClrWidgetEventBox, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookKeywordsHBoxVBoxLeft), gAddBook.AddBookKeywordsClrWidgetEventBox, FALSE, FALSE, 0);
    gAddBook.AddBookKeywordsClrWidget = gtk_image_new_from_stock(GTK_STOCK_CLEAR, GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookKeywordsClrWidgetEventBox), gAddBook.AddBookKeywordsClrWidget);
    gAddBook.AddBookKeywordsHBoxVBoxRight = gtk_vbox_new(FALSE, 4);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookKeywordsHBox), gAddBook.AddBookKeywordsHBoxVBoxRight, TRUE, TRUE, 0);
    gAddBook.AddBookKeywordsTreeviewScrollWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER(gAddBook.AddBookKeywordsTreeviewScrollWindow), 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(gAddBook.AddBookKeywordsTreeviewScrollWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookKeywordsHBoxVBoxRight), gAddBook.AddBookKeywordsTreeviewScrollWindow, FALSE, FALSE, 0);
    gAddBook.AddBookKeywordsTreeview = gtk_tree_view_new();
    gtk_widget_set_size_request(gAddBook.AddBookKeywordsTreeview, -1, 80);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookKeywordsTreeviewScrollWindow), gAddBook.AddBookKeywordsTreeview);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(gAddBook.AddBookKeywordsTreeview), FALSE);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(gAddBook.AddBookKeywordsTreeview), TRUE);

    gAddBook.AddBookEditMoreExpander = gtk_expander_new_with_mnemonic("_More...");
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditMainVBox), gAddBook.AddBookEditMoreExpander, FALSE, FALSE, 0);

    gAddBook.AddBookEditMoreVbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookEditMoreExpander), gAddBook.AddBookEditMoreVbox);

    gAddBook.AddBookEditMoreVboxHbox1 = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditMoreVbox), gAddBook.AddBookEditMoreVboxHbox1, TRUE, TRUE, 0);
    gAddBook.AddBookEditMoreSep = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditMoreVboxHbox1), gAddBook.AddBookEditMoreSep, TRUE, TRUE, 0);

    gAddBook.AddBookEditMoreVboxHbox2 = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditMoreVbox), gAddBook.AddBookEditMoreVboxHbox2, FALSE, FALSE, 4);
    gAddBook.AddBookSeriesLabel = gtk_label_new("Series");
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditMoreVboxHbox2), gAddBook.AddBookSeriesLabel, FALSE, FALSE, 4);
    gAddBook.AddBookSeriesEntry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditMoreVboxHbox2), gAddBook.AddBookSeriesEntry, TRUE, TRUE, 0);

    gAddBook.AddBookEditMoreVboxHbox3 = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditMoreVbox), gAddBook.AddBookEditMoreVboxHbox3, TRUE, TRUE, 0);
    gAddBook.AddBookWebsiteFrame = gtk_frame_new("Website:");
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditMoreVboxHbox3), gAddBook.AddBookWebsiteFrame, TRUE, TRUE, 0);
    gAddBook.AddBookWebsiteHBox = gtk_hbox_new(FALSE, 4);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookWebsiteFrame), gAddBook.AddBookWebsiteHBox);
    gAddBook.AddBookWebsiteHBoxVBoxLeft = gtk_vbox_new(FALSE, 4);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookWebsiteHBox), gAddBook.AddBookWebsiteHBoxVBoxLeft, FALSE, FALSE, 0);
    gAddBook.AddBookWebsiteAddWidgetEventBox = gtk_event_box_new();
    gtk_widget_set_events(gAddBook.AddBookWebsiteAddWidgetEventBox, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookWebsiteHBoxVBoxLeft), gAddBook.AddBookWebsiteAddWidgetEventBox, FALSE, FALSE, 0);
    gAddBook.AddBookWebsiteAddWidget = gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookWebsiteAddWidgetEventBox), gAddBook.AddBookWebsiteAddWidget);
    gAddBook.AddBookWebsiteDelWidgetEventBox = gtk_event_box_new();
    gtk_widget_set_events(gAddBook.AddBookWebsiteDelWidgetEventBox, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookWebsiteHBoxVBoxLeft), gAddBook.AddBookWebsiteDelWidgetEventBox, FALSE, FALSE, 0);
    gAddBook.AddBookWebsiteDelWidget = gtk_image_new_from_stock(GTK_STOCK_DELETE, GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookWebsiteDelWidgetEventBox), gAddBook.AddBookWebsiteDelWidget);
    gAddBook.AddBookWebsiteClrWidgetEventBox = gtk_event_box_new();
    gtk_widget_set_events(gAddBook.AddBookWebsiteClrWidgetEventBox, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookWebsiteHBoxVBoxLeft), gAddBook.AddBookWebsiteClrWidgetEventBox, FALSE, FALSE, 0);
    gAddBook.AddBookWebsiteClrWidget = gtk_image_new_from_stock(GTK_STOCK_CLEAR, GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookWebsiteClrWidgetEventBox), gAddBook.AddBookWebsiteClrWidget);
    gAddBook.AddBookWebsiteHBoxVBoxRight = gtk_vbox_new(FALSE, 4);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookWebsiteHBox), gAddBook.AddBookWebsiteHBoxVBoxRight, TRUE, TRUE, 0);
    gAddBook.AddBookWebsiteTreeviewScrollWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER(gAddBook.AddBookWebsiteTreeviewScrollWindow), 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(gAddBook.AddBookWebsiteTreeviewScrollWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookWebsiteHBoxVBoxRight), gAddBook.AddBookWebsiteTreeviewScrollWindow, FALSE, FALSE, 0);
    gAddBook.AddBookWebsiteTreeview = gtk_tree_view_new();
    gtk_widget_set_size_request(gAddBook.AddBookWebsiteTreeview, -1, 80);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookWebsiteTreeviewScrollWindow), gAddBook.AddBookWebsiteTreeview);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(gAddBook.AddBookWebsiteTreeview), FALSE);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(gAddBook.AddBookWebsiteTreeview), TRUE);

    gAddBook.AddBookCustomFrame = gtk_frame_new("Custom:");
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditMoreVboxHbox3), gAddBook.AddBookCustomFrame, TRUE, TRUE, 0);
    gAddBook.AddBookCustomHBox = gtk_hbox_new(FALSE, 4);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookCustomFrame), gAddBook.AddBookCustomHBox);
    gAddBook.AddBookCustomHBoxVBoxLeft = gtk_vbox_new(FALSE, 4);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookCustomHBox), gAddBook.AddBookCustomHBoxVBoxLeft, FALSE, FALSE, 0);
    gAddBook.AddBookCustomAddWidgetEventBox = gtk_event_box_new();
    gtk_widget_set_events(gAddBook.AddBookCustomAddWidgetEventBox, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookCustomHBoxVBoxLeft), gAddBook.AddBookCustomAddWidgetEventBox, FALSE, FALSE, 0);
    gAddBook.AddBookCustomAddWidget = gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookCustomAddWidgetEventBox), gAddBook.AddBookCustomAddWidget);
    gAddBook.AddBookCustomDelWidgetEventBox = gtk_event_box_new();
    gtk_widget_set_events(gAddBook.AddBookCustomDelWidgetEventBox, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookCustomHBoxVBoxLeft), gAddBook.AddBookCustomDelWidgetEventBox, FALSE, FALSE, 0);
    gAddBook.AddBookCustomDelWidget = gtk_image_new_from_stock(GTK_STOCK_DELETE, GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookCustomDelWidgetEventBox), gAddBook.AddBookCustomDelWidget);
    gAddBook.AddBookCustomClrWidgetEventBox = gtk_event_box_new();
    gtk_widget_set_events(gAddBook.AddBookCustomClrWidgetEventBox, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookCustomHBoxVBoxLeft), gAddBook.AddBookCustomClrWidgetEventBox, FALSE, FALSE, 0);
    gAddBook.AddBookCustomClrWidget = gtk_image_new_from_stock(GTK_STOCK_CLEAR, GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookCustomClrWidgetEventBox), gAddBook.AddBookCustomClrWidget);
    gAddBook.AddBookCustomHBoxVBoxRight = gtk_vbox_new(FALSE, 4);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookCustomHBox), gAddBook.AddBookCustomHBoxVBoxRight, TRUE, TRUE, 0);
    gAddBook.AddBookCustomTreeviewScrollWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER(gAddBook.AddBookCustomTreeviewScrollWindow), 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(gAddBook.AddBookCustomTreeviewScrollWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookCustomHBoxVBoxRight), gAddBook.AddBookCustomTreeviewScrollWindow, FALSE, FALSE, 0);
    gAddBook.AddBookCustomTreeview = gtk_tree_view_new();
    gtk_widget_set_size_request(gAddBook.AddBookCustomTreeview, -1, 80);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookCustomTreeviewScrollWindow), gAddBook.AddBookCustomTreeview);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(gAddBook.AddBookCustomTreeview), TRUE);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(gAddBook.AddBookCustomTreeview), TRUE);

    gAddBook.AddBookEditMoreVboxHbox4 = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditMoreVbox), gAddBook.AddBookEditMoreVboxHbox4, TRUE, TRUE, 0);
    gAddBook.AddBookIntroductionFrame = gtk_frame_new("Introduction:");
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditMoreVboxHbox4), gAddBook.AddBookIntroductionFrame, TRUE, TRUE, 0);
    gAddBook.AddBookIntroductionScrollWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER(gAddBook.AddBookIntroductionScrollWindow), 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(gAddBook.AddBookIntroductionScrollWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookIntroductionFrame), gAddBook.AddBookIntroductionScrollWindow);
    gAddBook.AddBookIntroductionTextview = gtk_text_view_new();
    gtk_widget_set_size_request(gAddBook.AddBookIntroductionTextview, -1, 80);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookIntroductionScrollWindow), gAddBook.AddBookIntroductionTextview);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(gAddBook.AddBookIntroductionTextview), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(gAddBook.AddBookIntroductionTextview), GTK_WRAP_WORD_CHAR);

    gAddBook.AddBookEditMoreVboxHbox5 = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditMoreVbox), gAddBook.AddBookEditMoreVboxHbox5, TRUE, TRUE, 0);
    gAddBook.AddBookContentsFrame = gtk_frame_new("Contents:");
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookEditMoreVboxHbox5), gAddBook.AddBookContentsFrame, TRUE, TRUE, 0);
    gAddBook.AddBookContentsScrollWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER(gAddBook.AddBookContentsScrollWindow), 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(gAddBook.AddBookContentsScrollWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookContentsFrame), gAddBook.AddBookContentsScrollWindow);
    gAddBook.AddBookContentsTextview = gtk_text_view_new();
    gtk_widget_set_size_request(gAddBook.AddBookContentsTextview, -1, 80);
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookContentsScrollWindow), gAddBook.AddBookContentsTextview);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(gAddBook.AddBookContentsTextview), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(gAddBook.AddBookContentsTextview), GTK_WRAP_WORD_CHAR);

    gAddBook.AddBookActionButtonBox = gtk_hbutton_box_new();
    gtk_hbutton_box_set_layout_default(GTK_BUTTONBOX_END);
    gtk_box_pack_start(GTK_BOX(gAddBook.AddBookMainVBox), gAddBook.AddBookActionButtonBox, FALSE, FALSE, 0);
    gAddBook.AddBookActionButtonReset = gtk_button_new_with_mnemonic("_Reset");
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookActionButtonBox), gAddBook.AddBookActionButtonReset);
    gAddBook.AddBookActionButtonAddContinue = gtk_button_new_with_mnemonic("_Add & Continue");
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookActionButtonBox), gAddBook.AddBookActionButtonAddContinue);
    gAddBook.AddBookActionButtonAddQuit = gtk_button_new_with_mnemonic("Add & _Quit");
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookActionButtonBox), gAddBook.AddBookActionButtonAddQuit);
    gAddBook.AddBookActionButtonCancel = gtk_button_new_with_mnemonic("_Cancel");
    gtk_container_add(GTK_CONTAINER(gAddBook.AddBookActionButtonBox), gAddBook.AddBookActionButtonCancel);

    gtk_window_set_focus(GTK_WINDOW(gAddBook.AddBookWindow), gAddBook.AddBookTitleEntry);
}

static void gbs_version_combox_append(GtkComboBox *combo)
{
    gtk_combo_box_append_text(combo, "1st edition");
    gtk_combo_box_append_text(combo, "2nd edition");
    gtk_combo_box_append_text(combo, "3rd edition");
    gtk_combo_box_append_text(combo, "4th edition");
    gtk_combo_box_append_text(combo, "5th edition");
    gtk_combo_box_append_text(combo, "6th edition");
    gtk_combo_box_append_text(combo, "7th edition");
    gtk_combo_box_append_text(combo, "8th edition");
    gtk_combo_box_append_text(combo, "9th edition");
    gtk_combo_box_append_text(combo, "10th edition");

    return;
}

static void gbs_gtk_book_add_combo_init(void)
{
    gbs_genre_combox_append(GTK_COMBO_BOX(gAddBook.AddBookGenreComboBoxEntry));
    gbs_subgenre_combox_append(GTK_COMBO_BOX(gAddBook.AddBookSubgenreComboBoxEntry), NULL);
    gbs_language_combox_append(GTK_COMBO_BOX(gAddBook.AddBookLanguageComboBoxEntry));
    gbs_format_combox_append(GTK_COMBO_BOX(gAddBook.AddBookFormatComboBoxEntry));
    gbs_publisher_combox_append(GTK_COMBO_BOX(gAddBook.AddBookPublisherComboBoxEntry));
    gbs_version_combox_append(GTK_COMBO_BOX(gAddBook.AddBookVersionComboBoxEntry));
}

static void gbs_gtk_book_add_treeview_edit(GtkCellRendererText * cell, const gchar * path_string, const gchar * new_text, gpointer data)
{
    GtkTreeModel *model = (GtkTreeModel *) data;
    GtkTreePath *path = gtk_tree_path_new_from_string(path_string);
    GtkTreeIter iter;

    gint column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    gtk_tree_model_get_iter(model, &iter, path);

    switch (column) {
    case 0:
        {
            gint i;
            gchar *old_text;

            gtk_tree_model_get(model, &iter, column, &old_text, -1);
            g_free(old_text);

            i = gtk_tree_path_get_indices(path)[0];
            gbs_debug("path indice %d, old: %s, new: %s\n", i, old_text, new_text);

            gtk_list_store_set(GTK_LIST_STORE(model), &iter, column, new_text, -1);
        }
        break;
    case 1:
        {
            gint i;
            gchar *old_text;

            gtk_tree_model_get(model, &iter, column, &old_text, -1);
            g_free(old_text);

            i = gtk_tree_path_get_indices(path)[1];
            gbs_debug("path indice %d, old: %s, new: %s\n", i, old_text, new_text);

            gtk_list_store_set(GTK_LIST_STORE(model), &iter, column, new_text, -1);
        }
        break;
    }

    gtk_tree_path_free(path);
}

static void gbs_gtk_book_add_treeview_init(void)
{
    /* init author treeview */
    GtkListStore *author_store;
    GtkTreeViewColumn *author_column;
    GtkCellRenderer *author_renderer;

    author_store = gtk_list_store_new(1, G_TYPE_STRING);
    author_renderer = gtk_cell_renderer_text_new();
    g_object_set(author_renderer, "editable", TRUE, NULL);
    g_signal_connect(author_renderer, "edited", G_CALLBACK(gbs_gtk_book_add_treeview_edit), GTK_TREE_MODEL(author_store));
    g_object_set_data(G_OBJECT(author_renderer), "column", GINT_TO_POINTER(0));

    author_column = gtk_tree_view_column_new_with_attributes("Author", author_renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(gAddBook.AddBookAuthorsTreeview), author_column);
    gtk_tree_view_set_model(GTK_TREE_VIEW(gAddBook.AddBookAuthorsTreeview), GTK_TREE_MODEL(author_store));
    g_object_unref(author_store);

    /* init keyword treeview */
    GtkListStore *keyword_store;
    GtkTreeViewColumn *keyword_column;
    GtkCellRenderer *keyword_renderer;

    keyword_store = gtk_list_store_new(1, G_TYPE_STRING);
    keyword_renderer = gtk_cell_renderer_text_new();
    g_object_set(keyword_renderer, "editable", TRUE, NULL);
    g_signal_connect(keyword_renderer, "edited", G_CALLBACK(gbs_gtk_book_add_treeview_edit), GTK_TREE_MODEL(keyword_store));
    g_object_set_data(G_OBJECT(keyword_renderer), "column", GINT_TO_POINTER(0));

    keyword_column = gtk_tree_view_column_new_with_attributes("Keyword", keyword_renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(gAddBook.AddBookKeywordsTreeview), keyword_column);
    gtk_tree_view_set_model(GTK_TREE_VIEW(gAddBook.AddBookKeywordsTreeview), GTK_TREE_MODEL(keyword_store));
    g_object_unref(keyword_store);

    /* init website treeview */
    GtkListStore *website_store;
    GtkTreeViewColumn *website_column;
    GtkCellRenderer *website_renderer;

    website_store = gtk_list_store_new(1, G_TYPE_STRING);
    website_renderer = gtk_cell_renderer_text_new();
    g_object_set(website_renderer, "editable", TRUE, NULL);
    g_signal_connect(website_renderer, "edited", G_CALLBACK(gbs_gtk_book_add_treeview_edit), GTK_TREE_MODEL(website_store));
    g_object_set_data(G_OBJECT(website_renderer), "column", GINT_TO_POINTER(0));

    website_column = gtk_tree_view_column_new_with_attributes("Website", website_renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(gAddBook.AddBookWebsiteTreeview), website_column);
    gtk_tree_view_set_model(GTK_TREE_VIEW(gAddBook.AddBookWebsiteTreeview), GTK_TREE_MODEL(website_store));
    g_object_unref(website_store);

    /* init custom treeview */
    GtkListStore *custom_store;
    GtkTreeViewColumn *custom_column_key;
    GtkCellRenderer *custom_renderer_key;
    GtkTreeViewColumn *custom_column_value;
    GtkCellRenderer *custom_renderer_value;

    custom_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

    custom_renderer_key = gtk_cell_renderer_text_new();
    g_object_set(custom_renderer_key, "editable", TRUE, NULL);
    g_signal_connect(custom_renderer_key, "edited", G_CALLBACK(gbs_gtk_book_add_treeview_edit), GTK_TREE_MODEL(custom_store));
    g_object_set_data(G_OBJECT(custom_renderer_key), "column", GINT_TO_POINTER(0));

    custom_column_key = gtk_tree_view_column_new_with_attributes("key", custom_renderer_key, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(gAddBook.AddBookCustomTreeview), custom_column_key);
    gtk_tree_view_column_set_min_width(custom_column_key, 100);

    custom_renderer_value = gtk_cell_renderer_text_new();
    g_object_set(custom_renderer_value, "editable", TRUE, NULL);
    g_signal_connect(custom_renderer_value, "edited", G_CALLBACK(gbs_gtk_book_add_treeview_edit), GTK_TREE_MODEL(custom_store));
    g_object_set_data(G_OBJECT(custom_renderer_value), "column", GINT_TO_POINTER(1));

    custom_column_value = gtk_tree_view_column_new_with_attributes("value", custom_renderer_value, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(gAddBook.AddBookCustomTreeview), custom_column_value);

    gtk_tree_view_set_model(GTK_TREE_VIEW(gAddBook.AddBookCustomTreeview), GTK_TREE_MODEL(custom_store));
    g_object_unref(custom_store);
}

void gbs_gtk_book_add(void)
{
    gbs_gtk_book_add_window_init();
    gbs_gtk_book_add_combo_init();
    gbs_gtk_book_add_treeview_init();

    g_signal_connect_swapped(G_OBJECT(gAddBook.AddBookWindow), "destroy", G_CALLBACK(gbs_gtk_book_add_cancel), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookTitleButton), "clicked", G_CALLBACK(gbs_gtk_book_add_browse), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookActionButtonAddQuit), "clicked", G_CALLBACK(gbs_gtk_book_add_quit), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookActionButtonAddContinue), "clicked", G_CALLBACK(gbs_gtk_book_add_continue), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookActionButtonCancel), "clicked", G_CALLBACK(gbs_gtk_book_add_cancel), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookActionButtonReset), "clicked", G_CALLBACK(gbs_gtk_book_add_reset), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookGenreComboBoxEntry), "changed", G_CALLBACK(on_genre_combox_changed), gAddBook.AddBookSubgenreComboBoxEntry);
    g_signal_connect(G_OBJECT(gAddBook.AddBookEditMoreExpander), "activate", G_CALLBACK(gbs_gtk_book_add_expander), NULL);

    g_signal_connect(G_OBJECT(gAddBook.AddBookAuthorsAddWidgetEventBox), "button-press-event", G_CALLBACK(gbs_gtk_book_add_author_add), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookAuthorsAddWidgetEventBox), "motion-notify-event", G_CALLBACK(gbs_gtk_book_add_author_add_motion_notify), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookAuthorsDelWidgetEventBox), "button-press-event", G_CALLBACK(gbs_gtk_book_add_author_del), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookAuthorsDelWidgetEventBox), "motion-notify-event", G_CALLBACK(gbs_gtk_book_add_author_del_motion_notify), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookAuthorsClrWidgetEventBox), "button-press-event", G_CALLBACK(gbs_gtk_book_add_author_clr), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookAuthorsClrWidgetEventBox), "motion-notify-event", G_CALLBACK(gbs_gtk_book_add_author_clr_motion_notify), NULL);

    g_signal_connect(G_OBJECT(gAddBook.AddBookKeywordsAddWidgetEventBox), "button-press-event", G_CALLBACK(gbs_gtk_book_add_keyword_add), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookKeywordsAddWidgetEventBox), "motion-notify-event", G_CALLBACK(gbs_gtk_book_add_keyword_add_motion_notify), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookKeywordsDelWidgetEventBox), "button-press-event", G_CALLBACK(gbs_gtk_book_add_keyword_del), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookKeywordsDelWidgetEventBox), "motion-notify-event", G_CALLBACK(gbs_gtk_book_add_keyword_del_motion_notify), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookKeywordsClrWidgetEventBox), "button-press-event", G_CALLBACK(gbs_gtk_book_add_keyword_clr), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookKeywordsClrWidgetEventBox), "motion-notify-event", G_CALLBACK(gbs_gtk_book_add_keyword_clr_motion_notify), NULL);

    g_signal_connect(G_OBJECT(gAddBook.AddBookWebsiteAddWidgetEventBox), "button-press-event", G_CALLBACK(gbs_gtk_book_add_website_add), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookWebsiteAddWidgetEventBox), "motion-notify-event", G_CALLBACK(gbs_gtk_book_add_website_add_motion_notify), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookWebsiteDelWidgetEventBox), "button-press-event", G_CALLBACK(gbs_gtk_book_add_website_del), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookWebsiteDelWidgetEventBox), "motion-notify-event", G_CALLBACK(gbs_gtk_book_add_website_del_motion_notify), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookWebsiteClrWidgetEventBox), "button-press-event", G_CALLBACK(gbs_gtk_book_add_website_clr), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookWebsiteClrWidgetEventBox), "motion-notify-event", G_CALLBACK(gbs_gtk_book_add_website_clr_motion_notify), NULL);

    g_signal_connect(G_OBJECT(gAddBook.AddBookCustomAddWidgetEventBox), "button-press-event", G_CALLBACK(gbs_gtk_book_add_custom_add), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookCustomAddWidgetEventBox), "motion-notify-event", G_CALLBACK(gbs_gtk_book_add_custom_add_motion_notify), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookCustomDelWidgetEventBox), "button-press-event", G_CALLBACK(gbs_gtk_book_add_custom_del), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookCustomDelWidgetEventBox), "motion-notify-event", G_CALLBACK(gbs_gtk_book_add_custom_del_motion_notify), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookCustomClrWidgetEventBox), "button-press-event", G_CALLBACK(gbs_gtk_book_add_custom_clr), NULL);
    g_signal_connect(G_OBJECT(gAddBook.AddBookCustomClrWidgetEventBox), "motion-notify-event", G_CALLBACK(gbs_gtk_book_add_custom_clr_motion_notify), NULL);

    gtk_widget_show_all(gAddBook.AddBookWindow);
    return;
}

void gbs_gtk_book_del(void)
{
    gint response;
    GtkWidget *dialog;
    GtkWidget *vbox, *hbox;
    GtkWidget *table;
    GdkPixbuf *pixbuf;
    GtkWidget *image;
    GtkWidget *label;

    //GtkWidget *title_combox;

    dialog = gtk_dialog_new_with_buttons("Delete book", GTK_WINDOW(g_window), GTK_DIALOG_MODAL |
                                         GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

    vbox = gtk_vbox_new(FALSE, 4);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vbox, FALSE, FALSE, 0);

    hbox = gtk_hbox_new(FALSE, 16);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 8);

    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), gtk_hseparator_new(), FALSE, FALSE, 0);

    pixbuf = gdk_pixbuf_new_from_inline(-1, gbs_logo_raw, FALSE, NULL);
    image = gtk_image_new_from_pixbuf(pixbuf);
    g_object_unref(pixbuf);

    gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);

    /* add table to the right of hbox */
    table = gtk_table_new(1, 1, FALSE);
    gtk_table_set_row_spacings(GTK_TABLE(table), 2);
    gtk_table_set_col_spacings(GTK_TABLE(table), 4);
    gtk_box_pack_start(GTK_BOX(hbox), table, FALSE, FALSE, 2);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>title</b>");
    gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1,0,1);

    gtk_widget_show_all(vbox);
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        g_modify_flag = 1;
    }

    gtk_widget_destroy(dialog);
    return;
}

