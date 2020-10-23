#include "gbookshelf.h"

static int g_sidebar_flag;      //*< 0: hide, 1, Genres, 2: Publishers */
static int g_modify_flag;
static int g_book_pagesize;

static gchar *g_db_filename;

static GtkWidget *g_window;
static GtkWidget *g_toolbar;
static GtkWidget *g_statusbar;
static GtkWidget *g_sidebar_treeview;
static GtkWidget *g_book_treeview;
static GtkWidget *g_thumbs_treeview;

static gchar *gAddBookOldDir;
GbsAddBookWindow gAddBook;

GdkPixbuf *gbs_logo_create_pixbuf(void)
{
    return gdk_pixbuf_new_from_inline(-1, gbs_logo_raw, FALSE, NULL);
}

/* create a new hbox with an image and a label packed into it
 * and return the box.. */
GtkWidget *gbs_create_image_text(const guint8 * raw_data, gchar * label_text)
{
    GtkWidget *hbox;
    GtkWidget *label;
    GtkWidget *image;
    GdkPixbuf *pixbuf;

    /* create box for xpm and label */
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_border_width(GTK_CONTAINER(hbox), 0);

    pixbuf = gdk_pixbuf_new_from_inline(-1, raw_data, FALSE, NULL);
    image = gtk_image_new_from_pixbuf(pixbuf);
    g_object_unref(pixbuf);
    gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 4);

    label = gtk_label_new(label_text);
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

    return hbox;
}

static const gchar *gbs_authors[] = {
    "liaoxf <liaofei1128@gmail.com>", NULL
};

static const gchar *gbs_documentors[] = {
    "liaoxf <liaofei1128@gmail.com>", NULL
};

static const gchar *gbs_license =
    "This library is free software; you can redistribute it and/or\n"
    "modify it under the terms of the GNU Library General Public License as\n"
    "published by the Free Software Foundation; either version 2 of the\n"
    "License, or (at your option) any later version.\n"
    "\n"
    "This library is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
    "Library General Public License for more details.\n"
    "\n"
    "You should have received a copy of the GNU Library General Public\n"
    "License along with the Gnome Library; see the file COPYING.LIB.  If not,\n"
    "write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,\n"
    "Boston, MA 02111-1307, USA.\n";

void gbs_about(void)
{
    GdkPixbuf *pixbuf, *transparent;

    pixbuf = NULL;
    transparent = NULL;

    pixbuf = gbs_logo_create_pixbuf();
    transparent = gdk_pixbuf_add_alpha(pixbuf, TRUE, 0xff, 0xff, 0xff);
    g_object_unref(pixbuf);

    gtk_show_about_dialog(GTK_WINDOW(g_window), "name", "gbookshelf", "version", "1.0", "copyright", "(C) 2010-2011 gbookshelf", "license", gbs_license, "website", "http://www.gbookshelf.com", "comments", "Program to manage your ebooks.", "authors", gbs_authors, "documenters", gbs_documentors, "logo", transparent, "title", "About gbookshelf", NULL);

    g_object_unref(transparent);
}

static void gbs_help_create_tags(GtkTextBuffer * buffer)
{
    gtk_text_buffer_create_tag(buffer, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_buffer_create_tag(buffer, "italic", "style", PANGO_STYLE_ITALIC, NULL);
    gtk_text_buffer_create_tag(buffer, "monospace", "family", "monospace", NULL);

    gtk_text_buffer_create_tag(buffer, "title", "weight", PANGO_WEIGHT_HEAVY, "justification", GTK_JUSTIFY_CENTER, NULL);
    gtk_text_buffer_create_tag(buffer, "H1", "weight", PANGO_WEIGHT_HEAVY, "size", 16 * PANGO_SCALE, "foreground", "red", NULL);
    gtk_text_buffer_create_tag(buffer, "H2", "weight", PANGO_WEIGHT_ULTRABOLD, "size", 14 * PANGO_SCALE, "foreground", "green", NULL);
    gtk_text_buffer_create_tag(buffer, "H3", "weight", PANGO_WEIGHT_BOLD, "size", 12 * PANGO_SCALE, "foreground", "blue", NULL);
    gtk_text_buffer_create_tag(buffer, "H4", "weight", PANGO_WEIGHT_SEMIBOLD, "size", 10 * PANGO_SCALE, "foreground", "gray", NULL);
    gtk_text_buffer_create_tag(buffer, "H5", "weight", PANGO_WEIGHT_NORMAL, "size", 8 * PANGO_SCALE, "foreground", "cyan", NULL);
    gtk_text_buffer_create_tag(buffer, "H6", "weight", PANGO_WEIGHT_LIGHT, "size", 6 * PANGO_SCALE, "foreground", "yellow", NULL);

    gtk_text_buffer_create_tag(buffer, "underline", "underline", PANGO_UNDERLINE_SINGLE, NULL);
    gtk_text_buffer_create_tag(buffer, "strikethrough", "strikethrough", TRUE, NULL);
    gtk_text_buffer_create_tag(buffer, "superscript", "rise", 10 * PANGO_SCALE, "size", 8 * PANGO_SCALE, NULL);
    gtk_text_buffer_create_tag(buffer, "subscript", "rise", -10 * PANGO_SCALE, "size", 8 * PANGO_SCALE, NULL);
    gtk_text_buffer_create_tag(buffer, "code", "foreground", "black", "background", "gray", NULL);
    gtk_text_buffer_create_tag(buffer, "wrap", "wrap_mode", GTK_WRAP_WORD, NULL);
    gtk_text_buffer_create_tag(buffer, "center", "justification", GTK_JUSTIFY_CENTER, NULL);
    gtk_text_buffer_create_tag(buffer, "right", "justification", GTK_JUSTIFY_RIGHT, NULL);
    gtk_text_buffer_create_tag(buffer, "margin", "left_margin", 24, "right_margin", 24, NULL);
}

static void gbs_help_insert_text(GtkTextBuffer * buffer)
{
    GtkTextIter iter;
    GtkTextIter start, end;

    GdkPixbuf *pixbuf;
    GdkPixbuf *scaled;

    pixbuf = gbs_logo_create_pixbuf();
    scaled = gdk_pixbuf_scale_simple(pixbuf, 32, 32, GDK_INTERP_BILINEAR);
    g_object_unref(pixbuf);
    pixbuf = scaled;

    gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);

    gtk_text_buffer_insert_pixbuf(buffer, &iter, pixbuf);
    gtk_text_buffer_insert(buffer, &iter, "\n", -1);
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "gBookshelf Manual", -1, "title", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);

    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Introduction", -1, "H2", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);

    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Genre management", -1, "H2", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Add genre", -1, "H3", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Delete genre", -1, "H3", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Add subgenre", -1, "H3", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Delete subgenre", -1, "H3", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);

    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Publisher management", -1, "H2", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Add publisher", -1, "H3", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Delete publisher", -1, "H3", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);

    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Book management", -1, "H2", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Add eBook", -1, "H3", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Delete eBook", -1, "H3", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Search eBook", -1, "H3", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Import eBook", -1, "H3", NULL);

    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Database management", -1, "H2", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "New database", -1, "H3", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Open database", -1, "H3", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Save database", -1, "H3", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Use SQLite3", -1, "H3", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Database tables", -1, "H3", NULL);
    gtk_text_buffer_insert(buffer, &iter, "\n\n", -1);

    /* Apply word_wrap tag to whole buffer */
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gtk_text_buffer_apply_tag_by_name(buffer, "wrap", &start, &end);
    g_object_unref(pixbuf);
}

