#include "compiler.h"
#include "alloc.h"
#include "ctx.h"
#include <stdio.h>
#include <string.h>

static LAYER7_MATCHING_EXPR_STACK_CTX *GetMatchingProgramFromFilterExpression(const char *filter, const size_t filter_size);

static int IsKnownOperator(const char *data);

static char *known_operators[] = {
    "or",
    "||",
    "and",
    "&&",
    "not",
    "!",
    ""
};

static int IsKnownOperator(const char *data) {
    int o;
    for (o = 0; known_operators[o][0] != 0; o++) {
        if (strcmp(data, known_operators[o]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int GetOperatorType(const char *operator) {
    if (strcmp(operator, "or") == 0 ||
        strcmp(operator, "||") == 0) {
        return WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_OR_OPERATOR;
    }
    if (strcmp(operator, "and") == 0 ||
        strcmp(operator, "&&") == 0) {
        return WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_AND_OPERATOR;
    }
    if (strcmp(operator, "not") == 0 ||
        strcmp(operator, "!") == 0) {
        return WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_NOT_OPERATOR;
    }
    return WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_ALIEN;
}

LAYER7_MATCHING_EXPR_STACK_CTX *CompileLayer7MatchingFilter(const char *filter, const size_t filter_size, char *message) {
  int state = 0;
  int round_brackets_counter = 0;
  int retval = 0;
  LAYER7_MATCHING_EXPR_STACK_CTX *matching_program = NULL;
  const char *fp = filter;
  const char *fp_end = filter + filter_size;
  char token[255];
  size_t t = 0;
  memset(token, 0, sizeof(token));
  while (fp != fp_end) {
    switch (*fp) {
        case '"':
            state = 0;
            fp++;
            while (fp != fp_end && *fp != '"') {
                if (*fp == '\\') {
                    fp++;
                }
                fp++;
            }
            if (*fp != '"') {
                if (message != NULL) {
                    sprintf(message, "invalid string.");
                }
                return 0;
            }
            fp++;
            break;

        case ' ':
        case '\n':
        case '\r':
        case '\t':
            if (t > 0) {
                if (t == sizeof(token)) {
                    if (message != NULL) {
                        sprintf(message, "too long token, aborting.");
                    }
                    return 0;
                }
                token[t] = 0;
                if (IsKnownOperator(token)) {
                    if (state != 0) {
                        if (message != NULL) {
                            sprintf(message, "unexpected operator at the position %d.", fp_end - fp - strlen(token) + 1);
                        }
                        return 0;
                    }
                    state = 1;
                } else {
                    if (message != NULL) {
                        sprintf(message, "\"%s\" is an unknown operator.", token);
                    }
                    return 0;
                }
                t = 0;
            }
            fp++;
            break;

        default:
            if (*fp == '(' ) {
                round_brackets_counter++;
                fp++;
                continue;
            } else if (*fp == ')') {
                round_brackets_counter--;
                fp++;
                continue;
            }
            if (t == sizeof(token)) {
                if (message != NULL) {
                    sprintf(message, "too long token, aborting.1");
                }
                return 0;
            }
            token[t] = *fp;
            t++;
            fp++;
            break;
    }
  }
  if ((retval = (state == 0 && round_brackets_counter == 0 && fp == fp_end))) {
    matching_program = GetMatchingProgramFromFilterExpression(filter, filter_size);
  }
  if (message != NULL) {
    if (retval) {
        sprintf(message, "success.");
    } else {
        sprintf(message, "invalid filter.");
    }
  }  
  return matching_program;
}

static LAYER7_MATCHING_EXPR_STACK_CTX *GetMatchingProgramFromFilterExpression(const char *filter, const size_t filter_size) {
    LAYER7_MATCHING_EXPR_STACK_CTX *sta = NULL, *stb = NULL, *top = NULL;
    char *infix_filter_expr = (char *) WinDivertAlloc(filter_size + 4);
    char token[255];
    size_t t;
    int neg = 0;
    const char *fp = NULL;
    const char *fp_end = NULL;
    memset(infix_filter_expr, 0, filter_size  + 3);
    memcpy(infix_filter_expr, filter, filter_size);
    strcat(infix_filter_expr,  " )");
    fp  = &infix_filter_expr[0];
    fp_end = fp + filter_size + 2;
    sta = PushTokenToLayer7MatchingExprStackCtx(sta, "(", 1, WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_ALIEN);
    while (fp != fp_end) {
        switch (*fp) {
            case '"':
                t = 0;
                memset(token, 0, sizeof(token));
                token[t++] = *fp;
                fp++;
                while (fp != fp_end && *fp != '"' && t < sizeof(token)) {
                    if (*fp == '\\') {
                        fp++;
                    }
                    token[t++] = *fp;
                    fp++;
                }
                if (*fp == '"' && t != sizeof(token)) {
                    fp++;
                    stb = PushTokenToLayer7MatchingExprStackCtx(stb, &token[1], t - 1, WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_STRING);
                } else {
                    DelLayer7MatchingExprStackCtx(sta);
                    DelLayer7MatchingExprStackCtx(stb);
                    return NULL;
                }
                t = 0;
                break;

            case '(':
                if (neg) {
                    DelLayer7MatchingExprStackCtx(sta);
                    DelLayer7MatchingExprStackCtx(stb);
                    return NULL;
                }
                sta = PushTokenToLayer7MatchingExprStackCtx(sta, fp, 1, WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_ALIEN);
                fp++;
                break;

            case ')':
                top = TopOfLayer7MatchingExprStackCtx(sta);
                while (top != NULL && *top->token != '(') {
                    stb = PushTokenToLayer7MatchingExprStackCtx(stb, top->token, strlen(top->token), top->type);
                    sta = PopTokenFromLayer7MatchingExprStackCtx(sta);
                    top = TopOfLayer7MatchingExprStackCtx(sta);
                }
                fp++;
                break;

            case ' ':
            case '\n':
            case '\r':
            case '\t':
                token[t] = 0;
                neg = 0;
                if (t > 0) {
                    if (IsKnownOperator(token)) {
                        if (strcmp(token, "not") == 0 || strcmp(token, "!") == 0) {
                            neg = 1;
                        }
                        top = TopOfLayer7MatchingExprStackCtx(sta);
                        while (IsKnownOperator(top->token)) {
                            stb = PushTokenToLayer7MatchingExprStackCtx(stb, top->token, strlen(top->token), top->type);
                            sta = PopTokenFromLayer7MatchingExprStackCtx(sta);
                            top = TopOfLayer7MatchingExprStackCtx(sta);
                        }
                        sta = PushTokenToLayer7MatchingExprStackCtx(sta, token, strlen(token), GetOperatorType(token));
                    } else {
                        DelLayer7MatchingExprStackCtx(sta);
                        DelLayer7MatchingExprStackCtx(stb);
                        return NULL;
                    }
                    t = 0;
                }
                fp++;
                break;

            default:
                token[t++] = *fp;
                fp++;
                break;
        }
    }
    DelLayer7MatchingExprStackCtx(sta);
    sta = NULL;
    while (!IsEmptyLayer7MatchingExprStackCtx(stb)) {
        top = TopOfLayer7MatchingExprStackCtx(stb);
        sta = PushTokenToLayer7MatchingExprStackCtx(sta, top->token, strlen(top->token), top->type);
        stb = PopTokenFromLayer7MatchingExprStackCtx(stb);
    }
    DelLayer7MatchingExprStackCtx(stb);
    free(infix_filter_expr);
    return sta;
}
