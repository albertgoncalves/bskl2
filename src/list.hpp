#ifndef __LIST_H__
#define __LIST_H__

#include "buffer.hpp"

template <typename T>
struct ListNode {
    T            value;
    ListNode<T>* next;
};

template <typename T>
struct List {
    ListNode<T>* first;
    ListNode<T>* last;
};

template <typename T, usize N>
static void append(Buffer<ListNode<T>, N>* buffer, List<T>* list, T value) {
    ListNode<T>* node = alloc(buffer);
    node->value = value;
    node->next = null;
    if (!list->first) {
        list->first = node;
        list->last = node;
        return;
    }
    list->last->next = node;
    list->last = node;
}

template <typename T>
static void concat(List<T>* a, List<T>* b) {
    a->last->next = b->first;
    a->last = b->last;
}

template <typename T>
static void println(File* stream, List<T>* list) {
    fprintf(stream, "[");
    ListNode<T>* node = list->first;
    if (node) {
        print(stream, node->value);
        for (;;) {
            node = node->next;
            if (!node) {
                break;
            }
            fprintf(stream, ", ");
            print(stream, node->value);
        }
    }
    fprintf(stream, "]\n");
}

#endif
