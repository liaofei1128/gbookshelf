/*
 * MBS: memory buffer string library.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "libmbs.h"
#include "libtypes.h"

enum {
    MBS_ALLOC_BY_CACHE_32 = 0,
    MBS_ALLOC_BY_CACHE_64,
    MBS_ALLOC_BY_CACHE_128,
    MBS_ALLOC_BY_CACHE_256,
    MBS_ALLOC_BY_CACHE_512,
    MBS_ALLOC_BY_CACHE_1024,
    MBS_ALLOC_BY_CACHE_2048,
    MBS_ALLOC_BY_CACHE_4096,
    MBS_ALLOC_BY_CACHE_8192,
    MBS_ALLOC_BY_CACHE_10240,

    MBS_ALLOC_BY_MALLOC
};

int mbsinit(int cnt, ...)
{
    return 0;
}

void mbsfini(void)
{
}

mbs_t mbsalloc(int size)
{
    uint8_t type;
    mbs_t mbs = NULL;
    mbs_hdr_t *hdr = NULL;

    int totalsize = MBSHDRSIZE + size + 1;

    hdr = malloc(totalsize);
    if (hdr) {
        type = MBS_ALLOC_BY_MALLOC;
        goto out;
    }

    return NULL;

out:
    hdr->len = 0;
    hdr->size = size;
    hdr->type = type;

    mbs = (char *)hdr + MBSHDRSIZE;
    mbs[hdr->len] = '\0';
    return mbs;
}

void mbsfree(mbs_t mbs)
{
    if (mbs) {
        mbs_hdr_t *hdr = MBSHDR(mbs);
        switch (hdr->type) {
        case MBS_ALLOC_BY_MALLOC:
            free(hdr);
            break;
        default:
            break;
        }
    }
}

/*
 * If mbsrealloc() fails the original block is left untouched,
 * it is not freed or moved.
 */
mbs_t mbsrealloc(mbs_t mbs, int size)
{
    int totalsize = 0;
    mbs_hdr_t *ohdr = NULL;
    mbs_hdr_t *nhdr = NULL;
    mbs_t nmbs = NULL;

    if (mbs == NULL) {
        return mbsalloc(size);
    }

    if (size == 0) {
        mbsfree(mbs);
        return NULL;
    }

    // exceed the max size
    if (size > 0x3FFFFFFF) {
        return NULL;
    }

    ohdr = MBSHDR(mbs);
    if (ohdr->size >= size) {
        ohdr->size = size;
        mbs[size] = '\0';
        return mbs;
    }

    totalsize = MBSHDRSIZE + size + 1;

    if (ohdr->type == MBS_ALLOC_BY_MALLOC) {
        nhdr = realloc(ohdr, totalsize);
        if (nhdr == NULL) {
            return NULL;
        }

        nhdr->size = size;
        nmbs = (char *)nhdr + MBSHDRSIZE;
        return nmbs;
    }

    return NULL;
}

mbs_t mbsnewsize(int size)
{
    return mbsalloc(size);
}

mbs_t mbsnewlen(char *str, int len)
{
    mbs_t mbs = NULL;

    mbs = mbsalloc(len);
    if (mbs == NULL) {
        return NULL;
    }

    if (str && len) {
        mbs_hdr_t *hdr = MBSHDR(mbs);

        hdr->len = len;
        memcpy(mbs, str, len);
        mbs[len] = '\0';
    }

    return mbs;
}

mbs_t mbsempty(void)
{
    return mbsnewlen("", 0);
}

mbs_t mbsnew(char *str)
{
    if (str) {
        return mbsnewlen(str, strlen(str));
    } else {
        return mbsnewlen("", 0);
    }
}