void gbs_help(void)
{
    GtkTextBuffer *buffer;
    GtkWidget *help_scroll_window;
    GtkWidget *help_dialog, *help_textview;

    help_dialog = gtk_dialog_new_with_buttons("Help", GTK_WINDOW(g_window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_NONE, NULL);

    gtk_widget_set_size_request(help_dialog, 480, 320);

    help_scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER(help_scroll_window), 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(help_scroll_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(help_dialog)->vbox), help_scroll_window);

    help_textview = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(help_textview), FALSE);
    gtk_container_add(GTK_CONTAINER(help_scroll_window), help_textview);
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(help_textview));

    gbs_help_create_tags(buffer);
    gbs_help_insert_text(buffer);

    g_signal_connect_swapped(help_dialog, "response", G_CALLBACK(gtk_widget_destroy), help_dialog);

    gtk_widget_show_all(help_dialog);
}

void gbs_message_dialog(int type, char *info, char *str, ...)
{
    va_list ap;
    char messages[1024];
    GtkWidget *dialog;

    switch (type) {
    case GTK_MESSAGE_INFO:
        dialog = gtk_message_dialog_new_with_markup(GTK_WINDOW(g_window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "<b>%s</b>", info);
        break;
    case GTK_MESSAGE_WARNING:
        dialog = gtk_message_dialog_new_with_markup(GTK_WINDOW(g_window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_CLOSE, "%s", info);
        break;
    case GTK_MESSAGE_QUESTION:
        dialog = gtk_message_dialog_new_with_markup (GTK_WINDOW(g_window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_CLOSE, "<b>%s</b>", info);
        break;
    case GTK_MESSAGE_ERROR:
        dialog = gtk_message_dialog_new_with_markup(GTK_WINDOW(g_window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "<b>%s</b>", info);
        break;
    case GTK_MESSAGE_OTHER:
        dialog = gtk_message_dialog_new_with_markup(GTK_WINDOW(g_window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_OTHER, GTK_BUTTONS_CLOSE, "<b>%s</b>", info);
    default:
        return;
    }

    va_start(ap, str);
    vsnprintf(messages, 1024, str, ap);
    va_end(ap);

    gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG (dialog), "<span color=\"red\">%s</span>", messages);

    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
    return;
}

static void gbs_sidebar_treeview_selected(GtkWidget * widget, gpointer statusbar)
{
    char *value;
    GtkTreeIter iter;
    GtkTreeModel *model;

    if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model, &iter)) {
        gtk_tree_model_get(model, &iter, 0, &value, -1);
        gtk_statusbar_push(GTK_STATUSBAR(statusbar), gtk_statusbar_get_context_id(GTK_STATUSBAR
                                                        (statusbar), value), value);
        g_free(value);
    }
}

static void gbs_sidebar_treeview_header_clicked(GtkWidget * widget, gpointer sidebar)
{
    switch (g_sidebar_flag) {
    case 1:
        printf("resort the genres tree cells.\n");
        break;
    case 2:
        printf("resort the publisher tree cells.\n");
        break;
    default:
        break;
    }
}

static void gbs_sidebar_treeview_expand_all(void)
{
    gtk_tree_view_expand_all(GTK_TREE_VIEW(g_sidebar_treeview));
}

static void gbs_sidebar_treeview_collapse_all(void)
{
    gtk_tree_view_collapse_all(GTK_TREE_VIEW(g_sidebar_treeview));
}

GtkTreeModel *gbs_sidebar_create_tree_model(void)
{
    GtkTreeStore *treestore;

    treestore = gtk_tree_store_new(1, G_TYPE_STRING);

    return GTK_TREE_MODEL(treestore);
}

void gbs_sidebar_clear_tree_model(void)
{
    GtkTreeModel *model;
    GtkTreeStore *treestore;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(g_sidebar_treeview));
    treestore = GTK_TREE_STORE(model);
    gtk_tree_store_clear(treestore);
}

GtkTreeModel *gbs_sidebar_update_tree_model(int flag)
{
    GtkTreeModel *model;
    GtkTreeStore *treestore;
    GtkTreeViewColumn *column;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(g_sidebar_treeview));
    treestore = GTK_TREE_STORE(model);
    column =
        gtk_tree_view_get_column(GTK_TREE_VIEW(g_sidebar_treeview), 0);

    gtk_tree_store_clear(treestore);

    if (!flag) {
        gtk_tree_view_column_set_title(column, "_Genre");
        gbs_genre_update_tree_model(treestore);
    } else {
        gtk_tree_view_column_set_title(column, "_Publisher");
        gbs_publisher_update_tree_model(treestore);
    }
    return GTK_TREE_MODEL(treestore);
}

