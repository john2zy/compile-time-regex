#ifndef CTRE_PARSER_H
#define CTRE_PARSER_H

#include "fixed_string.h"
#include "parse_table.h"
#include "stack.h"
#include "utility.h"

template <auto& fstr, typename Grammar>
class parser {
  private:
    ////// helpers
    // We need to add an EOF symbol to the end of the input string.
    // It is ok to use epsilon as EOF.
    template <int IDX>
    static constexpr auto fstr_at() {
        if constexpr (IDX < fstr.size())
            return character<fstr[IDX]>{};
        else
            return epsilon{};
    }

    template <typename T>
    static constexpr bool is_AST_action(T) {
        return std::is_base_of<AST_action, T>::value;
    }

    //////
    // parser entrance
    //
    // Since f has no definition, decltype(...){} here is
    // necessary to instantiate an object for deduction. We
    // can also add a "return {}" definition to f, so we can
    // use f(arg1, arg2) directly. But I'm too lazy to copy
    // that many lines.
    template <int IDX = 0, typename StackT, typename ASTT>
    static constexpr auto parse(StackT st, ASTT ast) {
        auto symbol = top(st);

        if constexpr (is_AST_action(symbol)) {
            auto new_ast = decltype(Grammar::build_AST()){};

            return parse<IDX>(pop(st), new_ast);
        } else {
            auto op = decltype(Grammar::f(symbol, fstr_at<IDX>())){};

            return next_op<IDX>(op, pop(st), ast);
        }
    }

    // table entry: epsilon
    // do nothing, carry on to next symbol
    template <int IDX, typename StackT, typename ASTT>
    static constexpr auto next_op(pass, StackT st, ASTT ast) {
        return parse<IDX>(st, ast);
    }

    // table entry: terminals
    // pop input
    template <int IDX, typename StackT, typename ASTT>
    static constexpr auto next_op(pop_input, StackT st, ASTT ast) {
        return parse<IDX + 1>(st, ast);
    }

    // table entry: stuff to push
    template <int IDX, typename StackT, typename... Ts, typename ASTT>
    static constexpr auto next_op(stack<Ts...>, StackT st, ASTT ast) {
        return parse<IDX>(push(st, Ts{}...), ast);
    }

    // getting results
    template <int IDX, typename StackT, typename ASTT>
    static constexpr auto next_op(accept, StackT, ASTT ast) {
        return std::make_pair(true, ast);
    }

    template <int IDX, typename StackT, typename ASTT>
    static constexpr auto next_op(reject, StackT, ASTT ast) {
        return std::make_pair(false, ast);
    }

    // starting symbol
    using S = typename Grammar::S;

  public:
    static constexpr auto result = parser::parse(stack<S>{}, stack<>{});

    static constexpr auto correct = result.first;
    static constexpr auto AST     = result.second;
};

#endif