mbs_t mbsnewx(char *xstr, char **endptr)
{
    char l, h;
    char *p = xstr;
    mbs_t ret = mbsempty();

    while (1) {
        if (p == NULL || *p == '\0')
            break;

        h = (*p >= '0' && *p <= '9') ? (*p - '0') :
            ((*p >= 'a' && *p <= 'f') ? (*p - 'a' + 10) :
             ((*p >= 'A' && *p <= 'F') ? (*p - 'A' + 10) : 0x10));
        if (h == 0x10) {
            if (endptr) *endptr = p;
            mbsfree(ret);
            return NULL;
        }

        p++;
        if (*p == '\0') {
            if (endptr) *endptr = p;
            mbsfree(ret);
            return NULL;
        }

        l = (*p >= '0' && *p <= '9') ? (*p - '0') :
            ((*p >= 'a' && *p <= 'f') ? (*p - 'a' + 10) :
             ((*p >= 'A' && *p <= 'F') ? (*p - 'A' + 10) : 0x10));

        if (l == 0x10) {
            if (endptr) *endptr = p;
            mbsfree(ret);
            return NULL;
        }

        mbscatchar(&ret, (h << 4) | l);
        p++;
    }

    if (endptr) *endptr = p;
    return ret;
}

mbs_t mbsnewvfmt(char *fmt, va_list ap)
{
    mbs_t nmbs;

    int len = 0;
    char *buf = NULL;

    len = vasprintf(&buf, fmt, ap);
    if (len < 0) {
        return NULL;
    }

    nmbs = mbsnewlen(buf, len);
    free(buf);
    return nmbs;
}

mbs_t mbsnewfmt(char *fmt, ...)
{
    mbs_t nmbs;
    va_list ap;

    va_start(ap, fmt);
    nmbs = mbsnewvfmt(fmt, ap);
    va_end(ap);

    return nmbs;
}

mbs_t mbsnewtime(time_t time)
{
    char buf[128] = {0, };

    if (time == 0) {
        return mbsnew("N/A");
    }

    struct tm *t = localtime(&time);
    strftime(buf, 32, "%Y-%m-%d %H:%M:%S", t);

    return mbsnew(buf);
}


mbs_t mbsdup(mbs_t mbs)
{
    return mbsnewlen(mbs, mbslen(mbs));
}

int mbssetlen(mbs_t mbs, int len)
{
    mbs_hdr_t *hdr = MBSHDR(mbs);

    if (len > hdr->size) {
        return -EOVERFLOW;
    }

    hdr->len = len;
    return 0;
}

int mbsinclen(mbs_t mbs, int inclen)
{
    mbs_hdr_t *hdr = MBSHDR(mbs);

    if (hdr->len + inclen > hdr->size || hdr->len + inclen < 0) {
        return -EOVERFLOW;
    }

    hdr->len = hdr->len + inclen;
    mbs[hdr->len] = '\0';
    return 0;
}

// Note: if you want to sort string, please use strcmp instead.
// This function just use to check if they are equal.
int mbscmp(mbs_t s1, mbs_t s2)
{
    if (s1 == NULL || s2 == NULL) {
        return 1;
    }

    if (mbslen(s1) != mbslen(s2)) {
        return 1;
    }

    return strcmp(s1, s2);
}

// Note: if you want to sort string, please use strcasecmp instead.
// This function just use to check if they are equal without case.
int mbscasecmp(mbs_t s1, mbs_t s2)
{
    if (s1 == NULL || s2 == NULL) {
        return 1;
    }

    if (mbslen(s1) != mbslen(s2)) {
        return 1;
    }

    return strcasecmp(s1, s2);
}

mbs_t mbsclear(mbs_t mbs)
{
    mbssetlen(mbs, 0);
    mbs[0] = '\0';

    return mbs;
}

mbs_t mbsexpand(mbs_t *pmbs, int inclen)
{
    if (pmbs == NULL) {
        return NULL;
    }

    mbs_t mbs = *pmbs;
    if (mbs == NULL) {
        mbs = mbsnewsize(inclen);
        *pmbs = mbs;
        return mbs;
    }

    mbs_hdr_t *hdr = MBSHDR(mbs);
    mbs_t nmbs = mbsrealloc(mbs, hdr->size + inclen);
    if (nmbs == NULL) {
        return NULL;
    }

    *pmbs = nmbs;
    return nmbs;
}

mbs_t mbsexpandto(mbs_t *pmbs, int destlen)
{
    mbs_t mbs;
    int len = 0;

    if (pmbs == NULL) {
        return NULL;
    }

    mbs = *pmbs;
    if (mbs == NULL) {
        mbs = mbsnewsize(destlen);
        *pmbs = mbs;
        return mbs;
    }

    len = mbslen(mbs);

    if (destlen <= len) {
        return mbs;
    }

    return mbsexpand(pmbs, destlen - len);
}

