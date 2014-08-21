#include "wildcard.h"
#include "types.h"

static int WinDivertWildcardMatchTest(const unsigned char *buf, const size_t bufsize,
                                      const unsigned char *wildcard, size_t wildcardsize);

/*
 * INFO(Santiago): Finds the first occurrence of wildcard in buf returning TRUE if it has
 * otherwise FALSE.
 */
int WinDivertWildcardMatch(const unsigned char *buf, const size_t bufsize,
                           const unsigned char *wildcard, size_t wildcardsize) {
    const unsigned char *wp = wildcard;
    const unsigned char *wp_end = wildcard + wildcardsize;
    int depth_ct = 0;
    if (wildcard == NULL || buf == NULL) {
        return 0;
    }
    if (*wp == '*') {
        depth_ct++;
    }
    wp++;
    while (wp !=  wp_end) {
        if (*wp == '*' && *(wp-1) != '\\') {
            depth_ct++;
        }
        wp++;
    }
    //  INFO(Santiago): We use it to avoid an uncontrolled stack growth.
    if (depth_ct > WINDIVERT_MAX_GLOB_DEPTH) {
        return 0;
    }
    return WinDivertWildcardMatchTest(buf, bufsize, wildcard, wildcardsize);
}
 
/*
 * INFO(Santiago): **Never** call it directly use windivert_wildcard_match() instead of it.
 */
static int WinDivertWildcardMatchTest(const unsigned char *buf, const size_t bufsize,
                                       const unsigned char *wildcard, size_t wildcardsize) {
    const unsigned char *bp = buf, *bp_end = buf + bufsize, *bpp;
    const unsigned char *wp = wildcard, *wp_end = wildcard + wildcardsize;
    const unsigned char *gp = NULL, *gp_end = NULL;
    int match = 0;
    int not = 0;
    while (bp != bp_end && !match) {
        bpp = bp + 1;
        match = 1;
        for (wp = wildcard; wp != wp_end && match && bp != bp_end; wp++) {
            switch (*wp) {
                case '*':
                    match = 0;
                    for (; bp != bp_end && !match; bp++) {
                        match = WinDivertWildcardMatchTest(bp, bp_end - bp,
                                                      wp + 1, wp_end - wp - 1);
                    }
                    if (match) {
                        return 1;
                    }
                    break;
                case '?':
                    match = (bp < bp_end);
                    bp++;
                    break;
                case '[':
                    gp = wp + 1;
                    if (*gp == '^') {
                        not = 1;
                        gp++;
                    }
                    while (wp < wp_end && *wp != ']') {
                        wp++;
                    }
                    if (wp == wp_end) {
                        //  INFO(Santiago): invalid wildcard buffer that shouldn't
                        //                  exist in normal (e.g. well compiled) situations.
                        return 0;
                    }
                    gp_end = wp;
                    if (not) {
                        match = 1;
                        for (; gp != gp_end && match; gp++) {
                            if (*gp == '\\') {
                                gp++;
                            }
                            match = (*gp != *bp);
                        }
                    } else {
                        match = 0;
                        for (; gp != gp_end && !match; gp++) {
                            if (*gp == '\\') {
                                gp++;
                            }
                            match = (*gp == *bp);
                        }
                    }
                    bp++;
                    break;
                case '\\':
                    if ((wp+1) != wp_end) {
                        wp++;
                        match = (*wp == *bp);
                        bp++;
                    }
                    break;
                default:
                    match = (*wp == *bp);
                    bp++;
                    break;
            }
        }
    match = match && (wp == wp_end);
    bp = bpp;
  }
  return match;
}
