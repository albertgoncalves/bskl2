#ifndef __LANG_H__
#define __LANG_H__

#include "list.hpp"
#include "string.hpp"

typedef struct Expr Expr;

enum BinOp {
    BINOP_ADD = 0,
    BINOP_SUB,
    BINOP_MUL,
    BINOP_DIV,

    BINOP_LT,
    BINOP_LE,
    BINOP_GT,
    BINOP_GE,
    BINOP_EQ,
    BINOP_NE,

    BINOP_OR,
    BINOP_AND,
};

enum ExprTag {
    EXPR_UNDEF = 0,
    EXPR_PACK,

    EXPR_APP,
    EXPR_LET,
    EXPR_LETREC,
    EXPR_UNPACK,

    EXPR_U32,
    EXPR_VAR,

    EXPR_BINOP,
};

struct ExprBinding {
    String name;
    Expr*  expr;
};

struct ExprLet {
    List<ExprBinding> bindings;
    Expr*             expr;
};

struct ExprBranch {
    List<String> args;
    Expr*        expr;
    u8           tag;
};

struct ExprUnpack {
    Expr*            expr;
    List<ExprBranch> branches;
};

union ExprBody {
    String     as_var;
    u32        as_u32;
    u8         as_pack[2];
    Expr*      as_app[2];
    ExprLet    as_let;
    ExprUnpack as_unpack;
    BinOp      as_binop;
};

struct Expr {
    ExprBody body;
    ExprTag  tag;
};

enum FuncTag {
    FUNC_VAR = 0,
    FUNC_BINOP,
};

union FuncName {
    String as_var;
    BinOp  as_binop;
};

struct Func {
    List<String> args;
    Expr*        expr;
    FuncName     name;
    FuncTag      tag;
};

#endif