mbs_t mbscpylen(mbs_t *pmbs, char *s, int len)
{
    mbs_t mbs;
    mbs_t nmbs;
    int size = 0;

    if (pmbs == NULL) {
        return NULL;
    }

    mbs = nmbs = *pmbs;
    if (mbs == NULL) {
        mbs = mbsnewlen(s, len);
        *pmbs = mbs;
        return mbs;
    }

    size = mbssize(mbs);

    if (size < len) {
        nmbs = mbsexpand(pmbs, len - size);
        if (nmbs == NULL) {
            return NULL;
        }
    }

    memcpy(nmbs, s, len);
    mbssetlen(nmbs, len);
    nmbs[len] = '\0';
    return nmbs;
}

mbs_t mbscpy(mbs_t *pmbs, char *s)
{
    return mbscpylen(pmbs, s, strlen(s));
}

mbs_t mbscpyvfmt(mbs_t *pmbs, char *fmt, va_list ap)
{
    mbs_t nmbs;

    int len = 0;
    char *buf = NULL;

    len = vasprintf(&buf, fmt, ap);
    if (len < 0) {
        return NULL;
    }

    nmbs = mbscpylen(pmbs, buf, len);
    free(buf);
    return nmbs;
}

mbs_t mbscpyfmt(mbs_t *pmbs, char *fmt, ...)
{
    mbs_t nmbs;
    va_list ap;

    va_start(ap, fmt);
    nmbs = mbscpyvfmt(pmbs, fmt, ap);
    va_end(ap);

    return nmbs;
}

mbs_t mbscatlen(mbs_t *pmbs, char *s, int len)
{
    mbs_t mbs;
    int curlen;

    if (pmbs == NULL) {
        return NULL;
    }

    mbs = *pmbs;
    if (mbs == NULL) {
        mbs = mbsnewlen(s, len);
        *pmbs = mbs;
        return mbs;
    }

    curlen = mbslen(mbs);

    if (curlen + len > mbssize(mbs)) {
        mbs = mbsexpand(pmbs, len);
        if (mbs == NULL) {
            return NULL;
        }
    }

    memcpy(mbs + curlen, s, len);
    mbssetlen(mbs, curlen + len);
    mbs[curlen + len] = '\0';
    return mbs;
}

mbs_t mbscat(mbs_t *pmbs, char *s)
{
    return mbscatlen(pmbs, s, strlen(s));
}

mbs_t mbscatmbs(mbs_t *pmbs, mbs_t mbs)
{
    return mbscatlen(pmbs, mbs, mbslen(mbs));
}

mbs_t mbscatchar(mbs_t *pmbs, char c)
{
    char s[2] = {c, '\0'};

    return mbscatlen(pmbs, s, 1);
}

mbs_t mbscatvfmt(mbs_t *pmbs, const char *fmt, va_list ap)
{
    mbs_t nmbs;

    int len = 0;
    char *buf = NULL;

    len = vasprintf(&buf, fmt, ap);
    if (len < 0) {
        return NULL;
    }

    nmbs = mbscatlen(pmbs, buf, len);
    free(buf);
    return nmbs;
}

mbs_t mbscatfmt(mbs_t *pmbs, const char *fmt, ...)
{
    mbs_t nmbs;
    va_list ap;

    va_start(ap, fmt);
    nmbs = mbscatvfmt(pmbs, fmt, ap);
    va_end(ap);

    return nmbs;
}

mbs_t mbscatu8(mbs_t *pmbs, uint8_t u8)
{
    return mbscatfmt(pmbs, "%hhx", u8);
}

mbs_t mbscatu16(mbs_t *pmbs, uint16_t u16)
{
    return mbscatfmt(pmbs, "%04x", u16);
}

mbs_t mbscatu32(mbs_t *pmbs, uint32_t u32)
{
    return mbscatfmt(pmbs, "%08x", u32);
}

mbs_t mbscatu64(mbs_t *pmbs, uint64_t u64)
{
    return mbscatfmt(pmbs, "%"PRIx64, u64);
}

mbs_t mbscatU8(mbs_t *pmbs, uint8_t U8)
{
    return mbscatfmt(pmbs, "%02X", U8);
}

mbs_t mbscatU16(mbs_t *pmbs, uint16_t U16)
{
    return mbscatfmt(pmbs, "%04X", U16);
}

