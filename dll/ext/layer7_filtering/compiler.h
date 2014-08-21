#ifndef _WINDIVERT_LAYER7_MATCHING_COMPILER_H
#define _WINDIVERT_LAYER7_MATCHING_COMPILER_H 1

#include "types.h"
#include <stdlib.h>

LAYER7_MATCHING_EXPR_STACK_CTX *CompileLayer7MatchingFilter(const char *filter, const size_t filter_size, char *message);

#endif
