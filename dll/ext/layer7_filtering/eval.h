#ifndef _WINDIVERT_LAYER7_MATCHING_EVAL_H
#define _WINDIVERT_LAYER7_MATCHING_EVAL_H 1

#include "types.h"
#include <stdlib.h>

int Layer7MatchingFilterEvaluate(const unsigned char *data, const size_t dsize, const LAYER7_MATCHING_EXPR_STACK_CTX *matching_filter);

#endif
