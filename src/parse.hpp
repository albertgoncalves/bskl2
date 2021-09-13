#ifndef __PARSE_H__
#define __PARSE_H__

#include "buffer.hpp"
#include "lang.hpp"
#include "string.hpp"

enum TokenTag {
    TOKEN_UNDEF = 0,
    TOKEN_NEGATE,
    TOKEN_IF,

    TOKEN_LET,
    TOKEN_LETREC,
    TOKEN_PACK,
    TOKEN_UNPACK,

    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SCOLON,
    TOKEN_ASSIGN,

    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_LT,
    TOKEN_LE,
    TOKEN_GT,
    TOKEN_GE,
    TOKEN_EQ,
    TOKEN_NE,

    TOKEN_AND,
    TOKEN_OR,

    TOKEN_U32,
    TOKEN_VAR,
};

union TokenBody {
    String as_string;
    u32    as_u32;
};

struct Token {
    TokenBody body;
    TokenTag  tag;
    usize     offset;
};

template <usize S, usize B, usize U, usize E, usize F>
struct ParseMemory {
    Buffer<ListNode<String>, S>      strings;
    Buffer<ListNode<ExprBinding>, B> bindings;
    Buffer<ListNode<ExprBranch>, U>  branches;
    Buffer<Expr, E>                  exprs;
    Buffer<Func, F>                  funcs;
};

#define IS_ALPHA(x) \
    ((('A' <= (x)) && ((x) <= 'Z')) || (('a' <= (x)) && ((x) <= 'z')))

#define IS_DIGIT(x) (('0' <= (x)) && ((x) <= '9'))

#define IS_PUNCT(x) ((x) == '_')

#define IS_ALPHA_OR_DIGIT_OR_PUNCT(x) \
    (IS_ALPHA(x) || IS_DIGIT(x) || IS_PUNCT(x))

static u32 parse_u32(String string, usize* i) {
    u32 a = 0;
    for (; ((*i) < string.len) && IS_DIGIT(string.chars[*i]); ++(*i)) {
        const u32 b = (a * 10) + static_cast<u32>(string.chars[*i] - '0');
        EXIT_IF(b < a);
        a = b;
    }
    return a;
}

template <usize N>
static void set_tokens(String source, Buffer<Token, N>* tokens) {
    tokens->len = 0;
    for (usize i = 0; i < source.len;) {
        switch (source.chars[i]) {
        case '#': {
            ++i;
            for (; i < source.len; ++i) {
                if (source.chars[i] == '\n') {
                    ++i;
                    break;
                }
            }
            break;
        }
        case ' ':
        case '\t':
        case '\n': {
            ++i;
            break;
        }
        case '(': {
            Token* token = alloc(tokens);
            token->tag = TOKEN_LPAREN;
            token->offset = i++;
            break;
        }
        case ')': {
            Token* token = alloc(tokens);
            token->tag = TOKEN_RPAREN;
            token->offset = i++;
            break;
        }
        case '{': {
            Token* token = alloc(tokens);
            token->tag = TOKEN_LBRACE;
            token->offset = i++;
            break;
        }
        case '}': {
            Token* token = alloc(tokens);
            token->tag = TOKEN_RBRACE;
            token->offset = i++;
            break;
        }
        case ';': {
            Token* token = alloc(tokens);
            token->tag = TOKEN_SCOLON;
            token->offset = i++;
            break;
        }
        case '=': {
            Token* token = alloc(tokens);
            token->tag = TOKEN_ASSIGN;
            token->offset = i++;
            if ((i < source.len) && (source.chars[i] == '=')) {
                token->tag = TOKEN_EQ;
                ++i;
            }
            break;
        }
        case '!': {
            Token* token = alloc(tokens);
            token->tag = TOKEN_NE;
            token->offset = i++;
            EXIT_IF(source.len <= i);
            EXIT_IF(source.chars[i] == '=');
            ++i;
            break;
        }
        case '+': {
            Token* token = alloc(tokens);
            token->tag = TOKEN_ADD;
            token->offset = i++;
            break;
        }
        case '-': {
            Token* token = alloc(tokens);
            token->tag = TOKEN_SUB;
            token->offset = i++;
            break;
        }
        case '*': {
            Token* token = alloc(tokens);
            token->tag = TOKEN_MUL;
            token->offset = i++;
            break;
        }
        case '/': {
            Token* token = alloc(tokens);
            token->tag = TOKEN_DIV;
            token->offset = i++;
            break;
        }
        case '<': {
            Token* token = alloc(tokens);
            token->tag = TOKEN_LT;
            token->offset = i++;
            if ((i < source.len) && (source.chars[i] == '=')) {
                token->tag = TOKEN_LE;
                ++i;
            }
            break;
        }
        case '>': {
            Token* token = alloc(tokens);
            token->tag = TOKEN_GT;
            token->offset = i++;
            if ((i < source.len) && (source.chars[i] == '=')) {
                token->tag = TOKEN_GE;
                ++i;
            }
            break;
        }
        case '&': {
            Token* token = alloc(tokens);
            token->tag = TOKEN_AND;
            token->offset = i++;
            break;
        }
        case '|': {
            Token* token = alloc(tokens);
            token->tag = TOKEN_OR;
            token->offset = i++;
            break;
        }
        default: {
            EXIT_IF(!(IS_ALPHA_OR_DIGIT_OR_PUNCT(source.chars[i])));
            Token* token = alloc(tokens);
            token->offset = i;
            if (IS_DIGIT(source.chars[i])) {
                token->body.as_u32 = parse_u32(source, &i);
                token->tag = TOKEN_U32;
                continue;
            }
            usize j = i;
            for (; j < source.len; ++j) {
                if (!IS_ALPHA_OR_DIGIT_OR_PUNCT(source.chars[j])) {
                    break;
                }
            }
            EXIT_IF(i == j);
            String var = {&source.chars[i], j - i};
            if (var == GET_STRING("undef")) {
                token->tag = TOKEN_UNDEF;
            } else if (var == GET_STRING("negate")) {
                token->tag = TOKEN_NEGATE;
            } else if (var == GET_STRING("if")) {
                token->tag = TOKEN_IF;
            } else if (var == GET_STRING("let")) {
                token->tag = TOKEN_LET;
            } else if (var == GET_STRING("letrec")) {
                token->tag = TOKEN_LETREC;
            } else if (var == GET_STRING("pack")) {
                token->tag = TOKEN_PACK;
            } else if (var == GET_STRING("unpack")) {
                token->tag = TOKEN_UNPACK;
            } else {
                token->body.as_string = var;
                token->tag = TOKEN_VAR;
            }
            i = j;
        }
        }
    }
}