static void gbs_sidebar_genre_actived(void)
{
    g_sidebar_flag = 1;
    gbs_sidebar_update_tree_model(0);
}

static void gbs_sidebar_publisher_actived(void)
{
    g_sidebar_flag = 2;
    gbs_sidebar_update_tree_model(1);
}

static void gbs_sidebar_hide(void)
{
    gtk_widget_hide(g_sidebar_widget);
}

static void gbs_sidebar_show(void)
{
    switch(g_sidebar_flag) {
    case 1:
        gbs_sidebar_update_tree_model(0);
        break;
    case 2:
        gbs_sidebar_update_tree_model(1);
        break;
    default:
        break;
    }
    gtk_widget_show(g_sidebar_widget);
}

static void gbs_sidebar_treeview_create(GtkWidget * left_scroll_window)
{
    GtkTreeModel *model;
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;
    GtkTreeSelection *selection;

    g_sidebar_treeview = gtk_tree_view_new();

    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_clickable(column, TRUE);
    g_signal_connect(column, "clicked", G_CALLBACK(gbs_sidebar_treeview_header_clicked), g_sidebar_treeview);
    gtk_tree_view_append_column(GTK_TREE_VIEW(g_sidebar_treeview), column);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 0);

    model = gbs_sidebar_create_tree_model();
    gtk_tree_view_set_model(GTK_TREE_VIEW(g_sidebar_treeview), model);
    g_object_unref(model);

    //gtk_tree_view_expand_all(GTK_TREE_VIEW(g_sidebar_treeview));
    //gtk_tree_view_set_reorderable(GTK_TREE_VIEW(g_sidebar_treeview), TRUE);
    gtk_tree_view_set_enable_search(GTK_TREE_VIEW(g_sidebar_treeview), TRUE);

    gtk_container_add(GTK_CONTAINER(left_scroll_window), g_sidebar_treeview);
    selection =
        gtk_tree_view_get_selection(GTK_TREE_VIEW(g_sidebar_treeview));
    g_signal_connect(selection, "changed", G_CALLBACK(gbs_sidebar_treeview_selected), g_statusbar);

    gbs_sidebar_genre_actived();
    return;
}

void gbs_combo_box_clear(GtkComboBox *combo)
{
    GtkTreeModel *model;
    GtkListStore *liststore;

    model = gtk_combo_box_get_model(combo);
    liststore = GTK_LIST_STORE(model);
    gtk_list_store_clear(liststore);
}

void on_genre_combox_changed(GtkWidget * widget, gpointer data)
{
    gchar *genre = gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget));
    gbs_combo_box_clear(GTK_COMBO_BOX(data));
    gbs_subgenre_combox_append(GTK_COMBO_BOX(data), genre);
    gtk_combo_box_set_active(GTK_COMBO_BOX(data), 0);
    g_free(genre);
}

static gboolean gbs_menu_save_response(void)
{
    gboolean ret = FALSE;
    GtkWidget *dialog;

    if (!g_modify_flag) {
        if (g_db_filename != NULL)
            return TRUE;
    }

    dialog = gtk_file_chooser_dialog_new("Save Database", GTK_WINDOW(g_window), GTK_FILE_CHOOSER_ACTION_SAVE,
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER
                                                   (dialog), TRUE);

    if (g_db_filename == NULL) {
        gchar *pwd = g_get_current_dir();
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), pwd);
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), GBS_DATABASE_NAME);
        g_free(pwd);
    } else {
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), g_db_filename);
    }

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        ret = gbs_db_write(filename);
        if(ret < 0) {
            gbs_message_dialog(GTK_MESSAGE_ERROR, "Save file failed!", "When write into database %s, sqlite3 exec ret %d", filename, ret);
        } else {
            g_modify_flag = 0;
            ret = TRUE;
        }
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
    return ret;
}

