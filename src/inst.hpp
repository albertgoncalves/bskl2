#ifndef __INST_H__
#define __INST_H__

#include "hash.hpp"
#include "list.hpp"

enum InstTag {
    INST_UNWIND = 0,

    INST_PUSH_GLOBAL,
    INST_PUSH_INT,
    INST_PUSH_UNDEF,
    INST_PUSH,

    INST_APP,

    INST_UPDATE,
    INST_POP,
    INST_ALLOC,
    INST_SLIDE,
    INST_EVAL,

    INST_PACK,
    INST_JUMP,
    INST_SPLIT,

    INST_COND,

    INST_ADD,
    INST_SUB,
    INST_MUL,
    INST_DIV,

    INST_EQ,
    INST_NE,
    INST_LT,
    INST_LE,
    INST_GT,
    INST_GE,

    INST_OR,
    INST_AND,
};

typedef struct Inst Inst;

struct InstCond {
    List<Inst> insts[2];
};

struct InstPack {
    u8 tag;
    u8 arity;
};

union InstBody {
    String            as_string;
    i64               as_i64;
    InstCond          as_cond;
    InstPack          as_pack;
    const List<Inst>* as_jump;
};

struct Inst {
    InstBody body;
    InstTag  tag;
};

typedef struct Node Node;

struct InstFrame {
    List<Inst>  insts;
    List<Node*> nodes;
};

enum NodeTag {
    NODE_UNDEF = 0,
    NODE_I64,
    NODE_APP,
    NODE_GLOBAL,
    NODE_INDIR,
    NODE_DATA,
};

struct NodeGlobal {
    List<Inst> insts;
    u8         arity;
};

struct NodePack {
    List<Node*> as_nodes;
    u8          tag;
};

union NodeBody {
    i64        as_i64;
    Node*      as_app[2];
    NodeGlobal as_global;
    Node*      as_indir;
    NodePack   as_pack;
};

struct Node {
    NodeBody body;
    NodeTag  tag;
};

template <usize I, usize L, usize N, usize F, usize G>
struct InstMemory {
    Buffer<Inst, I>                insts;
    Buffer<List<Inst>, L>          lists;
    Buffer<ListNode<Inst>, N>      nodes;
    Buffer<ListNode<InstFrame>, F> frames;
    Table<String, Node*, G>        globals;
};

#endif
