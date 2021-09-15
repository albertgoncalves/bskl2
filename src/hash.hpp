#ifndef __HASH_H__
#define __HASH_H__

#include "string.hpp"

template <typename K, typename V>
struct Item {
    K    key;
    V    value;
    bool alive;
};

template <typename K, typename V, usize N>
struct Table {
    Item<K, V> items[N];
    usize      len;
};

#define FNV_32_PRIME        16777619u
#define FNV_32_OFFSET_BASIS 2166136261u

static u32 fnv_1a_32(const u8* bytes, usize len) {
    u32 hash = FNV_32_OFFSET_BASIS;
    for (usize i = 0; i < len; ++i) {
        hash ^= bytes[i];
        hash *= FNV_32_PRIME;
    }
    return hash;
}

static u32 hash(String string) {
    return fnv_1a_32(reinterpret_cast<const u8*>(string.chars), string.len);
}

template <typename K, typename V, usize N>
static u32 find_slot(Table<K, V, N>* table, K key) {
    u32 h = hash(key);
    for (u32 i = 0; i < N; ++i) {
        u32 j = (h + i) % N;
        if ((!table->items[j].alive) || (table->items[j].key == key)) {
            return j;
        }
    }
    EXIT();
}

template <typename K, typename V, usize N>
static V* lookup(Table<K, V, N>* table, K key) {
    Item<K, V>* item = &table->items[find_slot(table, key)];
    if (item->alive) {
        return &item->value;
    }
    return null;
}

template <typename K, typename V, usize N>
static void insert(Table<K, V, N>* table, K key, V value) {
    EXIT_IF(N <= table->len);
    u32 i = find_slot(table, key);
    if (!table->items[i].alive) {
        ++table->len;
    }
    table->items[i] = {key, value, true};
    return;
}

template <typename K, typename V, usize N>
static void remove(Table<K, V, N>* table, K key) {
    u32 i = find_slot(table, key);
    if (!table->items[i].alive) {
        return;
    }
    u32 j = i;
    --table->len;
    for (;;) {
        table->items[i].alive = false;
        for (;;) {
            j = (j + 1) % N;
            if (!table->items[j].alive) {
                return;
            }
            u32 h = hash(table->items[j].key) % N;
            if (!(i <= j ? (i < h) && (h <= j) : (i < h) || (h <= j))) {
                break;
            }
        }
        table->items[i] = table->items[j];
        i = j;
    }
}

#endif