static gboolean gbs_check_modify_and_save(void)
{
    gboolean ret = FALSE;
    gint response;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *image;
    GtkWidget *label;
    GtkWidget *dialog;

    if (!g_modify_flag)
        return TRUE;

    dialog = gtk_dialog_new_with_buttons("gbookshelf", GTK_WINDOW(g_window), GTK_DIALOG_MODAL |
                                         GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

    vbox = gtk_vbox_new(FALSE, 4);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vbox, TRUE, TRUE, 0);

    hbox = gtk_hbox_new(FALSE, 4);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 4);

    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), gtk_hseparator_new(), FALSE, FALSE, 0);

    image =
        gtk_image_new_from_stock(GTK_STOCK_DIALOG_QUESTION, GTK_ICON_SIZE_DIALOG);
    gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "The current database has been <span background=\"gray\">modified</span>!\n"
                         "Do you want to <b>save it</b> before create a new database?");
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);

    gtk_widget_show_all(vbox);
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        ret = gbs_menu_save_response();
    }

    gtk_widget_destroy(dialog);

    return ret;
}

static gboolean gbs_menu_new_response(void)
{
    if (!gbs_check_modify_and_save()) {
        return FALSE;
    }

    gbs_publisher_fini();
    gbs_format_fini();
    gbs_language_fini();
    gbs_genre_fini();
    gbs_book_fini();
    gbs_sidebar_clear_tree_model();
    gbs_gtk_booklist_update_model_first_page();

    if (g_db_filename) {
        g_free(g_db_filename);
        g_db_filename = NULL;
    }

    return TRUE;
}

static gboolean gbs_menu_quit_response(GtkWidget * widget, GdkEvent * event, gpointer data)
{
    if (!gbs_check_modify_and_save()) {
        return FALSE;
    }

    gtk_main_quit();
    return FALSE;
}

static gboolean gbs_menu_open_response(void)
{
    int ret;
    GtkWidget *dialog;

    if (!gbs_check_modify_and_save()) {
        return FALSE;
    }

    dialog = gtk_file_chooser_dialog_new("Open Database", GTK_WINDOW(g_window), GTK_FILE_CHOOSER_ACTION_OPEN,
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        gbs_publisher_fini();
        gbs_format_fini();
        gbs_language_fini();
        gbs_genre_fini();
        gbs_book_fini();
        gbs_sidebar_clear_tree_model();
        gbs_gtk_booklist_update_model_first_page();

        if (g_db_filename) {
            g_free(g_db_filename);
            g_db_filename = NULL;
        }


        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gbs_debug("open database file %s\n", filename);
        ret = gbs_db_read(filename);
        if (ret < 0) {
            gbs_message_dialog (GTK_MESSAGE_INFO, "load database failed!", "load database <i>%s</i> failed <i>%s</i>", filename, gbs_err(ret));
            g_free(filename);
            return;
        }

        gbs_sidebar_update_tree_model(g_sidebar_flag);
        gbs_gtk_booklist_update_model_first_page();
        if (g_db_filename) {
            g_free(g_db_filename);
        }
        g_db_filename = filename;
    }

    gtk_widget_destroy(dialog);
}

static void gbs_menu_dummy(gchar * string)
{
    printf("%s\n", string);
}

