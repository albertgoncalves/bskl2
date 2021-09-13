#include "parse.hpp"

#define CAP_LIST_STRINGS (1 << 5)
#define CAP_TOKENS       (1 << 5)
#define CAP_STRINGS      (1 << 5)
#define CAP_BINDINGS     (1 << 5)
#define CAP_UNPACKS      (1 << 5)
#define CAP_EXPRS        (1 << 5)
#define CAP_FUNCS        (1 << 5)

struct Memory {
    Buffer<ListNode<String>, CAP_LIST_STRINGS> list_strings;
    Buffer<Token, CAP_TOKENS>                  tokens;
    ParseMemory<CAP_STRINGS, CAP_BINDINGS, CAP_UNPACKS, CAP_EXPRS, CAP_FUNCS>
        parse_memory;
};

template <usize N>
static void demo_list(Buffer<ListNode<String>, N>* list_strings) {
    List<String> a = {};
    List<String> b = {};
    println(stdout, &a);
    append(list_strings, &a, GET_STRING("a"));
    append(list_strings, &a, GET_STRING("b"));
    append(list_strings, &a, GET_STRING("c"));
    append(list_strings, &b, GET_STRING("d"));
    append(list_strings, &b, GET_STRING("e"));
    append(list_strings, &b, GET_STRING("f"));
    println(stdout, &a);
    concat(&a, &b);
    println(stdout, &a);
}

i32 main() {
    printf("\n"
           "sizeof(String)           : %zu\n"
           "sizeof(List<String>)     : %zu\n"
           "sizeof(ListNode<String>) : %zu\n"
           "sizeof(Token)            : %zu\n"
           "sizeof(ExprBinding)      : %zu\n"
           "sizeof(ExprLet)          : %zu\n"
           "sizeof(ExprTag)          : %zu\n"
           "sizeof(ExprBody)         : %zu\n"
           "sizeof(Expr)             : %zu\n"
           "sizeof(Func)             : %zu\n"
           "sizeof(Memory)           : %zu\n"
           "\n",
           sizeof(String),
           sizeof(List<String>),
           sizeof(ListNode<String>),
           sizeof(Token),
           sizeof(ExprBinding),
           sizeof(ExprLet),
           sizeof(ExprTag),
           sizeof(ExprBody),
           sizeof(Expr),
           sizeof(Func),
           sizeof(Memory));
    Memory* memory = reinterpret_cast<Memory*>(calloc(1, sizeof(Memory)));
    test_set_tokens(&memory->tokens);
    demo_list(&memory->list_strings);
    test_parse_program(&memory->tokens, &memory->parse_memory);
    free(memory);
    printf("Done!\n");
    return EXIT_SUCCESS;
}
