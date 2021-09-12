#ifndef __LANG_H__
#define __LANG_H__

#include "list.hpp"
#include "string.hpp"

typedef struct Expr Expr;

enum ExprTag {
    EXPR_UNDEF = 0,
    EXPR_VAR,
    EXPR_U32,
    EXPR_DATA,
    EXPR_APP,
    EXPR_LET,
    EXPR_LETREC,
    EXPR_CASE,
};

struct ExprBinding {
    String string;
    Expr*  expr;
};

struct ExprLet {
    List<ExprBinding> bindings;
    Expr*             expr;
};

union ExprBody {
    String  as_string;
    u32     as_u32;
    u8      as_data[2];
    Expr*   as_app[2];
    ExprLet as_let;
};

struct Expr {
    ExprBody body;
    ExprTag  tag;
};

#endif