static int gbs_create_menu(GtkWidget * headbox)
{
    GtkWidget *gbs_menubar;
    GtkWidget *menu, *menu_item;
    GtkWidget *sub_menu, *sub_menu_item;
    GtkWidget *file_menu, *edit_menu, *opt_menu, *help_menu;

    GtkAccelGroup *accel_group = NULL;
    accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(g_window), accel_group);

    gbs_menubar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(headbox), gbs_menubar, FALSE, FALSE, 0);

    /* the File menu */
    file_menu = gtk_menu_item_new_with_mnemonic("_File");
    gtk_menu_shell_append(GTK_MENU_SHELL(gbs_menubar), file_menu);

    menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu), menu);

    /* File->New Database */
    menu_item = gtk_menu_item_new_with_mnemonic("_New Database");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_menu_new_response), NULL);
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, GDK_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    /* File->Open Database */
    menu_item = gtk_menu_item_new_with_mnemonic("_Open Database");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_menu_open_response), NULL);
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, GDK_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    /* File->Save Database */
    menu_item = gtk_menu_item_new_with_mnemonic("_Save Database");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_menu_save_response), NULL);

    menu_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    /* File->Import Files */
    menu_item = gtk_menu_item_new_with_mnemonic("_Import Files");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_menu_dummy), (gpointer) g_strdup("Import File"));

    /* File->Import Directory */
    menu_item = gtk_menu_item_new_with_mnemonic("Import _Directory");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_menu_dummy), (gpointer) g_strdup("Import Dir"));

    menu_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    /* File->Export */
    menu_item = gtk_menu_item_new_with_mnemonic("_Export Wizard");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_menu_dummy), (gpointer) g_strdup("Export"));

    menu_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    /* File->Quit */
    menu_item = gtk_menu_item_new_with_mnemonic("_Quit");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_menu_quit_response), NULL);
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, GDK_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    /* the Edit menu */
    edit_menu = gtk_menu_item_new_with_mnemonic("_Edit");
    gtk_menu_shell_append(GTK_MENU_SHELL(gbs_menubar), edit_menu);

    menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_menu), menu);

    /* Edit->Copy */
    menu_item = gtk_menu_item_new_with_mnemonic("_Copy");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_menu_dummy), (gpointer) g_strdup("Copy"));
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, GDK_c, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    /* Edit->Paste */
    menu_item = gtk_menu_item_new_with_mnemonic("_Paste");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_menu_dummy), (gpointer) g_strdup("Paste"));
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, GDK_v, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    menu_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    /* Edit->Add book */
    menu_item = gtk_menu_item_new_with_mnemonic("_Add Book");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_gtk_book_add), NULL);
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, GDK_a, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    /* Edit->Delete book */
    menu_item = gtk_menu_item_new_with_mnemonic("_Delete Book");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_gtk_book_del), NULL);

    menu_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    /* Edit->Add Genre */
    menu_item = gtk_menu_item_new_with_mnemonic("_Add Genre");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_gtk_genre_add), NULL);

    /* Edit->Delete Genre */
    menu_item = gtk_menu_item_new_with_mnemonic("_Delete Genre");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_gtk_genre_del), NULL);

    menu_item = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    /* Edit->Add Publisher */
    menu_item = gtk_menu_item_new_with_mnemonic("_Add Publisher");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_gtk_publisher_add), NULL);

    /* Edit->Delete Publisher */
    menu_item = gtk_menu_item_new_with_mnemonic("_Delete Publisher");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_gtk_publisher_del), NULL);

    /* the Options menu */
    GSList *group = NULL;
    opt_menu = gtk_menu_item_new_with_mnemonic("_Options");
    gtk_menu_shell_append(GTK_MENU_SHELL(gbs_menubar), opt_menu);

    menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(opt_menu), menu);

    /* Options->Browse */
    menu_item = gtk_menu_item_new_with_mnemonic("_Browse");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    sub_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), sub_menu);

    sub_menu_item =
        gtk_radio_menu_item_new_with_mnemonic(group, "Browse by _Genres");
    gtk_menu_shell_append(GTK_MENU_SHELL(sub_menu), sub_menu_item);
    g_signal_connect_swapped(G_OBJECT(sub_menu_item), "activate", G_CALLBACK(gbs_sidebar_genre_actived), NULL);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(sub_menu_item), TRUE);

    group =
        gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(sub_menu_item));
    sub_menu_item =
        gtk_radio_menu_item_new_with_mnemonic(group, "Browse by _Publisher");
    gtk_menu_shell_append(GTK_MENU_SHELL(sub_menu), sub_menu_item);
    g_signal_connect_swapped(G_OBJECT(sub_menu_item), "activate", G_CALLBACK(gbs_sidebar_publisher_actived), NULL);

    /* Options->Expand all */
    menu_item = gtk_menu_item_new_with_mnemonic("_Expand all");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_sidebar_treeview_expand_all), NULL);

    /* Options->Collapse all */
    menu_item = gtk_menu_item_new_with_mnemonic("_Collapse all");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_sidebar_treeview_collapse_all), NULL);

    /* the Help menu */
    help_menu = gtk_menu_item_new_with_mnemonic("_Help");
    gtk_menu_shell_append(GTK_MENU_SHELL(gbs_menubar), help_menu);
    gtk_menu_item_set_right_justified(GTK_MENU_ITEM(help_menu), TRUE);

    menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_menu), menu);

    /* Help->Contents */
    menu_item = gtk_menu_item_new_with_mnemonic("_Contents");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_help), NULL);

    /* Help->About */
    menu_item = gtk_menu_item_new_with_mnemonic("_About");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    g_signal_connect_swapped(G_OBJECT(menu_item), "activate", G_CALLBACK(gbs_about), NULL);

    return 0;
}

static void gbs_create_toolbar(GtkWidget * headbox)
{
    GtkToolItem *new_db;
    GtkToolItem *open_db;
    GtkToolItem *save_db;
    GtkToolItem *sep;
    GtkToolItem *exit;

    g_toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(g_toolbar), GTK_TOOLBAR_ICONS);
    //gtk_container_set_border_width(GTK_CONTAINER(g_toolbar), 0);

    /* Tool->New Database */
    new_db = gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(new_db), "New Database");
    gtk_toolbar_insert(GTK_TOOLBAR(g_toolbar), new_db, -1);
    g_signal_connect(G_OBJECT(new_db), "clicked", G_CALLBACK(gbs_menu_new_response), NULL);

    /* Tool->Open Database */
    open_db = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(open_db), "Open Database");
    gtk_toolbar_insert(GTK_TOOLBAR(g_toolbar), open_db, -1);
    g_signal_connect(G_OBJECT(open_db), "clicked", G_CALLBACK(gbs_menu_open_response), NULL);

    /* Tool->Save Database */
    save_db = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
    gtk_toolbar_insert(GTK_TOOLBAR(g_toolbar), save_db, -1);
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(save_db), "Save Database");
    g_signal_connect(G_OBJECT(save_db), "clicked", G_CALLBACK(gbs_menu_save_response), NULL);

    sep = gtk_separator_tool_item_new();
    gtk_toolbar_insert(GTK_TOOLBAR(g_toolbar), sep, -1);

    exit = gtk_tool_button_new_from_stock(GTK_STOCK_QUIT);
    gtk_toolbar_insert(GTK_TOOLBAR(g_toolbar), exit, -1);
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(exit), "Quit");
    g_signal_connect(G_OBJECT(exit), "clicked", G_CALLBACK(gbs_menu_quit_response), NULL);

    gtk_box_pack_start(GTK_BOX(headbox), g_toolbar, FALSE, FALSE, 0);
    return;
}

