#include "gbookshelf.h"

GtkTreeModel *gbs_genre_create_model(void)
{
    int i;
    char *genre;
    GtkTreeIter genreIter;
    GtkTreeStore *treestore;

    treestore = gtk_tree_store_new(1, G_TYPE_STRING);
    for(i=0; i<dpa_used(&g_main_genres); i++) {
        genre = g_main_genres.array[i];
        gtk_tree_store_append(treestore, &genreIter, NULL);
        gtk_tree_store_set(treestore, &genreIter, 0, genre, -1);
    }

    return GTK_TREE_MODEL(treestore);
}

GtkTreeModel *gbs_subgenre_create_model(char *name)
{
    int i;
    gbs_genre_t *genre;
    GtkTreeIter subgenreIter;
    GtkTreeStore *treestore;

    treestore = gtk_tree_store_new(1, G_TYPE_STRING);
    list_for_each_entry(genre, &g_genre_list, node) {
        if (name ? !strcmp(&genre->genre, name) : 1) {
            gtk_tree_store_append(treestore, &subgenreIter, NULL);
            gtk_tree_store_set(treestore, &subgenreIter, 0, genre->subgenre, -1);
        }
    }

    return GTK_TREE_MODEL(treestore);
}

void gbs_genre_update_tree_model(GtkTreeStore * treestore)
{
    int i;
    gbs_genre_t *genre;
    GtkTreeIter genreIter, subgenreIter;

    list_for_each_entry(genre, &g_genre_list, node) {
        gtk_tree_store_append(treestore, &genreIter, NULL);
        gtk_tree_store_set(treestore, &genreIter, 0, genre->genre, -1);
        gtk_tree_store_append(treestore, &subgenreIter, &genreIter);
        gtk_tree_store_set(treestore, &subgenreIter, 0, genre->subgenre, -1);
    }
    return;
}

void gbs_genre_combox_append(GtkComboBox * combo)
{
    gbs_genre_t *genre;

    list_for_each_entry(genre, &g_genre_list, node) {
        gtk_combo_box_append_text(combo, genre->genre);
    }

    return;
}

void gbs_subgenre_combox_append(GtkComboBox * combo, char *name)
{
    int i;
    gbs_genre_t *genre;

    list_for_each_entry(genre, &g_genre_list, node) {
        if (name ? !strcmp(&genre->genre, name) : 1) {
            gtk_combo_box_append_text(combo, genre->subgenre);
        }
    }

    return;
}

void gbs_gtk_genre_add(void)
{
    int ret;
    gint response;
    GtkWidget *dialog;
    GtkWidget *vbox, *hbox;
    GtkWidget *table;
    GdkPixbuf *pixbuf;
    GtkWidget *image;
    GtkWidget *label;

    GtkWidget *genre_entry;
    GtkWidget *subgenre_entry;

    GtkTextBuffer *buffer;
    GtkTextIter start, end;
    GtkWidget *description_textview;
    GtkWidget *description_scrollwindow;

    GtkEntryCompletion *completion;
    GtkTreeModel *completion_model;

    dialog = gtk_dialog_new_with_buttons("Add genre", GTK_WINDOW(g_window), GTK_DIALOG_MODAL |
                                         GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

    vbox = gtk_vbox_new(FALSE, 4);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vbox, FALSE, FALSE, 0);

    hbox = gtk_hbox_new(FALSE, 16);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 8);

    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), gtk_hseparator_new(), FALSE, FALSE, 0);

    pixbuf = gdk_pixbuf_new_from_inline(-1, gbs_addgenre_raw, FALSE, NULL);
    image = gtk_image_new_from_pixbuf(pixbuf);
    g_object_unref(pixbuf);

    gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);

    /* add table to the right of hbox */
    table = gtk_table_new(3, 2, FALSE);
    gtk_table_set_row_spacings(GTK_TABLE(table), 2);
    gtk_table_set_col_spacings(GTK_TABLE(table), 4);
    gtk_box_pack_start(GTK_BOX(hbox), table, FALSE, FALSE, 2);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Genre</b>");
    gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1,0,1);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Subgenre</b>");
    gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1,1,2);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Description</b>");
    gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1,2,3);

    /* genre input entry */
    genre_entry = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(table), genre_entry, 1, 2, 0, 1);

    completion = gtk_entry_completion_new();
    gtk_entry_completion_set_minimum_key_length(GTK_ENTRY_COMPLETION(completion), 1);
    gtk_entry_set_completion(GTK_ENTRY(genre_entry), completion);
    g_object_unref(completion);

    completion_model = gbs_genre_create_model();
    gtk_entry_completion_set_model(completion, completion_model);
    g_object_unref(completion_model);

    gtk_entry_completion_set_text_column(completion, 0);

    /* subgenre input entry */
    subgenre_entry = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(table), subgenre_entry, 1, 2, 1, 2);

    /* description input textview */
    description_scrollwindow = gtk_scrolled_window_new(NULL, NULL);
    //gtk_widget_set_size_request(description_scrollwindow, -1, 200);
    gtk_container_set_border_width(GTK_CONTAINER(description_scrollwindow), 1);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(description_scrollwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    gtk_table_attach_defaults(GTK_TABLE(table), description_scrollwindow, 1, 2, 2, 3);

    description_textview = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(description_textview), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(description_textview), GTK_WRAP_WORD_CHAR);
    gtk_container_add(GTK_CONTAINER(description_scrollwindow), description_textview);
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(description_textview));

    gtk_widget_show_all(vbox);
