#include "gbookshelf.h"

GtkTreeModel *gbs_language_create_model(void)
{
    gbs_language_t *lang;
    GtkTreeIter langIter;
    GtkTreeStore *treestore;

    treestore = gtk_tree_store_new(1, G_TYPE_STRING);
    list_for_each_entry(lang, &g_language_list, node) {
        gtk_tree_store_append(treestore, &langIter, NULL);
        gtk_tree_store_set(treestore, &langIter,
            0, lang->language, -1);
    }

    return GTK_TREE_MODEL(treestore);
}

void gbs_language_update_tree_model(GtkTreeStore * treestore)
{
    gbs_language_t *lang;
    GtkTreeIter langIter;

    list_for_each_entry(lang, &g_language_list, node) {
        gtk_tree_store_append(treestore, &langIter, NULL);
        gtk_tree_store_set(treestore, &langIter,
            0, lang->language, -1);
    }
    return;
}

void gbs_language_combox_append(GtkComboBox * combo)
{
    gbs_language_t *lang;

    list_for_each_entry(lang, &g_language_list, node) {
        gtk_combo_box_append_text(combo, lang->language);
    }

    return;
}

