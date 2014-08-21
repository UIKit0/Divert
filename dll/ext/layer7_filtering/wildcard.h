#ifndef _WINDIVERT_LAYER7_MATCHING_WILDCARD_H
#define _WINDIVERT_LAYER7_MATCHING_WILDCARD_H 1

#include <stdlib.h>

int WinDivertWildcardMatch(const unsigned char *buf, const size_t bufsize,
                           const unsigned char *wildcard, size_t wildcardsize);

#endif
