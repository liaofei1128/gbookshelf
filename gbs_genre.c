#include "gbookshelf.h"

/**
 * gbs_genre table management
 */
dpa_t g_main_genres;
unsigned int g_genre_cnt;
struct list_head g_genre_list;
gbs_genre_t *g_genre_root = NULL;

static gbs_genre_name_t g_default_genres[] = {
    { "/Arts", "Architectural", "建筑设计|土木工程|Civil Engineering|Architectural" },
    { "/Arts", "Chinaware", "磁器|陶瓷|Ceramics|Chinaware" },
    { "/Arts", "Dance", "舞蹈|跳舞|芭蕾舞|现代舞|民族舞|迪斯科|街舞|歌舞|拉丁舞|踢踏舞|Ballet|Disco|Dance" },
    { "/Arts", "Movie", "电影|影评|影院|导演|表演|演员|Film|Cinema|Director|Movie" },
    { "/Arts", "Music", "音乐|吉他|钢琴|乐理|摇滚|管弦乐|爵士乐|布鲁斯|乐队|节奏|交响乐|嘻哈|Musicology|Guita|Piano|Rock & Roll|Jazz|Orchestral|Blues|Rhythm|Symphony|Hip-Hop|Music" },
    { "/Arts", "Opera", "歌剧|越剧|粤剧|戏曲|京剧|戏剧|昆曲|Drama|Opera" },
    { "/Arts", "Painting", "绘画|油画|画家|名画|画画|Draw|Painting" },
    { "/Arts", "Photography", "摄影|照相|拍照|Photography" },
    { "/Arts", "Sculpture", "雕塑|雕像|雕刻|雕塑|蜡人|Sculpture" },

    { "/Information Techology/Application", "Office", "OpenOffice|XPS|iWork|Word|Excel|PowerPoint|Visio|办公软件" },
    { "/Information Techology/Application", "System Tutorial", "操作|手册|指南|Manual|Tutorial|Cookbook" },
    { "/Information Techology/Application", "Model Design", "AutoCAD|Autodesk|3DSMax|3DS Max|3D Studio MAX|Maya|UG|Zbrush|Alias|Rhino|Solidworks|PROE|CATIA|sketchup|3D建模" },
    { "/Information Techology/Application", "Multimedia Design", "Macromedia|Authorware|Photoshop|Illustrator|After Effects|Fireworks|Flash|多媒体设计|平面设计|界面设计" },

    { "/Information Techology/Hardware", "CPU", "Instruction|指令|X86|PowerPC|ARM|MIPS|Phytium|Loongson|Freescale|Ingenic|Tilegx|Octeon|Cavium" },
    { "/Information Techology/Hardware", "Micro System", "MST|MicroSystem|NanoSystem|SOC|微系统|纳系统|微处理|微控制|单片机|MEMS|ASIC|OpenWrt|eCos|WindRiver|RTOS|Vxworks|MSP430|STM32||MCU|Freeswitch|OpenRISC|Keil|uVision|CodeWarrior" },
    { "/Information Techology/Hardware", "Hardware Design", "Hardware| AC-DC|硬件|电路|模拟电子|数字电子|微机原理|时钟|晶振|电源|电阻|电容|电感|磁珠|二极管|三极管|MOS管|变压器|光耦|继电器|连接器|示波器|Altium|Cadence|protel|Allegro" },
    { "/Information Techology/Hardware", "Integrated Circuit", "IC|Circuit|PCB|PCI|PCIE|POE|Datasheet|uart|I2C|SPI|GPIO|LCD|LocalBus|ZegBee|JTAG|RJ45|SFP|XFP|EEPROM|Flash|SDRAM|DDR|SERDES|PHY|MII|SGMI|RGMI|XUAI" },
    { "/Information Techology/Hardware", "Hardware Programming", "CPLD|FPGA|Verilog|VHDL|Quartus|ModelSim|Altera|Xilinx|Mentor|Primace|Vivado" },

    { "/Information Techology/Software", "Actionscript Language", "Actionscript|FLEX|Flash MX|Flash Builder|AIR" },
    { "/Information Techology/Software", "Algorithm", "Algorithm|ACM|ICPC|算法|程序设计竞赛" },
    { "/Information Techology/Software", "ASP Language", "ASP|ASP.net" },
    { "/Information Techology/Software", "Assembly Language", "Assembly|汇编程序|汇编语言" },
    { "/Information Techology/Software", "Bash script", "Bash" },
    { "/Information Techology/Software", "C Language", "C" },
    { "/Information Techology/Software", "C# Language", "C#|CSharp|C Sharp|\\.net|dotnet" },
    { "/Information Techology/Software", "C++ Language", "C++|CPP|C Plus Plus|Cplusplus||Visual C++|VC|VC++" },
    { "/Information Techology/Software", "CSS Language", "CSS" },
    { "/Information Techology/Software", "Data Structure", "数据结构" },
    { "/Information Techology/Software", "Delphi Language", "Delphi" },
    { "/Information Techology/Software", "D Language", "D" },
    { "/Information Techology/Software", "Fortran Language", "Fortran" },
    { "/Information Techology/Software", "Go Language", "Go" },
    { "/Information Techology/Software", "TeX Language", "TeX|LaTex|LuaTeX|pdfTeX" },
    { "/Information Techology/Software", "XML Language", "XML" },
    { "/Information Techology/Software", "Java Language", "Java" },
    { "/Information Techology/Software", "Javascript programming", "Javascript|JS" },
    { "/Information Techology/Software", "UML engining", "UML" },
    { "/Information Techology/Software", "Pascal Language", "Pascal" },
    { "/Information Techology/Software", "Perl script", "Perl" },
    { "/Information Techology/Software", "PHP Language", "PHP" },
    { "/Information Techology/Software", "PostScript Language", "Postscript" },
    { "/Information Techology/Software", "Python Language", "Python" },
    { "/Information Techology/Software", "Ruby Language", "Ruby" },
    { "/Information Techology/Software", "Open Source", "Opensource|开源软件" },
    { "/Information Techology/Software", "Compiler Principle", "Compiler|编译" },
    { "/Information Techology/Software", "Game Programming", "Game .* Programming|Game Design|游戏编程|游戏设计" },
    { "/Information Techology/Software", "Labview Programming", "Labview|System-C" },
    { "/Information Techology/Software", "GTK Programming", "GTK|GNOME" },
    { "/Information Techology/Software", "MFC Programming", "MFC" },
    { "/Information Techology/Software", "Matlab Programming", "Matlab" },
    { "/Information Techology/Software", "Mathematica Programming", "Mathematica" },
    { "/Information Techology/Software", "Linux Programming", "Linux .* Programming|Linux.*编程|Linux.*内核编程|Linux Kernel|Linux应用程序|Linux程序设计|Linux软件开发|Linux .* Driver" },
    { "/Information Techology/Software", "Windows Programming", "Windows .* Programming|Windows.*编程|Window.*内核编程|Windows Kernel|Windows应用程序|Windows程序设计|Windows软件开发" },
    { "/Information Techology/Software", "iOS Programming", "MacOS .* Programming|iOS .* Programming|iPhone Programming|iOS Application|iOS程序|iPhone应用|AppStore|xCode|Object-C" },
    { "/Information Techology/Software", "Android Programming", "Android .* Programming|Android开发|Android程序|Android软件|NDK|Android Studio" },
    { "/Information Techology/Software", "MeeGo Programming", "MeeGo" },
    { "/Information Techology/Software", "Symbian Programming", "Symbian" },
    { "/Information Techology/Software", "Network Programming", "Socket|协议栈|套接字|网络编程|Network Programming" },
    { "/Information Techology/Software", "Web Design", "Web|Website|Frontpage|HTML|AJAX|IIS|httpd|lighttpd|Restful|Rail|Apache|Ngix|CGI|MySQL|Dreamweaver|网页|网站" },
    { "/Information Techology/Software", "Software Security", "Virus|病毒|Exploit|Exploitation|Exploiting|Metasploit|渗透|木马|Trojan Horse|BackTrack|Malware|Hack|Hacker|Hacking|黑客|攻击|Attack|Crack|Defense|Buffer Overflow|缓冲区溢出|软件安全" },
    { "/Information Techology/Software", "Software Engineering", "Software Engineering|软件工程|Software Requirements|软件需求|Software Architecture|软件架构|架构设计|Software Design|软件设计|Software Development|软件开发|Software Testing|软件测试|测试方法|Software Maintenance|软件维护|Software Configuration|配置管理|Software Quality|软件质量" },

    { "/Information Techology/Network", "Network Principle", "网络原理|通信原理|计算机网络|局域网|物联网|IOT|Network Principle|Computer Network|Network Design|Communication|Wireless|无线|WiFI|GSM||GPRS|ZigBee|Bluetooth|蓝牙|NFC|近场通讯|RFID|射频识别|WiGig|DLNA" },
    { "/Information Techology/Network", "Network Security", "Firewall|IPSec|VPN|SSL|PKCS|ISAKMP|IKE|密钥交换协议|安全联盟|安全关联密钥管理协议|SSH|DPI|Snort|Sniffer|NIDS|L7filter|nDPI|nTOP|Suricata|Secure|Security|网络攻防|协议分析|深度协议解析|注入|网络安全" },
    { "/Information Techology/Network", "Network Protocol", "ICMP|IGMP|DHCP|ARP|IP|TCP|UDP|MPLS|IPV6|802.11|FTP|HTTP|HTTPS|FCP|iSCSI|IRC|Kerberos|LDAP|MPLS|MTP|SMTP|NetBIOS|NetFlow|NFS|NTP|OSPF|PPP|PPPoE|RPC|RTP|SFTP|SIP|SNMP|SOAP|STP|STUN|NAT|Telnet|TFTP|TLS|WAP|Samba|VOIP|协议解析" },
    { "/Information Techology/Network", "Cisco Network", "CISCO|CCNA|CCNE|CCNP|CCDA|CCDE|CCDP" },

    { "/Information Techology", "Digital Signal Processing", "DSP|Digital Signal Processing|数字信号处理|信号与系统|数字滤波|滤波器|Digital Filter" },
    { "/Information Techology", "Image Processing", "Image Processing|Image Compress|图像处理|图像压缩|图像变换|图像增强|图像编码|图像识别" },
    { "/Information Techology", "Data Compression", "Data Compression|数据压缩|压缩算法|LZW|BZIP|BZ2|7Z|LZMA|无损压缩|有损压缩" },
    { "/Information Techology", "Data Encryption", "Data Encrypt|Data Decrypt|Data Crypt|数据加密|数据解密|数据加解密|加解密算法|加密算法|解密算法" },
    { "/Information Techology", "Artificial Intelligence", "Artificial Intelligence|AI|Data Mining|Web Mining|Robot|人工智能|数据挖掘|网页挖掘|机器人|模式识别|TensorFlow|语音识别|NLP|自然语言处理|文字识别|OCR" },

    { "/Economics", "Behavioural Economics", "" },
    { "/Economics", "Computational Economics", "" },
    { "/Economics", "Econometrics", "" },
    { "/Economics", "Financial Economics", "" },
    { "/Economics", "Industrial Organization", "" },
    { "/Economics", "International Economics", "" },
    { "/Economics", "Law and Economics", "" },
    { "/Economics", "Macroeconomics", "" },
    { "/Economics", "Mathematical Economics", "" },
    { "/Economics", "Microeconomics", "" },
    { "/Economics", "Monetary Economics", "" },
    { "/Economics", "Political Economics", "" },
    { "/Economics", "Regional Science", "" },
    { "/Economics", "Socialist Economics", "" },
    { "/Economics", "Accounting", "会计|账务|财务" },
    { "/Economics", "Revenue", "Tax|税收" },

    { "/Foreian Language", "Chinese", "" },
    { "/Foreian Language", "English", "" },
    { "/Foreian Language", "French", "" },
    { "/Foreian Language", "Japanese", "" },
    { "/Foreian Language", "Korea", "" },
    { "/Foreian Language", "Russian", "" },
    { "/Foreian Language", "Greek", "" },

    { "/Mathematical", "Combinatorics", "组合数学|Combinatorics" },
    { "/Mathematical", "Discrete Mathematics", "离散数学|Discrete Mathematics" },
    { "/Mathematical", "Elementary Algebra", "初等代数|Elementary Algebra" },
    { "/Mathematical", "Graph Theory", "图论|Graph Theory" },
    { "/Mathematical", "Number Theory", "数论|Number Theory" },
    { "/Mathematical", "Algebraic Geometry", "代数几何|Algebraic Geometry" },
    { "/Mathematical", "Analytic Geometry", "解析几何|Analytic Geometry" },
    { "/Mathematical", "Computational Geometry", "组合几何|Computational Geometry" },
    { "/Mathematical", "Descriptive Geometry", "画法几何" },
    { "/Mathematical", "Differential Geometry", "微分几何" },
    { "/Mathematical", "Fractal Geometry", "分形几何" },
    { "/Mathematical", "Plane Geometry", "平面几何" },
    { "/Mathematical", "Projection Geometry", "射影几何" },
    { "/Mathematical", "Riemannian Geometry", "黎曼几何" },
    { "/Mathematical", "Solid Geometry", "立体几何" },
    { "/Mathematical", "Calculus of Variations", "变分法" },
    { "/Mathematical", "Complex Analysis", "复分析" },
    { "/Mathematical", "Computable Analysis", "" },
    { "/Mathematical", "Differential Equation", "微分方程" },
    { "/Mathematical", "Partial Differential Equation", "偏微分方程" },
    { "/Mathematical", "Fourier Analysis", "傅里叶分析" },
    { "/Mathematical", "Functional Analysis", "泛函分析" },
    { "/Mathematical", "Harmonic Analysis", "调和分析" },
    { "/Mathematical", "Measure Theory", "集合论|测度论|Set Theory|Measure Theory" },
    { "/Mathematical", "Numerical Analysis", "数值分析|Numerical Analysis" },
    { "/Mathematical", "Real Analysis", "实分析|Real Analysis" },
    { "/Mathematical", "Mathematical Analysis", "数学分析|微积分|微分|积分|Mathematical Analysis|Calculus" },
    { "/Mathematical", "Tropical Analysis", "" },
    { "/Mathematical", "Vector Calculus", "" },
    { "/Mathematical", "Wavelet Analysis", "" },
    { "/Mathematical", "Mathematical Olympiad", "" },

    { "/Medical", "Nurses", "" },
    { "/Medical", "First Aid", "" },
    { "/Medical", "Pharmacists", "" },
    { "/Medical", "Physiotherapists", "" },
    { "/Medical", "Respiratory Therapists", "" },
    { "/Medical", "Speech Therapists", "" },
    { "/Medical", "Occupational Therapists", "" },
    { "/Medical", "Radiographers", "" },
    { "/Medical", "Dietitians and Bioengineers", "" },

    { "/Chemistry", "Analytical Chemistry", "" },
    { "/Chemistry", "Biochemistry", "" },
    { "/Chemistry", "Inorganic Chemistry", "" },
    { "/Chemistry", "Materials Chemistry", "" },
    { "/Chemistry", "Neurochemistry", "" },
    { "/Chemistry", "Nuclear Chemistry", "" },
    { "/Chemistry", "Organic Chemistry", "" },
    { "/Chemistry", "Physical Chemistry", "" },
    { "/Chemistry", "Theoretical Chemistry", "" },

    { "/Biology", "Aerobiology", "" },
    { "/Biology", "Agriculture", "" },
    { "/Biology", "Anatomy", "" },
    { "/Biology", "Astrobiology", "" },
    { "/Biology", "Biochemistry", "" },
    { "/Biology", "Bioengineering", "" },
    { "/Biology", "Bioinformatics", "" },
    { "/Biology", "Biomathematics", "" },
    { "/Biology", "Biomechanics", "" },
    { "/Biology", "Biomedical", "" },
    { "/Biology", "Biophysics", "" },
    { "/Biology", "Biotechnology", "" },
    { "/Biology", "Building Biology", "" },
    { "/Biology", "Botany", "" },
    { "/Biology", "Cell Biology", "" },
    { "/Biology", "Conservation Biology", "" },
    { "/Biology", "Cryobiology", "" },
    { "/Biology", "Developmental Biology", "" },
    { "/Biology", "Ecology", "" },
    { "/Biology", "Embryology", "" },
    { "/Biology", "Entomology", "" },
    { "/Biology", "Environmental Biology", "" },
    { "/Biology", "Epidemiology", "" },
    { "/Biology", "Ethology", "" },
    { "/Biology", "Evolutionary Biology", "" },
    { "/Biology", "Genetics", "" },
    { "/Biology", "Herpetology", "" },
    { "/Biology", "Histology", "" },
    { "/Biology", "Ichthyology", "" },
    { "/Biology", "Integrative biology", "" },
    { "/Biology", "Limnology", "" },
    { "/Biology", "Mammalogy", "" },
    { "/Biology", "Marine Biology", "" },
    { "/Biology", "Microbiology", "" },
    { "/Biology", "Molecular Biology", "" },
    { "/Biology", "Mycology", "" },
    { "/Biology", "Neurobiology", "" },
    { "/Biology", "Oceanography", "" },
    { "/Biology", "Oncology", "" },
    { "/Biology", "Ornithology", "" },
    { "/Biology", "Population biology", "" },
    { "/Biology", "Population ecology", "" },
    { "/Biology", "Population genetics", "" },
    { "/Biology", "Paleontology", "" },
    { "/Biology", "Pathobiology", "" },
    { "/Biology", "Parasitology", "" },
    { "/Biology", "Pharmacology", "" },
    { "/Biology", "Physiology", "" },
    { "/Biology", "Phytopathology", "" },
    { "/Biology", "Psychobiology", "" },
    { "/Biology", "Sociobiology", "" },
    { "/Biology", "Structural Biology", "" },
    { "/Biology", "Virology", "" },
    { "/Biology", "Zoology", "" },

    { "/Philosophy", "Aesthetics", "" },
    { "/Philosophy", "Epistemology", "" },
    { "/Philosophy", "Ethics", "" },
    { "/Philosophy", "Logic", "" },
    { "/Philosophy", "Metaphysics", "" },
    { "/Philosophy", "Social Philosophy", "" },
    { "/Philosophy", "Political Philosophy", "" },

    { "/Physics", "Classical Mechanics", "" },
    { "/Physics", "Electromagnetism", "" },
    { "/Physics", "Statistical Mechanics", "" },
    { "/Physics", "Thermodynamics", "" },
    { "/Physics", "Quantum Mechanics", "" },
    { "/Physics", "Relativity", "" },

    { "/Livelihood", "Cooking", "" },
    { "/Livelihood", "Game", "" },
    { "/Livelihood", "Snack", "" },
    { "/Livelihood", "Sport", "" },
    { "/Livelihood", "Travel", "" },
    { "/Livelihood", "Handicraft", "手艺|手工|折纸|插花|编织|Handcraft|Origami|Flower Arrangement|Knitting" },

    { "/Literature", "Fiction", "小说|倾城之恋|飘|基督山伯爵|三个火枪手|傲慢与偏见|漂亮朋友|项链|米隆老爹|羊脂球|我的叔叔于勒|万卡|第六病室|变色龙|跳来跳去的女人|麦琪的礼物|二十年后|警察和赞美诗|财神和爱神|红毛酉长的赎金|十日谈|堂吉珂德|简爱|呼啸山庄|新月集|飞鸟集|吉檀迦利|约翰克里斯朵夫|红与黑|欧也妮葛朗台|第十二夜|仲夏夜之梦|威尼斯商人|欢天喜地|麦克白|李尔王|哈姆雷特|奥塞罗|罗密欧与朱莉叶|安娜卡列尼娜|平凡的世界"},
    { "/Literature", "Science Fiction", "Science Fiction|科幻|三体|基地|时间机器|星际旅行"},
    { "/Literature", "Detective Story", "Detective|Sherlock Holmes|侦探|福尔摩斯|柯南道尔|阿加莎克里斯蒂|Mystery Story|推理小说"},
    { "/Literature", "Poetry", "诗集|诗歌|古诗|史诗|诗情|画意|唐诗|宋词|元曲|Poetry" },

    { "/History", "Chinese History", "中国.*历史|中国.*简史|汉书|中国.*通史|中国.*社会史|上下五千年|二十四史|史记|汉书|后汉书|三国志|晋书|宋书|南齐书|梁书|陈书|魏书|北齐书|周书|隋书|南史|北史|旧唐书|新唐书|旧五代史|新五代史|宋史|辽史|金史|元史|明史|春秋|史记|资治通鉴|三国志|左传"},
    { "/History", "World History", "世界历史|世界通史|美国历史|德国历史|俄国历史"},

    { "/Others", "" },

    { NULL, NULL, NULL },
};