template <usize N>
static void test_set_tokens(Buffer<Token, N>* tokens) {
    {
        set_tokens(GET_STRING("1234"), tokens);
        EXIT_IF(tokens->len != 1);
        EXIT_IF(tokens->items[0].tag != TOKEN_U32);
        EXIT_IF(tokens->items[0].body.as_u32 != 1234);
        EXIT_IF(tokens->items[0].offset != 0);
        fprintf(stderr, ".");
    }
    {
        set_tokens(GET_STRING("\tundef"), tokens);
        EXIT_IF(tokens->len != 1);
        EXIT_IF(tokens->items[0].tag != TOKEN_UNDEF);
        EXIT_IF(tokens->items[0].offset != 1);
        fprintf(stderr, ".");
    }
    {
        set_tokens(GET_STRING("\n  negate"), tokens);
        EXIT_IF(tokens->len != 1);
        EXIT_IF(tokens->items[0].tag != TOKEN_NEGATE);
        EXIT_IF(tokens->items[0].offset != 3);
        fprintf(stderr, ".");
    }
    {
        set_tokens(GET_STRING("\n\nlet"), tokens);
        EXIT_IF(tokens->len != 1);
        EXIT_IF(tokens->items[0].tag != TOKEN_LET);
        EXIT_IF(tokens->items[0].offset != 2);
        fprintf(stderr, ".");
    }
    {
        set_tokens(GET_STRING(" letrec "), tokens);
        EXIT_IF(tokens->len != 1);
        EXIT_IF(tokens->items[0].tag != TOKEN_LETREC);
        EXIT_IF(tokens->items[0].offset != 1);
        fprintf(stderr, ".");
    }
    {
        set_tokens(GET_STRING("f x {\n  pack 3 1 x\n}"), tokens);
        EXIT_IF(tokens->len != 8);
        EXIT_IF(tokens->items[0].tag != TOKEN_VAR);
        EXIT_IF(tokens->items[0].body.as_string != GET_STRING("f"));
        EXIT_IF(tokens->items[0].offset != 0);
        EXIT_IF(tokens->items[1].tag != TOKEN_VAR);
        EXIT_IF(tokens->items[1].body.as_string != GET_STRING("x"));
        EXIT_IF(tokens->items[1].offset != 2);
        EXIT_IF(tokens->items[2].tag != TOKEN_LBRACE);
        EXIT_IF(tokens->items[2].offset != 4);
        EXIT_IF(tokens->items[3].tag != TOKEN_PACK);
        EXIT_IF(tokens->items[3].offset != 8);
        EXIT_IF(tokens->items[4].tag != TOKEN_U32);
        EXIT_IF(tokens->items[4].body.as_u32 != 3);
        EXIT_IF(tokens->items[4].offset != 13);
        EXIT_IF(tokens->items[5].tag != TOKEN_U32);
        EXIT_IF(tokens->items[5].body.as_u32 != 1);
        EXIT_IF(tokens->items[5].offset != 15);
        EXIT_IF(tokens->items[6].tag != TOKEN_VAR);
        EXIT_IF(tokens->items[6].body.as_string != GET_STRING("x"));
        EXIT_IF(tokens->items[6].offset != 17);
        EXIT_IF(tokens->items[7].tag != TOKEN_RBRACE);
        EXIT_IF(tokens->items[7].offset != 19);
        fprintf(stderr, ".");
    }
    {
        set_tokens(GET_STRING("if (a == b) _xy _uv"), tokens);
        EXIT_IF(tokens->len != 8);
        EXIT_IF(tokens->items[0].tag != TOKEN_IF);
        EXIT_IF(tokens->items[0].offset != 0);
        EXIT_IF(tokens->items[1].tag != TOKEN_LPAREN);
        EXIT_IF(tokens->items[1].offset != 3);
        EXIT_IF(tokens->items[2].tag != TOKEN_VAR);
        EXIT_IF(tokens->items[2].body.as_string != GET_STRING("a"));
        EXIT_IF(tokens->items[2].offset != 4);
        EXIT_IF(tokens->items[3].tag != TOKEN_EQ);
        EXIT_IF(tokens->items[3].offset != 6);
        EXIT_IF(tokens->items[4].tag != TOKEN_VAR);
        EXIT_IF(tokens->items[4].body.as_string != GET_STRING("b"));
        EXIT_IF(tokens->items[4].offset != 9);
        EXIT_IF(tokens->items[5].tag != TOKEN_RPAREN);
        EXIT_IF(tokens->items[5].offset != 10);
        EXIT_IF(tokens->items[6].tag != TOKEN_VAR);
        EXIT_IF(tokens->items[6].body.as_string != GET_STRING("_xy"));
        EXIT_IF(tokens->items[6].offset != 12);
        EXIT_IF(tokens->items[7].tag != TOKEN_VAR);
        EXIT_IF(tokens->items[7].body.as_string != GET_STRING("_uv"));
        EXIT_IF(tokens->items[7].offset != 16);
        fprintf(stderr, ".");
    }
    {
        set_tokens(
            GET_STRING("unpack xyz_123 { # ...\n  1 = 0;\n  2 x = x\n}"),
            tokens);
        EXIT_IF(tokens->len != 12);
        EXIT_IF(tokens->items[0].tag != TOKEN_UNPACK);
        EXIT_IF(tokens->items[0].offset != 0);
        EXIT_IF(tokens->items[1].tag != TOKEN_VAR);
        EXIT_IF(tokens->items[1].body.as_string != GET_STRING("xyz_123"));
        EXIT_IF(tokens->items[1].offset != 7);
        EXIT_IF(tokens->items[2].tag != TOKEN_LBRACE);
        EXIT_IF(tokens->items[2].offset != 15);
        EXIT_IF(tokens->items[3].tag != TOKEN_U32);
        EXIT_IF(tokens->items[3].body.as_u32 != 1);
        EXIT_IF(tokens->items[3].offset != 25);
        EXIT_IF(tokens->items[4].tag != TOKEN_ASSIGN);
        EXIT_IF(tokens->items[4].offset != 27);
        EXIT_IF(tokens->items[5].tag != TOKEN_U32);
        EXIT_IF(tokens->items[5].body.as_u32 != 0);
        EXIT_IF(tokens->items[5].offset != 29);
        EXIT_IF(tokens->items[6].tag != TOKEN_SCOLON);
        EXIT_IF(tokens->items[6].offset != 30);
        EXIT_IF(tokens->items[7].tag != TOKEN_U32);
        EXIT_IF(tokens->items[7].body.as_u32 != 2);
        EXIT_IF(tokens->items[7].offset != 34);
        EXIT_IF(tokens->items[8].tag != TOKEN_VAR);
        EXIT_IF(tokens->items[8].body.as_string != GET_STRING("x"));
        EXIT_IF(tokens->items[8].offset != 36);
        EXIT_IF(tokens->items[9].tag != TOKEN_ASSIGN);
        EXIT_IF(tokens->items[9].offset != 38);
        EXIT_IF(tokens->items[10].tag != TOKEN_VAR);
        EXIT_IF(tokens->items[10].body.as_string != GET_STRING("x"));
        EXIT_IF(tokens->items[10].offset != 40);
        EXIT_IF(tokens->items[11].tag != TOKEN_RBRACE);
        EXIT_IF(tokens->items[11].offset != 42);
        fprintf(stderr, ".");
    }
    {
        set_tokens(GET_STRING("<\t"), tokens);
        EXIT_IF(tokens->len != 1);
        EXIT_IF(tokens->items[0].tag != TOKEN_LT);
        EXIT_IF(tokens->items[0].offset != 0);
        fprintf(stderr, ".");
    }
    {
        set_tokens(GET_STRING("\n<=\n"), tokens);
        EXIT_IF(tokens->len != 1);
        EXIT_IF(tokens->items[0].tag != TOKEN_LE);
        EXIT_IF(tokens->items[0].offset != 1);
        fprintf(stderr, ".");
    }
    {
        set_tokens(GET_STRING("> ="), tokens);
        EXIT_IF(tokens->len != 2);
        EXIT_IF(tokens->items[0].tag != TOKEN_GT);
        EXIT_IF(tokens->items[0].offset != 0);
        EXIT_IF(tokens->items[1].tag != TOKEN_ASSIGN);
        EXIT_IF(tokens->items[1].offset != 2);
        fprintf(stderr, ".");
    }
    {
        set_tokens(GET_STRING("\t >="), tokens);
        EXIT_IF(tokens->len != 1);
        EXIT_IF(tokens->items[0].tag != TOKEN_GE);
        EXIT_IF(tokens->items[0].offset != 2);
        fprintf(stderr, ".");
    }
    {
        set_tokens(GET_STRING("+-*/&|"), tokens);
        EXIT_IF(tokens->len != 6);
        EXIT_IF(tokens->items[0].tag != TOKEN_ADD);
        EXIT_IF(tokens->items[0].offset != 0);
        EXIT_IF(tokens->items[1].tag != TOKEN_SUB);
        EXIT_IF(tokens->items[1].offset != 1);
        EXIT_IF(tokens->items[2].tag != TOKEN_MUL);
        EXIT_IF(tokens->items[2].offset != 2);
        EXIT_IF(tokens->items[3].tag != TOKEN_DIV);
        EXIT_IF(tokens->items[3].offset != 3);
        EXIT_IF(tokens->items[4].tag != TOKEN_AND);
        EXIT_IF(tokens->items[4].offset != 4);
        EXIT_IF(tokens->items[5].tag != TOKEN_OR);
        EXIT_IF(tokens->items[5].offset != 5);
        fprintf(stderr, ".");
    }
    fprintf(stderr, "\n");
}

