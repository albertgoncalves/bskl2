#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "prelude.hpp"

template <typename T, usize N>
struct Buffer {
    T     items[N];
    usize len;
};

template <typename T, usize N>
static T* alloc(Buffer<T, N>* buffer) {
    EXIT_IF(N <= buffer->len);
    return &buffer->items[buffer->len++];
}

#endif