gbs_genre_t *gbs_genre_alloc(void)
{
    gbs_genre_t *genre = malloc(sizeof(gbs_genre_t));
    if (genre == NULL)
        return NULL;

    memset(genre, 0, sizeof(gbs_genre_t));
    return genre;
}

void gbs_genre_free(gbs_genre_t * genre)
{
    if (genre) {
        mbsfree(genre->path);
        mbsfree(genre->genre);
        mbsfree(genre->parent);
        mbsfree(genre->fullpath);
        mbsfree(genre->keywords);
        dfa_destroy(genre->pattern);
        free(genre);
    }
}

int gbs_genre_insert(char *path, char *genre, char *keywords)
{
    char *dirname = NULL;
    gbs_genre_t *gen = gbs_genre_alloc();
    if (gen == NULL)
        return -GBS_ERROR_NOMEM;

    gen->path = mbsnewescapesqlite(path);
    gen->genre = mbsnewescapesqlite(genre);
    parse_dirname(gen->path, &dirname);
    gen->parent = mbsnew(dirname);
    free(dirname);
    gen->fullpath = NULL;
    mbscatfmt(&gen->fullpath, "%s/%s", gen->path, gen->genre);
    gen->keywords = mbsnewescapesqlite(keywords);
    gen->pattern = dfa_compile(keywords, REF_IGNORECASE);
    list_add_tail(&gen->node, &g_genre_list);
    dpa_append(&g_main_genres, mbsdup(gen->path), dpa_str_cmp, NULL);
    dpa_append(&g_main_genres, mbsdup(gen->parent), dpa_str_cmp, NULL);
    dpa_append(&g_main_genres, mbsdup(gen->fullpath), dpa_str_cmp, NULL);
    g_genre_cnt++;
    return 0;
}