mbs_t mbscatU32(mbs_t *pmbs, uint32_t U32)
{
    return mbscatfmt(pmbs, "%08X", U32);
}

mbs_t mbscatU64(mbs_t *pmbs, uint64_t U64)
{
    return mbscatfmt(pmbs, "%"PRIX64, U64);
}

mbs_t mbscato(mbs_t *pmbs, uint8_t o)
{
    return mbscatlen(pmbs, (char *)&o, 1);
}

mbs_t mbscaths(mbs_t *pmbs, uint16_t hs)
{
    return mbscatlen(pmbs, (char *)&hs, 2);
}

mbs_t mbscathl(mbs_t *pmbs, uint32_t hl)
{
    return mbscatlen(pmbs, (char *)&hl, 4);
}

mbs_t mbscathll(mbs_t *pmbs, uint64_t hll)
{
    return mbscatlen(pmbs, (char *)&hll, 8);
}

mbs_t mbsaddlen(mbs_t *pmbs, char *s, int len)
{
    mbs_t mbs;
    int curlen;

    if (pmbs == NULL) {
        return NULL;
    }

    mbs = *pmbs;
    if (mbs == NULL) {
        mbs = mbsnewlen(s, len);
        *pmbs = mbs;
        return mbs;
    }

    curlen = mbslen(mbs);

    if (curlen + len + 1 > mbssize(mbs)) {
        mbs = mbsexpand(pmbs, len + 1);
        if (mbs == NULL) {
            return NULL;
        }
    }

    memcpy(mbs + curlen + 1, s, len);
    mbssetlen(mbs, curlen + 1 + len);
    mbs[curlen + 1 + len] = '\0';
    return mbs;
}

mbs_t mbsadd(mbs_t *pmbs, char *s)
{
    return mbsaddlen(pmbs, s, strlen(s));
}

mbs_t mbsaddmbs(mbs_t *pmbs, mbs_t mbs)
{
    return mbsaddlen(pmbs, mbs, mbslen(mbs));
}

mbs_t mbsaddvfmt(mbs_t *pmbs, const char *fmt, va_list ap)
{
    mbs_t nmbs;

    int len = 0;
    char *buf = NULL;

    len = vasprintf(&buf, fmt, ap);
    if (len < 0) {
        return NULL;
    }

    nmbs = mbsaddlen(pmbs, buf, len);
    free(buf);
    return nmbs;
}

mbs_t mbsaddfmt(mbs_t *pmbs, const char *fmt, ...)
{
    mbs_t nmbs;
    va_list ap;

    va_start(ap, fmt);
    nmbs = mbsaddvfmt(pmbs, fmt, ap);
    va_end(ap);

    return nmbs;
}

mbs_t mbsjoin(mbs_t *pmbs, int argc, char **argv, char *sep)
{
    int i;
    mbs_t nmbs = NULL;

    for (i=0; i<argc; i++) {
        nmbs = mbscat(pmbs, argv[i]);
        if (i != argc - 1) {
            nmbs = mbscat(pmbs, sep);
        }
    }

    return nmbs;
}

mbs_t mbssum(mbs_t *pmbs, int argc, char **argv)
{
    int i;
    mbs_t nmbs = NULL;

    for (i=0; i<argc; i++) {
        nmbs = mbsadd(pmbs, argv[i]);
        if (nmbs == NULL) {
            return NULL;
        }
    }

    return nmbs;
}

mbs_t mbstolower(mbs_t mbs)
{
    int i;
    int len = mbslen(mbs);

    for (i=0; i<len; i++) {
        mbs[i] = tolower(mbs[i]);
    }

    return mbs;
}

mbs_t mbstoupper(mbs_t mbs)
{
    int i;
    int len = mbslen(mbs);

    for (i=0; i<len; i++) {
        mbs[i] = toupper(mbs[i]);
    }

    return mbs;
}

mbs_t mbscapitalize(mbs_t mbs)
{
    int i;
    int len = mbslen(mbs);

    mbs[0] = toupper(mbs[0]);
    for (i=1; i<len; i++) {
        mbs[i] = tolower(mbs[i]);
    }

    return mbs;
}