static void gbs_gtk_booklist_scaned_toggled(GtkCellRendererToggle * cell, gchar * path_str, gpointer data)
{
    GtkTreeIter iter;
    GtkTreeModel *model = (GtkTreeModel *) data;
    GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
    gboolean scaned;

    gbs_debug("path:[%s]\n", path_str);
    /* get toggled iter */
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, GBS_BOOK_COLUMN_SCANED, &scaned, -1);

    /* do something with the value */
    scaned ^= 1;

    /* set new value */
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, GBS_BOOK_COLUMN_SCANED, scaned, -1);

    /* clean up */
    gtk_tree_path_free(path);
}

static void gbs_gtk_booklist_add_columns(GtkTreeView * treeview)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);

    /* column for scaned toggles */
    renderer = gtk_cell_renderer_toggle_new();
    g_signal_connect(renderer, "toggled", G_CALLBACK(gbs_gtk_booklist_scaned_toggled), model);

    column = gtk_tree_view_column_new_with_attributes("Scaned", renderer, "active", GBS_BOOK_COLUMN_SCANED, NULL);

    /* set this column to a fixed sizing (of 50 pixels) */
    gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column), GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN(column), 50);
    gtk_tree_view_append_column(treeview, column);

    /* column for book format */
    renderer = gtk_cell_renderer_pixbuf_new();
    column = gtk_tree_view_column_new_with_attributes("Format", renderer, "pixbuf", GBS_BOOK_COLUMN_FORMAT, NULL);
    gtk_tree_view_column_set_sort_column_id(column, GBS_BOOK_COLUMN_FORMAT);
    gtk_tree_view_append_column(treeview, column);

    /* column for book title */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Title", renderer, "text", GBS_BOOK_COLUMN_TITLE, NULL);
    gtk_tree_view_column_set_sort_column_id(column, GBS_BOOK_COLUMN_TITLE);
    gtk_tree_view_append_column(treeview, column);

    /* column for book author */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Author", renderer, "text", GBS_BOOK_COLUMN_AUTHOR, NULL);
    gtk_tree_view_column_set_sort_column_id(column, GBS_BOOK_COLUMN_AUTHOR);
    gtk_tree_view_append_column(treeview, column);

    /* column for book publisher */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Publisher", renderer, "text", GBS_BOOK_COLUMN_PUBLISHER, NULL);
    gtk_tree_view_column_set_sort_column_id(column, GBS_BOOK_COLUMN_PUBLISHER);
    gtk_tree_view_append_column(treeview, column);

    /* column for book language */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Language", renderer, "text", GBS_BOOK_COLUMN_LANGUAGE, NULL);
    gtk_tree_view_column_set_sort_column_id(column, GBS_BOOK_COLUMN_LANGUAGE);
    gtk_tree_view_append_column(treeview, column);

    /* column for book version */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Version", renderer, "text", GBS_BOOK_COLUMN_VERSION, NULL);
    gtk_tree_view_column_set_sort_column_id(column, GBS_BOOK_COLUMN_VERSION);
    gtk_tree_view_append_column(treeview, column);
}

static void gbs_create_notebook_pages(GtkWidget * notebook)
{
    GtkWidget *label;
    GtkWidget *book_scroll_window;

    /**< list */
    label = gtk_label_new_with_mnemonic("Book _List");
    book_scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER(book_scroll_window), 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(book_scroll_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), book_scroll_window, label);

    g_book_treeview = gtk_tree_view_new_with_model(gbs_book_create_model());
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(g_book_treeview), TRUE);
    gtk_tree_view_set_search_column(GTK_TREE_VIEW(g_book_treeview), GBS_BOOK_COLUMN_TITLE);
    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(g_book_treeview)), GTK_SELECTION_SINGLE);
    gtk_container_add(GTK_CONTAINER(book_scroll_window), g_book_treeview);
    gbs_gtk_booklist_add_columns(GTK_TREE_VIEW(g_book_treeview));

    /**< thumbs */
    label = gtk_label_new_with_mnemonic("Book _Thumbs");
    book_scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER(book_scroll_window), 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(book_scroll_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), book_scroll_window, label);

    label = gtk_label_new_with_mnemonic("Book _Scaned");
    book_scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER(book_scroll_window), 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(book_scroll_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), book_scroll_window, label);

    label = gtk_label_new_with_mnemonic("Book _Info");
    book_scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER(book_scroll_window), 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(book_scroll_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), book_scroll_window, label);

    label = gtk_label_new_with_mnemonic("Book _View");
    book_scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER(book_scroll_window), 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(book_scroll_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), book_scroll_window, label);

    return;
}

void gbs_page_size_combo_changed(GtkWidget * widget, gpointer data)
{
    gchar *text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget));
    if(!str_equal(text, "all")){
        g_book_pagesize = atoi(text);
    } else {
        g_book_pagesize = -1;
    }
    gbs_gtk_booklist_update_model_first_page();
    g_free(text);
}

void gbs_gtk_book_goto_first_page(GtkWidget * widget, gpointer data)
{
    gbs_gtk_booklist_update_model_first_page();
}

void gbs_gtk_book_goto_prev_page(GtkWidget * widget, gpointer data)
{
    gbs_gtk_booklist_update_model_prev_page();
}

void gbs_gtk_book_goto_next_page(GtkWidget * widget, gpointer data)
{
    gbs_gtk_booklist_update_model_next_page();
}

void gbs_gtk_book_goto_last_page(GtkWidget * widget, gpointer data)
{
    gbs_gtk_booklist_update_model_last_page();
}

