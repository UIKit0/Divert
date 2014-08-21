#include "ctx.h"
#include "alloc.h"
#include <string.h>

#define NewLayer7MatchingExprStackCtx(l) ( (l) = (LAYER7_MATCHING_EXPR_STACK_CTX *) WinDivertAlloc(sizeof(LAYER7_MATCHING_EXPR_STACK_CTX)),\
                                           (l)->token = NULL, (l)->type = WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_ALIEN, (l)->neg = 0, (l)->next = NULL )

LAYER7_MATCHING_EXPR_STACK_CTX *PushTokenToLayer7MatchingExprStackCtx(LAYER7_MATCHING_EXPR_STACK_CTX *stack,
                                             const char *token, const size_t token_size, const int type) {
    LAYER7_MATCHING_EXPR_STACK_CTX *new_top = NULL;
    if (token == NULL || token_size == 0) {
        return stack;
    }
    NewLayer7MatchingExprStackCtx(new_top);
    new_top->token = (char *) WinDivertAlloc(token_size + 1);
    memset(new_top->token, 0, token_size + 1);
    memcpy(new_top->token, token, token_size);
    new_top->type = type;
    new_top->next = stack;
    return new_top;
}

LAYER7_MATCHING_EXPR_STACK_CTX *PopTokenFromLayer7MatchingExprStackCtx(LAYER7_MATCHING_EXPR_STACK_CTX *stack) {
    LAYER7_MATCHING_EXPR_STACK_CTX *curr_top = stack;
    LAYER7_MATCHING_EXPR_STACK_CTX *new_top;
    if (curr_top == NULL) {
        return NULL;
    }
    new_top = curr_top->next;
    curr_top->next = NULL;
    DelLayer7MatchingExprStackCtx(curr_top);
    return new_top;
}

void DelLayer7MatchingExprStackCtx(LAYER7_MATCHING_EXPR_STACK_CTX *stack) {
    LAYER7_MATCHING_EXPR_STACK_CTX *p, *t;
    for (t = p = stack; t; p = t) {
        t = p->next;
        free(p->token);
        free(p);
    }
}