template <usize T, usize S, usize B, usize U, usize E, usize F>
Expr* parse_expr(const Buffer<Token, T>*, ParseMemory<S, B, U, E, F>*, usize*);

template <usize T, usize S, usize B, usize U, usize E, usize F>
static ExprBinding parse_binding(const Buffer<Token, T>*     tokens,
                                 ParseMemory<S, B, U, E, F>* memory,
                                 usize*                      i) {
    ExprBinding binding;
    {
        const Token token = get(tokens, (*i)++);
        EXIT_IF(token.tag != TOKEN_VAR);
        binding.name = token.body.as_string;
    }
    {
        const Token token = get(tokens, (*i)++);
        EXIT_IF(token.tag != TOKEN_ASSIGN);
    }
    binding.expr = parse_expr(tokens, memory, i);
    return binding;
}

template <usize    T,
          usize    S,
          usize    B,
          usize    U,
          usize    E,
          usize    F,
          TokenTag X,
          ExprTag  Y>
static Expr* parse_let(const Buffer<Token, T>*     tokens,
                       ParseMemory<S, B, U, E, F>* memory,
                       usize*                      i) {
    {
        const Token token = get(tokens, (*i)++);
        EXIT_IF(token.tag != X);
    }
    {
        const Token token = get(tokens, (*i)++);
        EXIT_IF(token.tag != TOKEN_LBRACE);
    }
    Expr* expr = alloc(&memory->exprs);
    expr->tag = Y;
    for (;;) {
        append(&memory->bindings,
               &expr->body.as_let.bindings,
               parse_binding(tokens, memory, i));
        const Token token = get(tokens, (*i)++);
        if (token.tag == TOKEN_SCOLON) {
            continue;
        }
        if (token.tag == TOKEN_RBRACE) {
            return expr;
        }
    }
}

