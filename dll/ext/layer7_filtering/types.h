#ifndef _WINDIVERT_LAYER7_MATCHING_TYPES_H
#define _WINDIVERT_LAYER7_MATCHING_TYPES_H 1

#define WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_STRING       0
#define WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_AND_OPERATOR 1
#define WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_OR_OPERATOR  2
#define WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_NOT_OPERATOR 3
#define WINDIVERT_LAYER7_MATCHING_TOKEN_TYPE_ALIEN        4

#define WINDIVERT_MAX_GLOB_DEPTH    100

typedef struct _layer7_matching_expr_stack_ctx {
    char *token;
    int type;
    int neg;
    struct _layer7_matching_expr_stack_ctx *next;
}LAYER7_MATCHING_EXPR_STACK_CTX;

#endif