mbs_t mbsrtrim(mbs_t mbs, char *cset)
{
    size_t len;
    char *start, *end, *sp, *ep;
    char *whitespaces = " \t\r\n";

    cset = cset ? : whitespaces;

    sp = start = mbs;
    ep = end = mbs + mbslen(mbs) - 1;

    while(ep > sp && strchr(cset, *ep)) ep--;
    len = (sp > ep) ? 0 : ((ep-sp) + 1);
    mbs[len] = '\0';
    mbssetlen(mbs, len);
    return mbs;
}

mbs_t mbsltrim(mbs_t mbs, char *cset)
{
    size_t len;
    char *start, *end, *sp, *ep;
    char *whitespaces = " \t\r\n";

    cset = cset ? : whitespaces;

    sp = start = mbs;
    ep = end = mbs + mbslen(mbs) - 1;

    while(sp <= end && strchr(cset, *sp)) sp++;
    len = (sp > ep) ? 0 : ((ep-sp) + 1);
    if (mbs != sp) memmove(mbs, sp, len);
    mbs[len] = '\0';
    mbssetlen(mbs, len);
    return mbs;
}

mbs_t mbstrim(mbs_t mbs, char *cset)
{
    size_t len;
    char *start, *end, *sp, *ep;
    char *whitespaces = " \t\r\n";

    cset = cset ? : whitespaces;

    sp = start = mbs;
    ep = end = mbs + mbslen(mbs) - 1;

    while(sp <= end && strchr(cset, *sp)) sp++;
    while(ep > sp && strchr(cset, *ep)) ep--;
    len = (sp > ep) ? 0 : ((ep-sp) + 1);
    if (mbs != sp) memmove(mbs, sp, len);
    mbs[len] = '\0';
    mbssetlen(mbs, len);
    return mbs;
}

mbs_t mbsstripwhite(mbs_t mbs)
{
    return mbstrim(mbs, NULL);
}

mbs_t mbsrange(mbs_t mbs, int start, int end)
{
    size_t newlen, len = mbslen(mbs);

    if (len == 0) return mbs;

    if (start < 0) {
        start = len+start;
        if (start < 0) start = 0;
    }

    if (end < 0) {
        end = len+end;
        if (end < 0) end = 0;
    }

    newlen = (start > end) ? 0 : (end-start)+1;
    if (newlen != 0) {
        if (start >= (signed)len) {
            newlen = 0;
        } else if (end >= (signed)len) {
            end = len-1;
            newlen = (start > end) ? 0 : (end-start)+1;
        }
    } else {
        start = 0;
    }

    if (start && newlen) memmove(mbs, mbs+start, newlen);
    mbs[newlen] = 0;
    mbssetlen(mbs, newlen);
    return mbs;
}

mbs_t mbssubstring(mbs_t mbs, int start, int end)
{
    size_t newlen, len = mbslen(mbs);

    if (len == 0) return mbsempty();

    if (start < 0) {
        start = len+start;
        if (start < 0) start = 0;
    }

    if (end < 0) {
        end = len+end;
        if (end < 0) end = 0;
    }

    newlen = (start > end) ? 0 : (end-start)+1;
    if (newlen != 0) {
        if (start >= (signed)len) {
            newlen = 0;
        } else if (end >= (signed)len) {
            end = len-1;
            newlen = (start > end) ? 0 : (end-start)+1;
        }
    } else {
        start = 0;
    }

    if (newlen) {
        return mbsnewlen(mbs + start, newlen);
    } else {
        return mbsempty();
    }
}

mbs_t mbsprecatlen(mbs_t *pmbs, char *s, int len)
{
    mbs_t mbs = NULL;
    mbs_t nmbs = NULL;

    if (pmbs == NULL) {
        return NULL;
    }

    mbs = mbsnewlen(s, len);
    if (mbs == NULL) {
        return NULL;
    }

    if (*pmbs == NULL) {
        *pmbs = mbs;
        return mbs;
    }

    nmbs = mbscatmbs(&mbs, *pmbs);
    if (nmbs == NULL) {
        return NULL;
    }

    mbsfree(*pmbs);
    *pmbs = nmbs;
    return nmbs;
}

mbs_t mbsprecat(mbs_t *pmbs, char *s)
{
    return mbsprecatlen(pmbs, s, strlen(s));
}

mbs_t mbsprecatchar(mbs_t *pmbs, char c)
{
    char s[2] = { c, '\0' };

    return mbsprecat(pmbs, s);
}

