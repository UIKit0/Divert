#include "eval.h"
#include "ctx.h"
#include "wildcard.h"
#include <string.h>

int Layer7MatchingFilterEvaluate(const unsigned char *data, const size_t dsize, const LAYER7_MATCHING_EXPR_STACK_CTX *matching_filter) {
    int matches = 0;
    LAYER7_MATCHING_EXPR_STACK_CTX *rstack = NULL;
    const LAYER7_MATCHING_EXPR_STACK_CTX *ret = NULL, *top = NULL;
    unsigned char a, b;

    for (top = matching_filter; top != NULL; top = top->next) {
        switch (top->type) {
            case WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_STRING:
                matches = WinDivertWildcardMatch(data, dsize, top->token, strlen(top->token));
                rstack = PushTokenToLayer7MatchingExprStackCtx(rstack, (matches) ? "1" : "0", 1, WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_ALIEN);
                break;

            case WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_AND_OPERATOR:
            case WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_OR_OPERATOR:
                if (IsEmptyLayer7MatchingExprStackCtx(rstack)) {
                    continue;
                }
                ret = TopOfLayer7MatchingExprStackCtx(rstack);
                a = *ret->token;
                rstack = PopTokenFromLayer7MatchingExprStackCtx(rstack);
                if (IsEmptyLayer7MatchingExprStackCtx(rstack)) {
                    continue;
                }
                ret = TopOfLayer7MatchingExprStackCtx(rstack);
                b = *ret->token;
                rstack = PopTokenFromLayer7MatchingExprStackCtx(rstack);
                if (top->type == WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_OR_OPERATOR) {
                    matches = (a == '1' || b == '1');
                } else if (top->type == WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_AND_OPERATOR) {
                    matches = (a == '1' && b == '1');
                }
                rstack = PushTokenToLayer7MatchingExprStackCtx(rstack, (matches) ?  "1" : "0", 1, WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_ALIEN);
                break;
        }
    }
    if (!IsEmptyLayer7MatchingExprStackCtx(rstack)) {
        matches = (*rstack->token == '1');
    } else {
        matches = 0;
    }
    DelLayer7MatchingExprStackCtx(rstack);
    return matches;
}