int gbs_genre_delete(char *genre)
{
    gbs_genre_t *cur_genre, *next_genre;

    list_for_each_entry_safe(cur_genre, next_genre, &g_genre_list, node) {
        if (!strcmp(cur_genre->genre, genre)) {
            list_del(&cur_genre->node);
            gbs_genre_free(cur_genre);
            g_genre_cnt--;
            return 0;
        }
    }

    return GBS_ERROR_NOT_EXIST;
}

int gbs_genre_default_init(void)
{
    gbs_genre_name_t *gname;

    for (gname = g_default_genres; gname->genre; gname++) {
        gbs_genre_insert(gname->path, gname->genre, gname->keywords);
    }

    return 0;
}

void gbs_genre_dump(void)
{
    gbs_genre_t *genre;
    list_for_each_entry(genre, &g_genre_list, node) {
        printf("genre %s/%s:%s\n", genre->path, genre->genre, genre->keywords);
    }
}

int gbs_genre_foreach_write_db(sqlite3 *db, int (*insert)(sqlite3 *db, gbs_genre_t *data))
{
    int ret;
    gbs_genre_t *genre;

    list_for_each_entry(genre, &g_genre_list, node) {
        ret = insert(db, genre);
        if (ret < 0)
            return ret;
    }
    return 0;
}

int gbs_genre_init(void)
{
    INIT_LIST_HEAD(&g_genre_list);
    dpa_init(&g_main_genres, 40);
    //gbs_genre_default_init();
    return 0;
}

void gbs_genre_fini(void)
{
    char *name;
    gbs_genre_t *cur_genre, *next_genre;

    while (1) {
        name = dpa_pop(&g_main_genres);
        if (!name) {
            break;
        }
        mbsfree(name);
    }

    list_for_each_entry_safe(cur_genre, next_genre, &g_genre_list, node) {
        list_del(&cur_genre->node);
        gbs_genre_free(cur_genre);
    }

    g_genre_cnt = 0;
    return;
}
