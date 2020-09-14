#include "gbookshelf.h"

static unsigned int g_publisher_cnt;
static struct list_head g_publisher_list;

gbs_publisher_t *gbs_publisher_alloc(void)
{
    gbs_publisher_t *publisher = malloc(sizeof(gbs_publisher_t));
    if (publisher == NULL)
        return NULL;

    memset(publisher, 0, sizeof(gbs_publisher_t));
    return publisher;
}

void gbs_publisher_free(gbs_publisher_t * publisher)
{
    if (publisher) {
        mbsfree(publisher->publisher);
        mbsfree(publisher->website);
        mbsfree(publisher->description);
        free(publisher);
    }
}

gbs_publisher_t *gbs_publisher_find(char *name)
{
    gbs_publisher_t *publisher;

    list_for_each_entry(publisher, &g_publisher_list, node) {
        if (!strcmp(publisher->publisher, name)) {
            return publisher;
        }
    }

    return NULL;
}

int gbs_publisher_insert(char *name, char *website, char *description)
{
    gbs_publisher_t *publisher;

    publisher = gbs_publisher_find(name);
    if (publisher)
        return -GBS_ERROR_EXIST;

    publisher = gbs_publisher_alloc();
    if (publisher == NULL)
        return -GBS_ERROR_NOMEM;

    publisher->publisher = mbsnewescapesqlite(name);
    publisher->website = mbsnewescapesqlite(website);
    publisher->description = mbsnewescapesqlite(description);
    list_add_tail(&publisher->node, &g_publisher_list);
    g_publisher_cnt++;
    return 0;
}

int gbs_publisher_delete(char *name)
{
    gbs_publisher_t *publisher;

    publisher = gbs_publisher_find(name);
    if (!publisher)
        return -GBS_ERROR_NOT_EXIST;

    list_del(&publisher->node);
    gbs_publisher_free(publisher);
    g_publisher_cnt--;
    return 0;
}

