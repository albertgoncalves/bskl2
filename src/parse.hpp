#ifndef __PARSE_H__
#define __PARSE_H__

#include "buffer.hpp"
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
static void set_tokens(String source, Buffer<Token, N>* buffer) {
    buffer->len = 0;
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
            Token* token = alloc(buffer);
            token->tag = TOKEN_LPAREN;
            token->offset = i++;
            break;
        }
        case ')': {
            Token* token = alloc(buffer);
            token->tag = TOKEN_RPAREN;
            token->offset = i++;
            break;
        }
        case '{': {
            Token* token = alloc(buffer);
            token->tag = TOKEN_LBRACE;
            token->offset = i++;
            break;
        }
        case '}': {
            Token* token = alloc(buffer);
            token->tag = TOKEN_RBRACE;
            token->offset = i++;
            break;
        }
        case ';': {
            Token* token = alloc(buffer);
            token->tag = TOKEN_SCOLON;
            token->offset = i++;
            break;
        }
        case '=': {
            Token* token = alloc(buffer);
            token->tag = TOKEN_ASSIGN;
            token->offset = i++;
            if ((i < source.len) && (source.chars[i] == '=')) {
                token->tag = TOKEN_EQ;
                ++i;
            }
            break;
        }
        case '!': {
            Token* token = alloc(buffer);
            token->tag = TOKEN_NE;
            token->offset = i++;
            EXIT_IF(source.len <= i);
            EXIT_IF(source.chars[i] == '=');
            ++i;
            break;
        }
        case '+': {
            Token* token = alloc(buffer);
            token->tag = TOKEN_ADD;
            token->offset = i++;
            break;
        }
        case '-': {
            Token* token = alloc(buffer);
            token->tag = TOKEN_SUB;
            token->offset = i++;
            break;
        }
        case '*': {
            Token* token = alloc(buffer);
            token->tag = TOKEN_MUL;
            token->offset = i++;
            break;
        }
        case '/': {
            Token* token = alloc(buffer);
            token->tag = TOKEN_DIV;
            token->offset = i++;
            break;
        }
        case '<': {
            Token* token = alloc(buffer);
            token->tag = TOKEN_LT;
            token->offset = i++;
            if ((i < source.len) && (source.chars[i] == '=')) {
                token->tag = TOKEN_LE;
                ++i;
            }
            break;
        }
        case '>': {
            Token* token = alloc(buffer);
            token->tag = TOKEN_GT;
            token->offset = i++;
            if ((i < source.len) && (source.chars[i] == '=')) {
                token->tag = TOKEN_GE;
                ++i;
            }
            break;
        }
        case '&': {
            Token* token = alloc(buffer);
            token->tag = TOKEN_AND;
            token->offset = i++;
            break;
        }
        case '|': {
            Token* token = alloc(buffer);
            token->tag = TOKEN_OR;
            token->offset = i++;
            break;
        }
        default: {
            EXIT_IF(!(IS_ALPHA_OR_DIGIT_OR_PUNCT(source.chars[i])));
            Token* token = alloc(buffer);
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

#endif