static int gbs_create_layout(void)
{
    GtkWidget *layout;
    GtkWidget *head_box;
    GtkWidget *body_box;
    GtkWidget *foot_box;

    GtkWidget *right_body_box;
    GtkWidget *right_body_box_top;
    GtkWidget *right_body_box_middle;
    GtkWidget *right_body_box_bottom;

    GtkWidget *notebook;
    GtkWidget *left_scroll_window;

    GtkWidget *search_expander;
    GtkWidget *search_expander_table;
    GtkWidget *control_pannel;

    layout = gtk_vbox_new(FALSE, 1);
    head_box = gtk_vbox_new(FALSE, 1);
    body_box = gtk_hbox_new(FALSE, 1);
    foot_box = gtk_vbox_new(FALSE, 1);

    gtk_box_pack_start(GTK_BOX(layout), head_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(layout), body_box, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(layout), foot_box, FALSE, FALSE, 0);

    /* create the menu basr */
    gbs_create_menu(head_box);

    /* create the tool basr */
    gbs_create_toolbar(head_box);

    /* create the status basr */
    g_statusbar = gtk_statusbar_new();
    gtk_box_pack_start(GTK_BOX(foot_box), g_statusbar, TRUE, TRUE, 0);

    /* create the left genre tree view */
    left_scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(left_scroll_window, 240, -1);
    gtk_container_set_border_width(GTK_CONTAINER(left_scroll_window), 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(left_scroll_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(body_box), left_scroll_window, FALSE, FALSE, 1);

    gbs_sidebar_treeview_create(left_scroll_window);

    /* the right box */
    right_body_box = gtk_vbox_new(FALSE, 1);
    gtk_box_pack_start(GTK_BOX(body_box), right_body_box, TRUE, TRUE, 0);

    /* create the right top notebook */
    right_body_box_top = gtk_vbox_new(FALSE, 1);
    gtk_box_pack_start(GTK_BOX(right_body_box), right_body_box_top, TRUE, TRUE, 0);

    notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
    gtk_box_pack_start(GTK_BOX(right_body_box_top), notebook, TRUE, TRUE, 1);
    gbs_create_notebook_pages(notebook);

    /* create the right middle controls */
    right_body_box_middle = gtk_hbox_new(TRUE, 1);
    gtk_box_pack_start(GTK_BOX(right_body_box), right_body_box_middle, FALSE, FALSE, 0);
    gtk_widget_set_size_request(right_body_box_middle, -1, 30);

    control_pannel = gtk_hbox_new(TRUE, 1);
    gtk_box_pack_start(GTK_BOX(right_body_box_middle), control_pannel, FALSE, FALSE, 0);

    GtkWidget *button_image;
    GtkWidget *page_size_combo;
    GtkWidget *first_page, *last_page;
    GtkWidget *prev_page, *next_page;

    page_size_combo = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(page_size_combo), "10");
    gtk_combo_box_append_text(GTK_COMBO_BOX(page_size_combo), "20");
    gtk_combo_box_append_text(GTK_COMBO_BOX(page_size_combo), "40");
    gtk_combo_box_append_text(GTK_COMBO_BOX(page_size_combo), "All");
    gtk_combo_box_set_active(GTK_COMBO_BOX(page_size_combo), 3);
    gtk_container_add(GTK_CONTAINER(control_pannel), page_size_combo);
    g_signal_connect(G_OBJECT(page_size_combo), "changed", G_CALLBACK(gbs_page_size_combo_changed), NULL);

    first_page = gtk_button_new();
    button_image = gbs_create_image_text(gbs_first_raw, "First Page");
    gtk_container_add(GTK_CONTAINER(first_page), button_image);
    gtk_container_add(GTK_CONTAINER(control_pannel), first_page);
    g_signal_connect(G_OBJECT(first_page), "clicked", G_CALLBACK(gbs_gtk_book_goto_first_page), NULL);

    prev_page = gtk_button_new();
    button_image = gbs_create_image_text(gbs_left_raw, "Prev Page");
    gtk_container_add(GTK_CONTAINER(prev_page), button_image);
    gtk_container_add(GTK_CONTAINER(control_pannel), prev_page);
    g_signal_connect(G_OBJECT(prev_page), "clicked", G_CALLBACK(gbs_gtk_book_goto_prev_page), NULL);

    next_page = gtk_button_new();
    button_image = gbs_create_image_text(gbs_right_raw, "Next Page");
    gtk_container_add(GTK_CONTAINER(next_page), button_image);
    gtk_container_add(GTK_CONTAINER(control_pannel), next_page);
    g_signal_connect(G_OBJECT(next_page), "clicked", G_CALLBACK(gbs_gtk_book_goto_next_page), NULL);

    last_page = gtk_button_new();
    button_image = gbs_create_image_text(gbs_last_raw, "Last Page");
    gtk_container_add(GTK_CONTAINER(last_page), button_image);
    gtk_container_add(GTK_CONTAINER(control_pannel), last_page);
    g_signal_connect(G_OBJECT(last_page), "clicked", G_CALLBACK(gbs_gtk_book_goto_last_page), NULL);

    /* create the right bottom seach buttons */
    right_body_box_bottom = gtk_vbox_new(FALSE, 1);
    gtk_box_pack_start(GTK_BOX(right_body_box), right_body_box_bottom, FALSE, FALSE, 0);

    search_expander = gtk_expander_new_with_mnemonic("Fi_nd books");
    gtk_box_pack_start(GTK_BOX(right_body_box_bottom), search_expander, TRUE, TRUE, 1);

    search_expander_table = gtk_table_new(2, 6, FALSE);
    gtk_container_add(GTK_CONTAINER(search_expander), search_expander_table);

    gtk_table_set_row_spacings(GTK_TABLE(search_expander_table), 2);
    gtk_table_set_col_spacings(GTK_TABLE(search_expander_table), 4);

    // row 1
    GtkWidget *search_title_label;
    search_title_label = gtk_label_new("Title");
    gtk_misc_set_alignment(GTK_MISC(search_title_label), 0.2, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(search_expander_table), search_title_label, 0,1,0,1);
    GtkWidget *search_title_entry;
    search_title_entry = gtk_entry_new();
    gtk_entry_set_has_frame(GTK_ENTRY(search_title_entry), FALSE);
    gtk_table_attach_defaults(GTK_TABLE(search_expander_table), search_title_entry, 1,2,0,1);

    GtkWidget *search_author_label;
    search_author_label = gtk_label_new("Author");
    gtk_misc_set_alignment(GTK_MISC(search_author_label), 0.2, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(search_expander_table), search_author_label, 2,3,0,1);
    GtkWidget *search_author_entry;
    search_author_entry = gtk_entry_new();
    gtk_entry_set_has_frame(GTK_ENTRY(search_author_entry), FALSE);
    gtk_table_attach_defaults(GTK_TABLE(search_expander_table), search_author_entry, 3,4,0,1);

    GtkWidget *search_publisher_label;
    search_publisher_label = gtk_label_new("Publisher");
    gtk_misc_set_alignment(GTK_MISC(search_publisher_label), 0.2, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(search_expander_table), search_publisher_label, 4,5,0,1);
    GtkWidget *search_publisher_entry;
    search_publisher_entry = gtk_entry_new();
    gtk_entry_set_has_frame(GTK_ENTRY(search_publisher_entry), FALSE);
    gtk_table_attach_defaults(GTK_TABLE(search_expander_table), search_publisher_entry, 5,6,0,1);

    // row 2
    GtkWidget *search_genre_label;
    search_genre_label = gtk_label_new("Genre");
    gtk_misc_set_alignment(GTK_MISC(search_genre_label), 0.2, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(search_expander_table), search_genre_label, 0,1,1,2);
    GtkWidget *search_genre_entry;
    search_genre_entry = gtk_entry_new();
    gtk_entry_set_has_frame(GTK_ENTRY(search_genre_entry), FALSE);
    gtk_table_attach_defaults(GTK_TABLE(search_expander_table), search_genre_entry, 1,2,1,2);

    GtkWidget *search_keyword_label;
    search_keyword_label = gtk_label_new("Keyword");
    gtk_misc_set_alignment(GTK_MISC(search_keyword_label), 0.2, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(search_expander_table), search_keyword_label, 2,3,1,2);
    GtkWidget *search_keyword_entry;
    search_keyword_entry = gtk_entry_new();
    gtk_entry_set_has_frame(GTK_ENTRY(search_keyword_entry), FALSE);
    gtk_table_attach_defaults(GTK_TABLE(search_expander_table), search_keyword_entry, 3,4,1,2);

    GtkWidget *search_button;
    search_button = gtk_button_new_with_mnemonic("_Do search");
    gtk_table_attach_defaults(GTK_TABLE(search_expander_table), search_button, 4,5,1,2);
    GtkWidget *search_result;
    search_result = gtk_label_new("");
    gtk_table_attach_defaults(GTK_TABLE(search_expander_table), search_result, 5,6,1,2);

    gtk_container_add(GTK_CONTAINER(g_window), layout);
    return 0;
}

