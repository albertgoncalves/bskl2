#include "lang.hpp"
#include "parse.hpp"
#include "string.hpp"

#define CAP_LIST_STRINGS  (1 << 5)
#define CAP_LIST_BINDINGS (1 << 5)
#define CAP_TOKENS        (1 << 5)

struct Memory {
    Buffer<ListNode<String>, CAP_LIST_STRINGS>       list_strings;
    Buffer<ListNode<ExprBinding>, CAP_LIST_BINDINGS> list_bindings;
    Buffer<Token, CAP_TOKENS>                        tokens;
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
           "sizeof(String)       : %zu\n"
           "sizeof(List<String>) : %zu\n"
           "sizeof(Token)        : %zu\n"
           "sizeof(ExprBinding)  : %zu\n"
           "sizeof(ExprLet)      : %zu\n"
           "sizeof(ExprTag)      : %zu\n"
           "sizeof(ExprBody)     : %zu\n"
           "sizeof(Expr)         : %zu\n"
           "sizeof(Memory)       : %zu\n"
           "\n",
           sizeof(String),
           sizeof(List<String>),
           sizeof(Token),
           sizeof(ExprBinding),
           sizeof(ExprLet),
           sizeof(ExprTag),
           sizeof(ExprBody),
           sizeof(Expr),
           sizeof(Memory));
    Memory* memory = reinterpret_cast<Memory*>(calloc(1, sizeof(Memory)));
    test_set_tokens(&memory->tokens);
    demo_list(&memory->list_strings);
    free(memory);
    printf("Done!\n");
    return EXIT_SUCCESS;
}
