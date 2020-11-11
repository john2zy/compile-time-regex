// Grammar:
// S -> E $  $ - the special EOF symbol

// E ->  ch mod seq alt
// E -> ( alt0 ) mod seq alt
// E -> epsilon

// seq0 -> ch mod seq
// seq0 -> ( alt0 ) mod seq
// seq -> ( alt0 ) mod seq
// seq -> ch mod seq
// seq -> epsilon

// alt0 -> ch mod seq alt
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

template <char C>
struct ch {};

struct epsilon {};

// operations
struct reject {};
struct accept {};
struct pass {};
struct pop_input {};

struct parse_table {
    struct E {};
    struct alt0 {};
    struct alt {};
    struct seq0 {};
    struct seq {};
    struct mod {};

    // the starting symbol
    using S = E;

    //
    // parse table entries
    //

    //////
    // if the same terminal, then pop 1 input char
    // poping is handled by the parser itself
    template <char C>
    static auto f(ch<C>, ch<C>) -> pop_input;

    //////
    // E
    static auto f(E, ch<'('>) -> stack<ch<'('>, alt0, ch<')'>, mod, seq, alt>;

    template <char C>
    static auto f(E, ch<C>) -> stack<ch<C>, mod, seq, alt>;

    static auto f(E, epsilon) -> pass;

    static auto f(E, ch<')'>) -> reject;
    static auto f(E, ch<'*'>) -> reject;
    static auto f(E, ch<'+'>) -> reject;
    static auto f(E, ch<'?'>) -> reject;
    static auto f(E, ch<'|'>) -> reject;

    //////
    // alt0
    static auto f(alt0, ch<'('>) -> stack<ch<'('>, alt0, ch<')'>, mod, seq, alt>;

    template <char C>
    static auto f(alt0, ch<C>) -> stack<ch<C>, mod, seq, alt>;

    static auto f(alt0, ch<')'>) -> reject;
    static auto f(alt0, ch<'*'>) -> reject;
    static auto f(alt0, ch<'+'>) -> reject;
    static auto f(alt0, ch<'?'>) -> reject;
    static auto f(alt0, ch<'|'>) -> reject;
    static auto f(alt0, epsilon) -> reject;

    //////
    // alt
    static auto f(alt, ch<'|'>) -> stack<ch<'|'>, seq0, alt>;

    static auto f(alt, ch<')'>) -> pass;
    static auto f(alt, epsilon) -> pass;

    static auto f(alt, ch<'('>) -> reject;
    static auto f(alt, ch<'*'>) -> reject;
    static auto f(alt, ch<'+'>) -> reject;
    static auto f(alt, ch<'?'>) -> reject;

    template <char C>
    static auto f(alt, ch<C>) -> reject;

    //////
    // mod
    static auto f(mod, ch<'+'>) -> stack<ch<'+'>>;
    static auto f(mod, ch<'?'>) -> stack<ch<'?'>>;
    static auto f(mod, ch<'*'>) -> stack<ch<'*'>>;

    static auto f(mod, ch<'('>) -> pass;
    static auto f(mod, ch<')'>) -> pass;
    static auto f(mod, ch<'|'>) -> pass;

    template <char C>
    static auto f(mod, ch<C>) -> pass;

    static auto f(mod, epsilon) -> pass;

    //////
    // seq0
    static auto f(seq0, ch<'('>) -> stack<ch<'('>, alt0, ch<')'>, mod, seq>;

    template <char C>
    static auto f(seq0, ch<C>) -> stack<ch<C>, mod, seq>;

    static auto f(seq0, ch<')'>) -> reject;
    static auto f(seq0, ch<'*'>) -> reject;
    static auto f(seq0, ch<'+'>) -> reject;
    static auto f(seq0, ch<'?'>) -> reject;
    static auto f(seq0, ch<'|'>) -> reject;
    static auto f(seq0, epsilon) -> reject;

    //////
    // seq
    static auto f(seq, ch<'('>) -> stack<ch<'('>, alt0, ch<')'>, mod, seq>;

    static auto f(seq, ch<')'>) -> pass;
    static auto f(seq, ch<'|'>) -> pass;
    static auto f(seq, epsilon) -> pass;

    template <char C>
    static auto f(seq, ch<C>) -> stack<ch<C>, mod, seq>;

    static auto f(seq, ch<'*'>) -> reject;
    static auto f(seq, ch<'+'>) -> reject;
    static auto f(seq, ch<'?'>) -> reject;

    //////
    // accept & reject
    static auto f(stack_empty, epsilon) -> accept;
    static auto f(...) -> reject;
};

#endif