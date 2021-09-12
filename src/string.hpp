#ifndef __STRING_H__
#define __STRING_H__

#include "prelude.hpp"

#include <string.h>

struct String {
    const char* chars;
    usize       len;
};

#define GET_STRING(literal)  \
    ((String){               \
        literal,             \
        sizeof(literal) - 1, \
    })

static void print(File* stream, String string) {
    fprintf(stream, "\"%.*s\"", static_cast<i32>(string.len), string.chars);
}

static bool operator==(String a, String b) {
    return (a.len == b.len) && (!memcmp(a.chars, b.chars, a.len));
}

static bool operator!=(String a, String b) {
    return !(a == b);
}

#endif