template <usize T, usize S>
static void parse_args(const Buffer<Token, T>*      tokens,
                       Buffer<ListNode<String>, S>* strings,
                       List<String>*                list,
                       usize*                       i) {
    for (;;) {
        const Token token = get(tokens, *i);
        if (token.tag != TOKEN_VAR) {
            return;
        }
        ++(*i);
        append(strings, list, token.body.as_string);
    }
}

template <usize T, usize S, usize B, usize U, usize E, usize F>
static ExprBranch parse_branch(const Buffer<Token, T>*     tokens,
                               ParseMemory<S, B, U, E, F>* memory,
                               usize*                      i) {
    ExprBranch branch;
    {
        const Token token = get(tokens, (*i)++);
        EXIT_IF(token.tag != TOKEN_U32);
        EXIT_IF(0xFF < token.body.as_u32);
        branch.tag = static_cast<u8>(token.body.as_u32);
    }
    parse_args(tokens, &memory->strings, &branch.args, i);
    {
        const Token token = get(tokens, (*i)++);
        EXIT_IF(token.tag != TOKEN_ASSIGN);
    }
    branch.expr = parse_expr(tokens, memory, i);
    return branch;
}

template <usize T, usize S, usize B, usize U, usize E, usize F>
static Expr* parse_unpack(const Buffer<Token, T>*     tokens,
                          ParseMemory<S, B, U, E, F>* memory,
                          usize*                      i) {
    Expr* expr = alloc(&memory->exprs);
    expr->tag = EXPR_UNPACK;
    {
        const Token token = get(tokens, (*i)++);
        EXIT_IF(token.tag != TOKEN_UNPACK);
    }
    expr->body.as_unpack.expr = parse_expr(tokens, memory, i);
    {
        const Token token = get(tokens, (*i)++);
        EXIT_IF(token.tag != TOKEN_LBRACE);
    }
    for (;;) {
        append(&memory->branches,
               &expr->body.as_unpack.branches,
               parse_branch(tokens, memory, i));
        const Token token = get(tokens, (*i)++);
        if (token.tag == TOKEN_SCOLON) {
            continue;
        }
        if (token.tag == TOKEN_LBRACE) {
            return expr;
        }
    }
}

