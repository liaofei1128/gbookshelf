#include "gbookshelf.h"

GtkTreeModel *gbs_publisher_create_model(void)
{
    gbs_publisher_t *pub;
    GtkTreeIter pubIter;
    GtkTreeStore *treestore;

    treestore = gtk_tree_store_new(1, G_TYPE_STRING);
    list_for_each_entry(pub, &g_publisher_list, node) {
        gtk_tree_store_append(treestore, &pubIter, NULL);
        gtk_tree_store_set(treestore, &pubIter,
            0, pub->publisher, -1);
    }

    return GTK_TREE_MODEL(treestore);
}

void gbs_publisher_update_tree_model(GtkTreeStore * treestore)
{
    gbs_publisher_t *pub;
    GtkTreeIter pubIter;

    list_for_each_entry(pub, &g_publisher_list, node) {
        gtk_tree_store_append(treestore, &pubIter, NULL);
        gtk_tree_store_set(treestore, &pubIter, 0, pub->publisher, -1);
    }
    return;
}

void gbs_publisher_combox_append(GtkComboBox * combo)
{
    gbs_publisher_t *pub;

    list_for_each_entry(pub, &g_publisher_list, node) {
        gtk_combo_box_append_text(combo, pub->publisher);
    }

    return;
}

void gbs_gtk_publisher_add(void)
{
    int ret;
    gint response;
    GtkWidget *dialog;
    GtkWidget *vbox, *hbox;
    GtkWidget *table;
    GdkPixbuf *pixbuf;
    GtkWidget *image;
    GtkWidget *label;

    GtkWidget *publisher_entry;
    GtkWidget *website_entry;

    GtkTextBuffer *buffer;
    GtkTextIter start, end;
    GtkWidget *description_textview;
    GtkWidget *description_scrollwindow;

    GtkEntryCompletion *completion;
    GtkTreeModel *completion_model;

    dialog = gtk_dialog_new_with_buttons("Add publisher", GTK_WINDOW(g_window), GTK_DIALOG_MODAL |
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
    table = gtk_table_new(3, 2, FALSE);
    gtk_table_set_row_spacings(GTK_TABLE(table), 2);
    gtk_table_set_col_spacings(GTK_TABLE(table), 4);
    gtk_box_pack_start(GTK_BOX(hbox), table, FALSE, FALSE, 2);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>publisher</b>");
    gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1,0,1);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>website</b>");
    gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1,1,2);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Description</b>");
    gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1,2,3);

    /* publisher input entry */
    publisher_entry = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(table), publisher_entry, 1, 2, 0, 1);

    completion = gtk_entry_completion_new();
    gtk_entry_completion_set_minimum_key_length(GTK_ENTRY_COMPLETION(completion), 1);
    gtk_entry_set_completion(GTK_ENTRY(publisher_entry), completion);
    g_object_unref(completion);

    completion_model = gbs_publisher_create_model();
    gtk_entry_completion_set_model(completion, completion_model);
    g_object_unref(completion_model);

    gtk_entry_completion_set_text_column(completion, 0);

    /* website input entry */
    website_entry = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(table), website_entry, 1, 2, 1, 2);

    /* description input textview */
    description_scrollwindow = gtk_scrolled_window_new(NULL, NULL);
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
        //add publisher and update treeview;
        char *publisher = (char *)gtk_entry_get_text (GTK_ENTRY(publisher_entry));
        char *website = (char *)gtk_entry_get_text (GTK_ENTRY(website_entry));
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        char *description = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(buffer), &start, &end, FALSE);
        gbs_debug("%s\n%s\n%s\n", publisher, website, description);
        if ((ret = gbs_publisher_insert (publisher, website, description)) < 0){
            gbs_message_dialog (GTK_MESSAGE_INFO, "Add publisher failed!", "add publisher <i>%s</i>: <b>%s</b>", publisher, gbs_err(ret));
            goto run;
        }
        if (g_sidebar_flag == 1)
            gbs_sidebar_update_tree_model(1);
        g_modify_flag = 1;
        g_free(description);
    }

    gtk_widget_destroy(dialog);
    return;
}

void gbs_gtk_publisher_del(void)
{
    int ret;
    gint response;
    GtkWidget *dialog;
    GtkWidget *vbox, *hbox;
    GtkWidget *table;
    GdkPixbuf *pixbuf;
    GtkWidget *image;
    GtkWidget *label;

    GtkWidget *publisher_combox;

    dialog = gtk_dialog_new_with_buttons("Delete publisher", GTK_WINDOW(g_window), GTK_DIALOG_MODAL |
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
    gtk_label_set_markup(GTK_LABEL(label), "<b>publisher</b>");
    gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1,0,1);

    /* publisher combox entry */
    publisher_combox = gtk_combo_box_new_text();
    gbs_publisher_combox_append(GTK_COMBO_BOX(publisher_combox));
    gtk_table_attach_defaults(GTK_TABLE(table), publisher_combox, 1, 2, 0, 1);

    gtk_widget_show_all(vbox);
run:
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        //del publisher and update treeview;
        char *publisher = gtk_combo_box_get_active_text (GTK_COMBO_BOX(publisher_combox));
        if ((ret = gbs_publisher_delete (publisher)) < 0) {
            gbs_message_dialog (GTK_MESSAGE_INFO, "Delete publisher failed!", "delete publisher <i>%s</i>: <b>%s</b>", publisher, gbs_err(ret));
            goto run;
        }
        if (g_sidebar_flag == 1)
            gbs_sidebar_update_tree_model(1);
        g_modify_flag = 1;
        g_free(publisher);
    }

    gtk_widget_destroy(dialog);
    return;
}