run:
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        //add genre and update treeview;
        char *genre = (char *)gtk_entry_get_text (GTK_ENTRY(genre_entry));
        char *subgenre = (char *)gtk_entry_get_text (GTK_ENTRY(subgenre_entry));
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        char *description = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(buffer), &start, &end, FALSE);
        gbs_debug("%s\n%s\n%s\n", genre, subgenre, description);
        if ((ret = gbs_genre_insert (genre, subgenre, description)) < 0) {
            gbs_message_dialog (GTK_MESSAGE_INFO, "add genre failed!", "add genre <i>%s</i>, subgenre <i>%s</i>: <b>%s</b>", genre, subgenre, gbs_err(ret));
            goto run;
        }
        if (g_sidebar_flag == 0)
            gbs_sidebar_update_tree_model(0);
        g_modify_flag = 1;
        g_free(description);
    }

    gtk_widget_destroy(dialog);
    return;
}

void gbs_gtk_genre_del(void)
{
    int ret;
    gint response;
    GtkWidget *dialog;
    GtkWidget *vbox, *hbox;
    GtkWidget *table;
    GdkPixbuf *pixbuf;
    GtkWidget *image;
    GtkWidget *label;

    GtkWidget *genre_combox;
    GtkWidget *subgenre_combox;

    dialog = gtk_dialog_new_with_buttons("Delete genre", GTK_WINDOW(g_window), GTK_DIALOG_MODAL |
                                         GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

    vbox = gtk_vbox_new(FALSE, 4);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vbox, FALSE, FALSE, 0);

    hbox = gtk_hbox_new(FALSE, 16);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 8);

    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), gtk_hseparator_new(), FALSE, FALSE, 0);

    pixbuf = gdk_pixbuf_new_from_inline(-1, gbs_addgenre_raw, FALSE, NULL);
    image = gtk_image_new_from_pixbuf(pixbuf);
    g_object_unref(pixbuf);

    gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);

    /* add table to the right of hbox */
    table = gtk_table_new(2, 2, FALSE);
    gtk_table_set_row_spacings(GTK_TABLE(table), 2);
    gtk_table_set_col_spacings(GTK_TABLE(table), 4);
    gtk_box_pack_start(GTK_BOX(hbox), table, FALSE, FALSE, 2);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Genre</b>");
    gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1,0,1);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Subgenre</b>");
    gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1,1,2);

    /* genre combox entry */
    genre_combox = gtk_combo_box_new_text(); //gtk_combo_box_entry_new_with_model(gbs_genre_create_model(), 0);
    gtk_table_attach_defaults(GTK_TABLE(table), genre_combox, 1, 2, 0, 1);

    /* subgenre combox entry */
    subgenre_combox = gtk_combo_box_new_text(); //gtk_combo_box_entry_new_text();
    gtk_table_attach_defaults(GTK_TABLE(table), subgenre_combox, 1, 2, 1, 2);

    gbs_genre_combox_append(GTK_COMBO_BOX(genre_combox));
    gbs_subgenre_combox_append(GTK_COMBO_BOX(subgenre_combox), NULL);

    g_signal_connect (genre_combox, "changed", G_CALLBACK (on_genre_combox_changed), subgenre_combox);

    gtk_widget_show_all(vbox);
run:
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        //del genre and update treeview;
        char *genre = gtk_combo_box_get_active_text (GTK_COMBO_BOX(genre_combox));
        char *subgenre = gtk_combo_box_get_active_text (GTK_COMBO_BOX(subgenre_combox));
        if ((ret = gbs_genre_delete (genre, subgenre)) < 0){
            gbs_message_dialog (GTK_MESSAGE_INFO, "Delete genre failed!", "delete genre <i>%s</i>, subgenre <i>%s</i>: <b>%s</b>", genre, subgenre, gbs_err(ret));
            goto run;
        }
        if (g_sidebar_flag == 0)
            gbs_sidebar_update_tree_model(0);
        g_modify_flag = 1;
        g_free(subgenre);
        g_free(genre);
    }

    gtk_widget_destroy(dialog);
    return;
}