template <usize E>
static Expr* get_app(Buffer<Expr, E>* exprs, Expr* l, Expr* r) {
    Expr* app = alloc(exprs);
    app->tag = EXPR_APP;
    app->body.as_app[0] = l;
    app->body.as_app[1] = r;
    return app;
}

template <usize T, usize S, usize B, usize U, usize E, usize F>
static Expr* parse_atomic(const Buffer<Token, T>*     tokens,
                          ParseMemory<S, B, U, E, F>* memory,
                          usize*                      i) {
    const Token token = get(tokens, *i);
    if (token.tag == TOKEN_UNDEF) {
        ++(*i);
        Expr* expr = alloc(&memory->exprs);
        expr->tag = EXPR_UNDEF;
        return expr;
    } else if (token.tag == TOKEN_PACK) {
        ++(*i);
        Expr* expr = alloc(&memory->exprs);
        expr->tag = EXPR_PACK;
        {
            const Token token0 = get(tokens, (*i)++);
            EXIT_IF(token0.tag != TOKEN_U32);
            EXIT_IF(0xFF < token0.body.as_u32);
            expr->body.as_pack[0] = static_cast<u8>(token0.body.as_u32);
        }
        {
            const Token token0 = get(tokens, (*i)++);
            EXIT_IF(token0.tag != TOKEN_U32);
            EXIT_IF(0xFF < token0.body.as_u32);
            expr->body.as_pack[1] = static_cast<u8>(token0.body.as_u32);
        }
        return expr;
    } else if (token.tag == TOKEN_LPAREN) {
        ++(*i);
        Expr* expr = parse_expr(tokens, memory, i);
        {
            const Token token0 = get(tokens, (*i)++);
            EXIT_IF(token0.tag != TOKEN_RPAREN);
        }
        return expr;
    } else if (token.tag == TOKEN_VAR) {
        ++(*i);
        Expr* expr = alloc(&memory->exprs);
        expr->tag = EXPR_VAR;
        expr->body.as_var = token.body.as_string;
        return expr;
    } else if (token.tag == TOKEN_U32) {
        ++(*i);
        Expr* expr = alloc(&memory->exprs);
        expr->tag = EXPR_U32;
        expr->body.as_u32 = token.body.as_u32;
        return expr;
    }
    return null;
}

