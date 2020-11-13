// Grammar:
// S -> E $  $ - the special EOF symbol

// E ->  character mod seq alt
// E -> ( alt0 ) mod seq alt
// E -> epsilon

// seq0 -> character mod seq
// seq0 -> ( alt0 ) mod seq
// seq -> ( alt0 ) mod seq
// seq -> character mod seq
// seq -> epsilon

// alt0 -> character mod seq alt
// alt0 -> ( alt0 ) mod seq alt
// alt -> | seq0 alt
// alt -> epsilon

// mod -> *
// mod -> +
// mod -> ?
// mod -> epsilon

#ifndef CTRE_PARSE_TABLE_H
#define CTRE_PARSE_TABLE_H

#include "stack.h"

// parser operations
struct reject {};
struct accept {};
struct pass {};
struct pop_input {};

// terminal wrapper
template <char C>
struct character {};

struct epsilon {};

// AST action base type
struct AST_action {};

//
// AST types
//

// single char
template <char C>
struct ch {};

// |
template <typename... Ts>
struct alter {};

// cdot
template <typename... Ts>
struct concat {};

// *
template <typename T>
struct star {};

// +
template <typename T>
using plus = concat<T, star<T>>;

// ?
template <typename T>
using opt = alter<epsilon, T>;

//
//
//

struct parse_table {
    // non-terminals
    struct E {};
    struct alt0 {};
    struct alt {};
    struct seq0 {};
    struct seq {};
    struct mod {};

    // the starting symbol
    using S = E;

    //
    // AST actions
    //

    struct _char : AST_action {};  // push char
    struct _concat : AST_action {};
    struct _alter : AST_action {};
    struct _star : AST_action {};
    struct _plus : AST_action {};
    struct _opt : AST_action {};

    //
    // AST builder
    //

    template <char C, typename... Ts>
    static auto build_AST(_char, character<C> _pre_char, stack<Ts...> _ast) -> stack<ch<C>, Ts...>;

    template <char C, typename T1, typename T2, typename... Ts>
    static auto build_AST(_concat, character<C>, stack<T1, T2, Ts...>) -> stack<concat<T2, T1>, Ts...>;

    template <char C, typename T, typename... Ts1, typename... Ts2>
    static auto build_AST(_concat, character<C>, stack<T, concat<Ts1...>, Ts2...>) -> stack<concat<Ts1..., T>, Ts2...>;

    template <char C, typename T1, typename T2, typename... Ts>
    static auto build_AST(_alter, character<C>, stack<T1, T2, Ts...>) -> stack<alter<T2, T1>, Ts...>;

    template <char C, typename T, typename... Ts1, typename... Ts2>
    static auto build_AST(_alter, character<C>, stack<T, alter<Ts1...>, Ts2...>) -> stack<alter<Ts1..., T>, Ts2...>;

    template <char C, typename T, typename... Ts>
    static auto build_AST(_star, character<C>, stack<T, Ts...>) -> stack<star<T>, Ts...>;

    template <char C, typename T, typename... Ts>
    static auto build_AST(_plus, character<C>, stack<T, Ts...>) -> stack<plus<T>, Ts...>;

    template <char C, typename T, typename... Ts>
    static auto build_AST(_opt, character<C>, stack<T, Ts...>) -> stack<opt<T>, Ts...>;

    //
    // the parse table
    //

    //////
    // if the same terminal, then pop 1 input char
    // poping is handled by the parser itself
    template <char C>
    static auto f(character<C>, character<C>) -> pop_input;

    //////
    // E
    static auto f(E, character<'('>) -> stack<character<'('>, alt0, character<')'>, mod, seq, alt>;

    template <char C>
    static auto f(E, character<C>) -> stack<character<C>, _char, mod, seq, alt>;

    static auto f(E, epsilon) -> pass;

    static auto f(E, character<')'>) -> reject;
    static auto f(E, character<'*'>) -> reject;
    static auto f(E, character<'+'>) -> reject;
    static auto f(E, character<'?'>) -> reject;
    static auto f(E, character<'|'>) -> reject;

    //////
    // alt0
    static auto f(alt0, character<'('>) -> stack<character<'('>, alt0, character<')'>, mod, seq, alt>;

    template <char C>
    static auto f(alt0, character<C>) -> stack<character<C>, _char, mod, seq, alt>;

    static auto f(alt0, character<')'>) -> reject;
    static auto f(alt0, character<'*'>) -> reject;
    static auto f(alt0, character<'+'>) -> reject;
    static auto f(alt0, character<'?'>) -> reject;
    static auto f(alt0, character<'|'>) -> reject;
    static auto f(alt0, epsilon) -> reject;

    //////
    // alt
    static auto f(alt, character<'|'>) -> stack<character<'|'>, seq0, _alter, alt>;

    static auto f(alt, character<')'>) -> pass;
    static auto f(alt, epsilon) -> pass;

    static auto f(alt, character<'('>) -> reject;
    static auto f(alt, character<'*'>) -> reject;
    static auto f(alt, character<'+'>) -> reject;
    static auto f(alt, character<'?'>) -> reject;

    template <char C>
    static auto f(alt, character<C>) -> reject;

    //////
    // mod
    static auto f(mod, character<'+'>) -> stack<character<'+'>, _plus>;
    static auto f(mod, character<'?'>) -> stack<character<'?'>, _opt>;
    static auto f(mod, character<'*'>) -> stack<character<'*'>, _star>;

    static auto f(mod, character<'('>) -> pass;
    static auto f(mod, character<')'>) -> pass;
    static auto f(mod, character<'|'>) -> pass;

    template <char C>
    static auto f(mod, character<C>) -> pass;

    static auto f(mod, epsilon) -> pass;

    //////
    // seq0
    static auto f(seq0, character<'('>) -> stack<character<'('>, alt0, character<')'>, mod, seq>;

    template <char C>
    static auto f(seq0, character<C>) -> stack<character<C>, _char, mod, seq>;

    static auto f(seq0, character<')'>) -> reject;
    static auto f(seq0, character<'*'>) -> reject;
    static auto f(seq0, character<'+'>) -> reject;
    static auto f(seq0, character<'?'>) -> reject;
    static auto f(seq0, character<'|'>) -> reject;
    static auto f(seq0, epsilon) -> reject;

    //////
    // seq
    static auto f(seq, character<'('>) -> stack<character<'('>, alt0, character<')'>, mod, _concat, seq>;

    static auto f(seq, character<')'>) -> pass;
    static auto f(seq, character<'|'>) -> pass;
    static auto f(seq, epsilon) -> pass;

    template <char C>
    static auto f(seq, character<C>) -> stack<character<C>, _char, mod, _concat, seq>;

    static auto f(seq, character<'*'>) -> reject;
    static auto f(seq, character<'+'>) -> reject;
    static auto f(seq, character<'?'>) -> reject;

    //////
    // accept & reject
    static auto f(stack_empty, epsilon) -> accept;
    static auto f(...) -> reject;
};

#endif