int main(int argc, char *argv[])
{
    gbs_book_init();
    gbs_genre_init();
    gbs_publisher_init();
    gbs_format_init();
    gbs_language_init();

    int width, height;
    GdkScreen *screen = NULL;

    gtk_init(&argc, &argv);

    g_sidebar_flag = 0;
    g_modify_flag = 0;
    g_book_pagesize = 40;
    g_db_filename = NULL;
    gAddBookOldDir = NULL;
    g_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(g_window), "gBookShelf");
    gtk_container_set_border_width(GTK_CONTAINER(g_window), 1);

    screen = gtk_window_get_screen(GTK_WINDOW(g_window));
    width = gdk_screen_get_width(screen);
    height = gdk_screen_get_height(screen);
    printf("gbookshelf v1.0\n");
    printf("Screen resolution is (%d x %d)\n", width, height);

    gtk_window_maximize(GTK_WINDOW(g_window));
    gtk_window_set_default_size(GTK_WINDOW(g_window), 800, 600);
    gtk_window_set_position(GTK_WINDOW(g_window), GTK_WIN_POS_CENTER);
    gtk_window_set_icon(GTK_WINDOW(g_window), gbs_logo_create_pixbuf());

    gbs_create_layout();

    g_signal_connect_swapped(G_OBJECT(g_window), "destroy", G_CALLBACK(gbs_menu_quit_response), NULL);

    gtk_window_set_focus(GTK_WINDOW(g_window), g_book_treeview);
    gtk_widget_show_all(g_window);
    gtk_main();

    if (g_db_filename != NULL) {
        g_free(g_db_filename);
    }

    if (gAddBookOldDir != NULL) {
        g_free(gAddBookOldDir);
    }

    gbs_language_fini();
    gbs_format_fini();
    gbs_publisher_fini();
    gbs_genre_fini();
    gbs_book_fini();

    return 0;
}