template <usize T, usize S, usize B, usize U, usize E, usize F>
static Expr* parse_expr6(const Buffer<Token, T>*     tokens,
                         ParseMemory<S, B, U, E, F>* memory,
                         usize*                      i) {
    Expr* l = parse_atomic(tokens, memory, i);
    EXIT_IF(!l);
    for (;;) {
        Expr* r = parse_atomic(tokens, memory, i);
        if (!r) {
            return l;
        }
        l = get_app(&memory->exprs, l, r);
    }
}

template <usize T, usize S, usize B, usize U, usize E, usize F>
static Expr* parse_expr5(const Buffer<Token, T>*     tokens,
                         ParseMemory<S, B, U, E, F>* memory,
                         usize*                      i) {
    Expr* l = parse_expr6(tokens, memory, i);
    {
        const Token token = get(tokens, *i);
        if (token.tag == TOKEN_MUL) {
            ++(*i);
            Expr* r = parse_expr5(tokens, memory, i);
            Expr* op = alloc(&memory->exprs);
            op->tag = EXPR_BINOP;
            op->body.as_binop = BINOP_MUL;
            return get_app(&memory->exprs, get_app(&memory->exprs, op, l), r);
        } else if (token.tag == TOKEN_DIV) {
            ++(*i);
            Expr* r = parse_expr6(tokens, memory, i);
            Expr* op = alloc(&memory->exprs);
            op->tag = EXPR_BINOP;
            op->body.as_binop = BINOP_DIV;
            return get_app(&memory->exprs, get_app(&memory->exprs, op, l), r);
        }
    }
    return l;
}

template <usize T, usize S, usize B, usize U, usize E, usize F>
static Expr* parse_expr4(const Buffer<Token, T>*     tokens,
                         ParseMemory<S, B, U, E, F>* memory,
                         usize*                      i) {
    Expr* l = parse_expr5(tokens, memory, i);
    {
        const Token token = get(tokens, *i);
        if (token.tag == TOKEN_ADD) {
            ++(*i);
            Expr* r = parse_expr4(tokens, memory, i);
            Expr* op = alloc(&memory->exprs);
            op->tag = EXPR_BINOP;
            op->body.as_binop = BINOP_ADD;
            return get_app(&memory->exprs, get_app(&memory->exprs, op, l), r);
        } else if (token.tag == TOKEN_SUB) {
            ++(*i);
            Expr* r = parse_expr5(tokens, memory, i);
            Expr* op = alloc(&memory->exprs);
            op->tag = EXPR_BINOP;
            op->body.as_binop = BINOP_SUB;
            return get_app(&memory->exprs, get_app(&memory->exprs, op, l), r);
        }
    }
    return l;
}

template <usize T, usize S, usize B, usize U, usize E, usize F>
static Expr* parse_expr3(const Buffer<Token, T>*     tokens,
                         ParseMemory<S, B, U, E, F>* memory,
                         usize*                      i) {
    Expr* l = parse_expr4(tokens, memory, i);
    {
        const Token token = get(tokens, *i);
        if (token.tag == TOKEN_LT) {
            ++(*i);
            Expr* r = parse_expr4(tokens, memory, i);
            Expr* op = alloc(&memory->exprs);
            op->tag = EXPR_BINOP;
            op->body.as_binop = BINOP_LT;
            return get_app(&memory->exprs, get_app(&memory->exprs, op, l), r);
        } else if (token.tag == TOKEN_LE) {
            ++(*i);
            Expr* r = parse_expr4(tokens, memory, i);
            Expr* op = alloc(&memory->exprs);
            op->tag = EXPR_BINOP;
            op->body.as_binop = BINOP_LE;
            return get_app(&memory->exprs, get_app(&memory->exprs, op, l), r);
        } else if (token.tag == TOKEN_GT) {
            ++(*i);
            Expr* r = parse_expr4(tokens, memory, i);
            Expr* op = alloc(&memory->exprs);
            op->tag = EXPR_BINOP;
            op->body.as_binop = BINOP_GT;
            return get_app(&memory->exprs, get_app(&memory->exprs, op, l), r);
        } else if (token.tag == TOKEN_GE) {
            ++(*i);
            Expr* r = parse_expr4(tokens, memory, i);
            Expr* op = alloc(&memory->exprs);
            op->tag = EXPR_BINOP;
            op->body.as_binop = BINOP_GE;
            return get_app(&memory->exprs, get_app(&memory->exprs, op, l), r);
        } else if (token.tag == TOKEN_EQ) {
            ++(*i);
            Expr* r = parse_expr4(tokens, memory, i);
            Expr* op = alloc(&memory->exprs);
            op->tag = EXPR_BINOP;
            op->body.as_binop = BINOP_EQ;
            return get_app(&memory->exprs, get_app(&memory->exprs, op, l), r);
        } else if (token.tag == TOKEN_NE) {
            ++(*i);
            Expr* r = parse_expr4(tokens, memory, i);
            Expr* op = alloc(&memory->exprs);
            op->tag = EXPR_BINOP;
            op->body.as_binop = BINOP_NE;
            return get_app(&memory->exprs, get_app(&memory->exprs, op, l), r);
        }
    }
    return l;
}

