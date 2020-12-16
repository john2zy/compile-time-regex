#ifndef CTRE_MATCH_H
#define CTRE_MATCH_H

#include "finite_automata.h"
#include "parser.h"
#include <stack>
#include <string>

template <auto& pattern>
bool match(const std::string& target_str) {
    constexpr auto ast = typename parser<pattern, parse_table>::AST{};
    static_assert(parser<pattern, parse_table>::correct, "Regular expression syntax error");

    constexpr auto nfa = build_FA(ast);
    // nfa.print();

    // state number, index in target str
    std::stack<std::pair<int, int>> st;
    st.push(std::make_pair(0, 0));
    while (!st.empty()) {
        auto [state, idx] = st.top();
        st.pop();

        // target_str[0, idx) is matched
        if (idx == target_str.size()) {
            if (nfa.is_final_state(state)) {
                return true;
            }
        }

        int idx_trans = nfa.lower_idx_in_trans(state);
        if (idx_trans < 0)
            continue;

        while (idx_trans < nfa.size_transition() && nfa.transitions[idx_trans].src == state) {
            const transition& trans = nfa.transitions[idx_trans];

            if (trans.is_epsilon) {
                st.push(std::make_pair(trans.dst, idx));
            } else if (trans.match(target_str[idx])) {
                st.push(std::make_pair(trans.dst, idx + 1));
            }

            idx_trans++;
        }
    }

    return false;
}

#endif