mbs_t mbsprecatfmt(mbs_t *pmbs, char *fmt, ...)
{
    va_list params;
    mbs_t mbs = NULL;
    mbs_t nmbs = NULL;

    if (pmbs == NULL) {
        return NULL;
    }

    va_start(params, fmt);
    mbscatvfmt(&mbs, fmt, params);
    va_end(params);
    if (mbs == NULL)
        return NULL;

    if (*pmbs == NULL) {
        *pmbs = mbs;
        return mbs;
    }

    nmbs = mbscatmbs(&mbs, *pmbs);
    if (nmbs == NULL) {
        return NULL;
    }

    mbsfree(*pmbs);
    *pmbs = nmbs;
    return nmbs;
}

mbs_t mbsreadline(FILE *fp)
{
    int c;
    int len;
    mbs_t mbs = mbsnewsize(1024);

    while ((c = fgetc(fp)) != EOF) {
        mbscatchar(&mbs, (c & 0xFF));
        if (c == '\n') {
            break;
        }
    }

    len = mbslen(mbs);
    if (len == 0) {
        mbsfree(mbs);
        return NULL; // read EOF
    }

    if (mbs[len - 1] == '\n') {
        mbssetlen(mbs, len - 1);
        mbs[len - 1] = '\0';
    }

    len = mbslen(mbs);
    if (len == 0) {
        return mbs; // read a empty line
    }

    if (mbs[len - 1] == '\r') {
        mbssetlen(mbs, len - 1);
        mbs[len - 1] = '\0';
    }

    return mbs;
}

mbs_t mbscatstdout(mbs_t *pmbs, char *cmd)
{
    int rdlen = 0;
    size_t len = 0;
    FILE *f = NULL;
    char *line = NULL;
    mbs_t nmbs = NULL;

    if (!(f = popen(cmd, "r"))) {
        return NULL;
    }

    while ((rdlen = getline(&line, &len, f)) != -1) {
        nmbs = mbscatlen(pmbs, line, rdlen);
        if (nmbs == NULL) {
            free(line);
            pclose(f);
            return NULL;
        }
    }

    free(line);
    pclose(f);
    return nmbs;
}

mbs_t mbscatstdoutvargs(mbs_t *pmbs, char *fmt, ...)
{
    va_list params;
    char *cmd = NULL;
    mbs_t nmbs = NULL;

    va_start(params, fmt);
    vasprintf(&cmd, fmt, params);
    va_end(params);

    nmbs = mbscatstdout(pmbs, cmd);
    if (nmbs == NULL) {
        free(cmd);
        return NULL;
    }

    free(cmd);
    return nmbs;
}

mbs_t mbscatmem(mbs_t *pmbs, void *mem, int len)
{
    int i;
    mbs_t nmbs = NULL;
    unsigned char *buf = mem;

    for (i = 0; i < len; i++) {
        nmbs = mbscatfmt(pmbs, "%02X", buf[i]);
        if (nmbs == NULL) {
            return NULL;
        }
    }

    return nmbs;
}

mbs_t mbscatmac(mbs_t *pmbs, void *mem, int len)
{
    int i;
    mbs_t nmbs = NULL;
    unsigned char *buf = mem;

    for (i = 0; i < len; i++) {
        nmbs = mbscatfmt(pmbs, i ? ":%02X" : "%02X", buf[i]);
        if (nmbs == NULL) {
            return NULL;
        }
    }

    return nmbs;
}

mbs_t mbsnewescapesqlite(char *str)
{
    int i;
    mbs_t nmbs = NULL;
    int len = strlen(str);

    nmbs = mbsnewsize(len);
    if (nmbs == NULL) {
        return NULL;
    }

    for (i=0; i<len; i++) {
        if (str[i] == '\'') {
            mbscatchar(&nmbs, '\'');
        }
        mbscatchar(&nmbs, str[i]);
    }

    return nmbs;
}

mbs_t mbsescapesqlite(mbs_t mbs)
{
    int i;
    mbs_t nmbs = NULL;
    int len = mbslen(mbs);

    nmbs = mbsnewsize(len);
    if (nmbs == NULL) {
        return NULL;
    }

    for (i=0; i<len; i++) {
        unsigned char c = mbs[i];
        if (c == '\'') {
            mbscatchar(&nmbs, '\'');
        }
        mbscatchar(&nmbs, c);
    }

    return nmbs;
}