template <usize T, usize S, usize B, usize U, usize E, usize F>
static Expr* parse_expr2(const Buffer<Token, T>*     tokens,
                         ParseMemory<S, B, U, E, F>* memory,
                         usize*                      i) {
    Expr* l = parse_expr3(tokens, memory, i);
    {
        const Token token = get(tokens, *i);
        if (token.tag == TOKEN_AND) {
            ++(*i);
            Expr* r = parse_expr2(tokens, memory, i);
            Expr* op = alloc(&memory->exprs);
            op->tag = EXPR_BINOP;
            op->body.as_binop = BINOP_AND;
            return get_app(&memory->exprs, get_app(&memory->exprs, op, l), r);
        }
    }
    return l;
}

template <usize T, usize S, usize B, usize U, usize E, usize F>
static Expr* parse_expr1(const Buffer<Token, T>*     tokens,
                         ParseMemory<S, B, U, E, F>* memory,
                         usize*                      i) {
    Expr* l = parse_expr2(tokens, memory, i);
    {
        const Token token = get(tokens, *i);
        if (token.tag == TOKEN_OR) {
            ++(*i);
            Expr* r = parse_expr1(tokens, memory, i);
            Expr* op = alloc(&memory->exprs);
            op->tag = EXPR_BINOP;
            op->body.as_binop = BINOP_OR;
            return get_app(&memory->exprs, get_app(&memory->exprs, op, l), r);
        }
    }
    return l;
}

template <usize T, usize S, usize B, usize U, usize E, usize F>
Expr* parse_expr(const Buffer<Token, T>*     tokens,
                 ParseMemory<S, B, U, E, F>* memory,
                 usize*                      i) {
    const Token token = get(tokens, *i);
    if (token.tag == TOKEN_LET) {
        ++(*i);
        return parse_let<T, S, B, U, E, F, TOKEN_LET, EXPR_LET>(tokens,
                                                                memory,
                                                                i);
    } else if (token.tag == TOKEN_LETREC) {
        ++(*i);
        return parse_let<T, S, B, U, E, F, TOKEN_LETREC, EXPR_LETREC>(tokens,
                                                                      memory,
                                                                      i);
    } else if (token.tag == TOKEN_UNPACK) {
        ++(*i);
        return parse_unpack(tokens, memory, i);
    }
    return parse_expr1(tokens, memory, i);
}

template <usize T, usize S, usize B, usize U, usize E, usize F>
static void parse_func(const Buffer<Token, T>*     tokens,
                       ParseMemory<S, B, U, E, F>* memory,
                       usize*                      i) {
    Func* func = alloc(&memory->funcs);
    {
        const Token token = get(tokens, (*i)++);
        EXIT_IF(token.tag != TOKEN_VAR);
        func->name.as_var = token.body.as_string;
    }
    parse_args(tokens, &memory->strings, &func->args, i);
    {
        const Token token = get(tokens, (*i)++);
        EXIT_IF(token.tag != TOKEN_LBRACE);
    }
    func->expr = parse_expr(tokens, memory, i);
    {
        const Token token = get(tokens, (*i)++);
        EXIT_IF(token.tag != TOKEN_RBRACE);
    }
}

template <usize T, usize S, usize B, usize U, usize E, usize F>
static void parse_program(const Buffer<Token, T>*     tokens,
                          ParseMemory<S, B, U, E, F>* memory) {
    memory->funcs.len = 0;
    memory->exprs.len = 0;
    usize i = 0;
    while (i < tokens->len) {
        parse_func(tokens, memory, &i);
    }
    EXIT_IF(memory->funcs.len == 0);
}

