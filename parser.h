#ifndef CTRE_PARSER_H
#define CTRE_PARSER_H

#include "fixed_string.h"
#include "parse_table.h"
#include "stack.h"

template <auto& fstr, typename Grammar>
struct parser {
    // starting symbol
    using S = typename Grammar::S;

    static constexpr bool result = parser::parse(stack<S>{});

    //
    //
    // We need to add an EOF symbol to the end of the input string.
    // It is ok to use epsilon as EOF.
    template <int IDX>
    static constexpr auto fstr_at() {
        if constexpr (IDX < fstr.size())
            return ch<fstr[IDX]>{};
        else
            return epsilon{};
    }

    // parser entrance
    template <int IDX = 0, typename StackT>
    static constexpr bool parse(StackT st) {
        // Since f has no definition, decltype here is
        // necessary to instantiate an object for deduction.
        // We can also add a "return {}" definition to f, so
        // we can use f(arg1, arg2) directly. But I'm too
        // lazy to copy that many lines.
        auto op = decltype(Grammar::f(top(st), fstr_at<IDX>())){};

        return next_op<IDX>(op, pop(st));
    }

    // table entry: epsilon
    template <int IDX, typename StackT>
    static constexpr bool next_op(pass, StackT st) {
        return parse<IDX>(st);
    }

    // table entry: terminals
    template <int IDX, typename StackT>
    static constexpr bool next_op(pop_input, StackT st) {
        return parse<IDX + 1>(st);
    }

    // table entry: stuff to push
    template <int IDX, typename StackT, typename... Ts>
    static constexpr bool next_op(stack<Ts...>, StackT st) {
        return parse<IDX>(push(st, Ts{}...));
    }

    // getting results
    template <int IDX, typename StackT>
    static constexpr bool next_op(accept, StackT) {
        return true;
    }

    template <int IDX, typename StackT>
    static constexpr bool next_op(reject, StackT) {
        return false;
    }
};

#endif