mbs_t mbscatescapesqlite(mbs_t *pmbs, mbs_t mbs)
{
    int i;
    int len = mbslen(mbs);

    if (pmbs == NULL) {
        return NULL;
    }

    for (i=0; i<len; i++) {
        unsigned char c = mbs[i];
        if (c == '\'') {
            mbscatchar(pmbs, '\'');
        }
        mbscatchar(pmbs, c);
    }

    return *pmbs;
}

mbs_t mbsnewescapejson(char *str)
{
    int i;
    mbs_t nmbs = NULL;
    int len = strlen(str);

    nmbs = mbsnewsize(len);
    if (nmbs == NULL) {
        return NULL;
    }

    for (i=0; i<len; i++) {
        unsigned char c = str[i];
        if (c == '"' || c == '\\' || c == '/') {
            mbscatchar(&nmbs, '\\');
            mbscatchar(&nmbs, c);
        } else if (c == '\b') {
            mbscat(&nmbs, "\\b");
        } else if (c == '\f') {
            mbscat(&nmbs, "\\f");
        } else if (c == '\n') {
            mbscat(&nmbs, "\\n");
        } else if (c == '\r') {
            mbscat(&nmbs, "\\r");
        } else if (c == '\t') {
            mbscat(&nmbs, "\\t");
        } else {
            mbscatchar(&nmbs, c);
        }
    }

    return nmbs;
}

mbs_t mbsescapejson(mbs_t mbs)
{
    int i;
    mbs_t nmbs = NULL;
    int len = mbslen(mbs);

    nmbs = mbsnewsize(len);
    if (nmbs == NULL) {
        return NULL;
    }

    for (i=0; i<len; i++) {
        unsigned char c = mbs[i];
        if (c == '"' || c == '\\' || c == '/') {
            mbscatchar(&nmbs, '\\');
            mbscatchar(&nmbs, c);
        } else if (c == '\b') {
            mbscat(&nmbs, "\\b");
        } else if (c == '\f') {
            mbscat(&nmbs, "\\f");
        } else if (c == '\n') {
            mbscat(&nmbs, "\\n");
        } else if (c == '\r') {
            mbscat(&nmbs, "\\r");
        } else if (c == '\t') {
            mbscat(&nmbs, "\\t");
        } else {
            mbscatchar(&nmbs, c);
        }
    }

    return nmbs;
}

mbs_t mbscatescapejson(mbs_t *pmbs, mbs_t mbs)
{
    int i;
    int len = mbslen(mbs);

    if (pmbs == NULL) {
        return NULL;
    }

    for (i=0; i<len; i++) {
        unsigned char c = mbs[i];
        if (c == '"' || c == '\\' || c == '/') {
            mbscatchar(pmbs, '\\');
            mbscatchar(pmbs, c);
        } else if (c == '\b') {
            mbscat(pmbs, "\\b");
        } else if (c == '\f') {
            mbscat(pmbs, "\\f");
        } else if (c == '\n') {
            mbscat(pmbs, "\\n");
        } else if (c == '\r') {
            mbscat(pmbs, "\\r");
        } else if (c == '\t') {
            mbscat(pmbs, "\\t");
        } else {
            mbscatchar(pmbs, c);
        }
    }

    return *pmbs;
}

mbs_t mbsprint(mbs_t mbs)
{
    int i;
    mbs_t nmbs = NULL;
    int len = mbslen(mbs);

    for (i=0; i<len+1; i++) {
        if (isprint(mbs[i])) {
            mbscatfmt(&nmbs, "%c", mbs[i]);
        } else {
            mbscatfmt(&nmbs, "\\x%02X", mbs[i] & 0xFF);
        }
    }

    return nmbs;
}

void mbsdump(mbs_t mbs)
{
    int i;
    mbs_hdr_t *hdr = MBSHDR(mbs);

    printf("mbs size %d len %d type %d data:\n", hdr->size, hdr->len, hdr->type);
    for (i=0; i<hdr->len+1; i++) {
        if (isprint(mbs[i])) {
            printf("%c", mbs[i]);
        } else {
            printf("\\x%02X", mbs[i] & 0xFF);
        }
    }
    printf("\n");

    return;
}

