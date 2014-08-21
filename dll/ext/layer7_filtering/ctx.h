#ifndef _WINDIVERT_LAYER7_MATCHING_CTX_H
#define _WINDIVERT_LAYER7_MATCHING_CTX_H 1

#include "types.h"
#include <stdlib.h>

#define TopOfLayer7MatchingExprStackCtx(l) ( (l) )

#define IsEmptyLayer7MatchingExprStackCtx(l) ( ((l) == NULL) )

LAYER7_MATCHING_EXPR_STACK_CTX *PushTokenToLayer7MatchingExprStackCtx(LAYER7_MATCHING_EXPR_STACK_CTX *stack, const char *token, const size_t token_size, const int type);

LAYER7_MATCHING_EXPR_STACK_CTX *PopTokenFromLayer7MatchingExprStackCtx(LAYER7_MATCHING_EXPR_STACK_CTX *stack);

void DelLayer7MatchingExprStackCtx(LAYER7_MATCHING_EXPR_STACK_CTX *stack);


#endif