static gbs_publisher_name_t g_default_publishers[] = {
    { "人民教育出版社", "", "" },
    { "清华大学出版社", "", "" },
    { "电子工业出版社", "", "" },
    { "高等教育出版社", "", "" },
    { "机械工业出版社", "", "" },
    { "科学出版社", "", "" },
    { "人民邮电出版社", "", "" },
    { "北京师范大学出版社", "", "" },
    { "人民卫生出版社", "", "" },
    { "中国人民大学出版社", "", "" },
    { "法律出版社", "", "" },
    { "浙江教育出版社", "", "" },
    { "北京大学出版社", "", "" },
    { "中国电力出版社", "", "" },
    { "化学工业出版社", "", "" },
    { "中国建筑工业出版社", "", "" },
    { "教育科学出版社", "", "" },
    { "中国青年出版社", "", "" },
    { "上海外语教育出版社", "", "" },
    { "北京出版社", "", "" },
    { "中国铁道出版社", "", "" },
    { "中国财政经济出版社", "", "" },
    { "高教出版社", "", "" },
    { "北京教育出版社", "", "" },
    { "人民出版社", "", "" },
    { "国防工业出版社", "", "" },
    { "北师大出版社", "", "" },
    { "人民大学出版社", "", "" },
    { "上海人民出版社", "", "" },
    { "冶金工业出版社", "", "" },
    { "武汉大学出版社", "", "" },
    { "长江文艺出版社", "", "" },
    { "中国水利水电出版社", "", "" },
    { "译林出版社", "", "" },
    { "化工出版社", "", "" },
    { "中国地图出版社", "", "" },
    { "中国纺织出版社", "", "" },
    { "中国社会科学出版社", "", "" },
    { "上海古籍出版社", "", "" },
    { "新华出版社", "", "" },
    { "水利水电出版社", "", "" },
    { "中国经济出版社", "", "" },
    { "复旦大学出版社", "", "" },
    { "上海译文出版社", "", "" },
    { "人民音乐出版社", "", "" },
    { "经济科学出版社", "", "" },
    { "东南大学出版社", "", "" },
    { "铁道出版社", "", "" },
    { "光明日报出版社", "", "" },
    { "世界知识出版社", "", "" },
    { "人民日报出版社", "", "" },
    { "西安交通大学出版社", "", "" },
    { "浙江大学出版社", "", "" },
    { "中国少年儿童出版社", "", "" },
    { "中国农业出版社", "", "" },
    { "湖南教育出版社", "", "" },
    { "人教出版社", "", "" },
    { "学苑出版社", "", "" },
    { "上海科技出版社", "", "" },
    { "人民交通出版社", "", "" },
    { "中国林业出版社", "", "" },
    { "西南师范大学出版社", "", "" },
    { "新疆青少年出版社", "", "" },
    { "上海交通大学出版社", "", "" },
    { "重庆大学出版社", "", "" },
    { "上海文艺出版社", "", "" },
    { "内蒙古人民出版社", "", "" },
    { "中国广播电视出版社", "", "" },
    { "人民体育出版社", "", "" },
    { "武汉理工大学出版社", "", "" },
    { "经济管理出版社", "", "" },
    { "石油工业出版社", "", "" },
    { "大连理工大学出版社", "", "" },
    { "Attic Press", "http://ireland.iol.ie/~atticirl", "" },
    { "Curzon Press", "http://nias.ku.dk/curzonpress.html", "" },
    { "Science Press", "http://science-press.com", "" },
    { "Freedom Press",
            "http://web.cs.city.ac.uk/homes/louise/freehome.html", "" },
    { "Academic Press", "http://www.apnet.com", "" },
    { "Appletree Press", "http://www.appletree.ie", "" },
    { "Architectural Press", "http://www.architecturalpress.com", "" },
    { "Arnold Publishers", "http://www.arnoldpublishers.com", "" },
    { "Finder Press", "http://www.authenticfood.com", "" },
    { "Addison Wesley Longman", "http://www.awl.co.uk", "" },
    { "Bentham Press", "http://www.bentham.com", "" },
    { "Berg Publishers", "http://www.berg.demon.co.uk", "" },
    { "Berlitz Publishing", "http://www.berlitz.com", "" },
    { "Blackstone Press", "http://www.blackstonepress.com", "" },
    { "Boydell & Brewer", "http://www.boydell.co.uk", "" },
    { "Broadview Press", "http://www.broadviewpress.com", "" },
    { "Carcanet Press", "http://www.carcanet.co.uk", "" },
    { "Carfax Publishing", "http://www.carfax.co.uk/index.htm", "" },
    { "Cicerone Press", "http://www.cicerone.demon.co.uk", "" },
    { "CRC Press", "http://www.crcpress.com", "" },
    { "Elsevier Science", "http://www.elsevier.nl", "" },
    { "Hillside Publishing", "http://www.elthillside.com", "" },
    { "Erasmus Publishing", "http://www.erasmuspublishing.net", "" },
    { "Express Publishing", "http://www.expresspublishing.co.uk", "" },
    { "Voyager", "http://www.fireandwater.com/imprints/voyager", "" },
    { "Gateway Books", "http://www.gatewaybooks.com", "" },
    { "Gill & Macmillan", "http://www.gillmacmillan.ie", "" },
    { "Gladiolus Press", "http://www.gladioluspress.clara.net", "" },
    { "Globefield Press", "http://www.globefield.com", "" },
    { "Gomer Press", "http://www.gomer.co.uk", "" },
    { "Gower Publishing", "http://www.gowerpub.com", "" },
    { "Gulf Publishing", "http://www.gulfpub.com/books.html", "" },
    { "Harvard University Press", "http://www.hup.harvard.edu", "" },
    { "Irish Academic Press", "http://www.iap.ie", "" },
    { "Imperial College Press", "http://www.icpress.demon.co.uk", "" },
    { "Institute of Physics Publishing", "http://www.iop.org", "" },
    { "Jessica Kingsley Publishers", "http://www.jkp.com", "" },
    { "John Jones Publishing", "http://www.johnjonespublishing.ltd.uk",
            "" },
    { "Jordan Publishing", "http://www.jordanpublishing.co.uk", "" },
    { "Mercat Press", "http://www.jthin.co.uk/merchome.htm", "" },
    { "Lion Publishing", "http://www.lion-publishing.co.uk", "" },
    { "Long Barn Books", "http://www.longbarnbooks.co.uk", "" },
    { "Luath Press", "http://www.luath.co.uk", "" },
    { "Lutterworth Press", "http://www.lutterworth.com", "" },
    { "Maney Publishing", "http://www.maney.co.uk", "" },
    { "Marshall Publishing", "http://www.marshallpublishing.com", "" },
    { "Johnson Matthey", "http://www.matthey.com", "" },
    { "Amulree Publications", "http://www.mcb.net/amulree", "" },
    { "McGraw Hill", "http://www.mcgraw-hill.co.uk", "" },
    { "Military Press", "http://www.militarypress.co.uk", "" },
    { "Minerva Press", "http://www.minerva-press.co.uk", "" },
    { "Mulberry Press", "http://www.mulberrybush.com", "" },
    { "O'Brien Press", "http://www.obrien.ie", "" },
    { "Octagon Press", "http://www.octagonpress.com", "" },
    { "Omnibus Press", "http://www.omnibuspress.com", "" },
    { "O'Reilly", "http://www.oreilly.com", "" },
    { "Oxford University Press", "http://www.oup.co.uk", "" },
    { "Pluto Press", "http://www.plutobooks.com", "" },
    { "Polity Press", "http://www.polity.co.uk", "" },
    { "Crux Press", "http://www.press.mid-wales.net", "" },
    { "Sage Publications", "http://www.sagepub.co.uk", "" },
    { "Sigma Press", "http://www.sigmapress.co.uk", "" },
    { "Stockton Press", "http://www.stockton-press.co.uk", "" },
    { "Sybex", "http://www.sybex.co.uk", "" },
    { "Thistle Press", "http://www.thistlepress.co.uk", "" },
    { "Thoemmes Press", "http://www.thoemmes.com", "" },
    { "Weston Publishing", "http://www.weston-publishing.co.uk", "" },
    { "Westzone Publishing", "http://www.westzonepublishing.com", "" },
    { "John Wiley & Sons", "http://www.wiley.co.uk", "" },
    { "WIT Press", "http://www.witpress.com", "" },
    { "Wrox", "http://www.wrox.co.uk", "" },
    { "Yale University Press", "http://www.yale.edu/yup", "" },
    { "Zero Press", "http://www.zero.dircon.co.uk", "" },

    { NULL, NULL, NULL },
};

int gbs_publisher_default_init(void)
{
    gbs_publisher_name_t *pub;

    for (pub = g_default_publishers; pub->publisher; pub++) {
        gbs_publisher_insert((char *)pub->publisher, (char *)pub->website, (char *)pub->description);
    }

    return 0;
}

int gbs_publisher_foreach_write_db(sqlite3 *db, int (*insert)(sqlite3 *db, gbs_publisher_t *data))
{
    int ret;
    gbs_publisher_t *pub;

    list_for_each_entry(pub, &g_publisher_list, node) {
        ret = insert(db, pub);
        if (ret < 0)
            return ret;
    }
    return 0;
}

int gbs_publisher_init(void)
{
    INIT_LIST_HEAD(&g_publisher_list);
    gbs_publisher_default_init();
    return 0;
}

void gbs_publisher_fini(void)
{
    gbs_publisher_t *pub, *next_publisher;

    list_for_each_entry_safe(pub, next_publisher, &g_publisher_list,
        node) {
        list_del(&pub->node);
        gbs_publisher_free(pub);
        g_publisher_cnt--;
    }

    return;
}