template <usize T, usize S, usize B, usize U, usize E, usize F>
static void test_parse_program(Buffer<Token, T>*           tokens,
                               ParseMemory<S, B, U, E, F>* memory) {
    {
        set_tokens(GET_STRING("main { 1234 }"), tokens);
        parse_program(tokens, memory);
        EXIT_IF(memory->funcs.len != 1);
        {
            EXIT_IF(memory->funcs.items[0].tag != FUNC_VAR);
            EXIT_IF(memory->funcs.items[0].name.as_var != GET_STRING("main"));
            EXIT_IF(memory->funcs.items[0].args.first);
            EXIT_IF(memory->funcs.items[0].args.last);
            EXIT_IF(memory->funcs.items[0].expr->tag != EXPR_U32);
            EXIT_IF(memory->funcs.items[0].expr->body.as_u32 != 1234);
        }
        fprintf(stderr, ".");
    }
    {
        set_tokens(GET_STRING("# ...\n"
                              "f x { x }\n"
                              "g a b c { (a - b) + c }\n"
                              "h { pack 1 0 } # ?!"),
                   tokens);
        parse_program(tokens, memory);
        EXIT_IF(memory->funcs.len != 3);
        {
            EXIT_IF(memory->funcs.items[0].tag != FUNC_VAR);
            EXIT_IF(memory->funcs.items[0].name.as_var != GET_STRING("f"));
            EXIT_IF(memory->funcs.items[0].args.first->value !=
                    GET_STRING("x"));
            EXIT_IF(memory->funcs.items[0].args.first->next);
            EXIT_IF(memory->funcs.items[0].args.last->value !=
                    GET_STRING("x"));
        }
        {
            EXIT_IF(memory->funcs.items[1].tag != FUNC_VAR);
            EXIT_IF(memory->funcs.items[1].name.as_var != GET_STRING("g"));
            {
                ListNode<String>* arg = memory->funcs.items[1].args.first;
                EXIT_IF(arg->value != GET_STRING("a"));
                arg = arg->next;
                EXIT_IF(arg->value != GET_STRING("b"));
                arg = arg->next;
                EXIT_IF(arg->value != GET_STRING("c"));
                EXIT_IF(arg->next);
            }
            {
                Expr* expr = memory->funcs.items[1].expr;
                EXIT_IF(expr->tag != EXPR_APP);
                EXIT_IF(expr->body.as_app[0]->tag != EXPR_APP);
                {
                    Expr* l0 = expr->body.as_app[0];
                    EXIT_IF(l0->tag != EXPR_APP);
                    {
                        Expr* l1 = l0->body.as_app[0];
                        EXIT_IF(l1->tag != EXPR_BINOP);
                        EXIT_IF(l1->body.as_binop != BINOP_ADD);
                    }
                    {
                        Expr* r1 = l0->body.as_app[1];
                        EXIT_IF(r1->tag != EXPR_APP);
                        {
                            Expr* l2 = r1->body.as_app[0];
                            EXIT_IF(l2->tag != EXPR_APP);
                            {
                                Expr* l3 = l2->body.as_app[0];
                                EXIT_IF(l3->tag != EXPR_BINOP);
                                EXIT_IF(l3->body.as_binop != BINOP_SUB);
                            }
                            {
                                Expr* r3 = l2->body.as_app[1];
                                EXIT_IF(r3->tag != EXPR_VAR);
                                EXIT_IF(r3->body.as_var != GET_STRING("a"));
                            }
                        }
                        {
                            Expr* r2 = r1->body.as_app[1];
                            EXIT_IF(r2->tag != EXPR_VAR);
                            EXIT_IF(r2->body.as_var != GET_STRING("b"));
                        }
                    }
                }
                {
                    Expr* r0 = expr->body.as_app[1];
                    EXIT_IF(r0->tag != EXPR_VAR);
                    EXIT_IF(r0->body.as_var != GET_STRING("c"));
                }
            }
        }
        {
            EXIT_IF(memory->funcs.items[2].tag != FUNC_VAR);
            EXIT_IF(memory->funcs.items[2].name.as_var != GET_STRING("h"));
            EXIT_IF(memory->funcs.items[2].args.first);
            {
                Expr* expr = memory->funcs.items[2].expr;
                EXIT_IF(expr->tag != EXPR_PACK);
                EXIT_IF(expr->body.as_pack[0] != 1);
                EXIT_IF(expr->body.as_pack[1] != 0);
            }
        }
        fprintf(stderr, ".");
    }
    fprintf(stderr, "\n");
}

#endif
