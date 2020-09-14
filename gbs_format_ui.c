#include "gbookshelf.h"

GtkTreeModel *gbs_format_create_model(void)
{
    gbs_format_t *fmt;
    GtkTreeIter fmtIter;
    GtkTreeStore *treestore;

    treestore = gtk_tree_store_new(1, G_TYPE_STRING);
    list_for_each_entry(fmt, &g_format_list, node) {
        gtk_tree_store_append(treestore, &fmtIter, NULL);
        gtk_tree_store_set(treestore, &fmtIter, 0, fmt->format, -1);
    }

    return GTK_TREE_MODEL(treestore);
}

void gbs_format_update_tree_model(GtkTreeStore * treestore)
{
    gbs_format_t *fmt;
    GtkTreeIter fmtIter;

    list_for_each_entry(fmt, &g_format_list, node) {
        gtk_tree_store_append(treestore, &fmtIter, NULL);
        gtk_tree_store_set(treestore, &fmtIter, 0, fmt->format, -1);
    }
    return;
}

void gbs_format_combox_append(GtkComboBox * combo)
{
    gbs_format_t *fmt;

    list_for_each_entry(fmt, &g_format_list, node) {
        gtk_combo_box_append_text(combo, fmt->format);
    }

    return;
}

int gbs_format_combox_insert_new(GtkComboBox * combo, char *suffix)
{
    int index = 0;
    int actived = 0;
    gbs_format_t *fmt;

    list_for_each_entry(fmt, &g_format_list, node) {
        if (!strcasecmp(fmt->format, suffix)) {
            gtk_combo_box_set_active(combo, index);
            actived = 1;
            break;
        }
        index++;
    }

    if (!actived) {
        gchar *nfmt = g_ascii_strup(suffix, -1);
        gbs_format_insert(nfmt, "System insert automatically");
        gtk_combo_box_append_text(combo, nfmt);
        gtk_combo_box_set_active(combo, index);
        g_free(nfmt);
        return 1;
    }

    return 0;
}

GdkPixbuf *gbs_format_get_pixbuf(char *format)
{
    const guint8 *fmt_raw;

    if (!strcasecmp(format, "avi")) {
        fmt_raw = gbs_avi_raw;
    } else if (!strcasecmp(format, "bmp")) {
        fmt_raw = gbs_bmp_raw;
    } else if (!strcasecmp(format, "cab")) {
        fmt_raw = gbs_cab_raw;
    } else if (!strcasecmp(format, "chm")) {
        fmt_raw = gbs_chm_raw;
    } else if (!strcasecmp(format, "css")) {
        fmt_raw = gbs_css_raw;
    } else if (!strcasecmp(format, "doc")) {
        fmt_raw = gbs_doc_raw;
    } else if (!strcasecmp(format, "docx")) {
        fmt_raw = gbs_docx_raw;
    } else if (!strcasecmp(format, "gif")) {
        fmt_raw = gbs_gif_raw;
    } else if (!strcasecmp(format, "hlp")) {
        fmt_raw = gbs_hlp_raw;
    } else if (!strcasecmp(format, "html")) {
        fmt_raw = gbs_html_raw;
    } else if (!strcasecmp(format, "iso")) {
        fmt_raw = gbs_iso_raw;
    } else if (!strcasecmp(format, "java")) {
        fmt_raw = gbs_java_raw;
    } else if (!strcasecmp(format, "jpeg")) {
        fmt_raw = gbs_jpeg_raw;
    } else if (!strcasecmp(format, "jpg")) {
        fmt_raw = gbs_jpg_raw;
    } else if (!strcasecmp(format, "pdf")) {
        fmt_raw = gbs_pdf_raw;
    } else if (!strcasecmp(format, "php")) {
        fmt_raw = gbs_php_raw;
    } else if (!strcasecmp(format, "png")) {
        fmt_raw = gbs_png_raw;
    } else if (!strcasecmp(format, "ppt")) {
        fmt_raw = gbs_ppt_raw;
    } else if (!strcasecmp(format, "pptx")) {
        fmt_raw = gbs_pptx_raw;
    } else if (!strcasecmp(format, "rar")) {
        fmt_raw = gbs_rar_raw;
    } else if (!strcasecmp(format, "txt")) {
        fmt_raw = gbs_txt_raw;
    } else if (!strcasecmp(format, "wri")) {
        fmt_raw = gbs_wri_raw;
    } else if (!strcasecmp(format, "xsl")) {
        fmt_raw = gbs_xsl_raw;
    } else if (!strcasecmp(format, "xlsx")) {
        fmt_raw = gbs_xlsx_raw;
    } else if (!strcasecmp(format, "xml")) {
        fmt_raw = gbs_xml_raw;
    } else if (!strcasecmp(format, "xsl")) {
        fmt_raw = gbs_xsl_raw;
    } else if (!strcasecmp(format, "zip")) {
        fmt_raw = gbs_zip_raw;
    } else {
        fmt_raw = gbs_unknown_raw;
    }
    return gdk_pixbuf_new_from_inline(-1, fmt_raw, FALSE, NULL);
}

