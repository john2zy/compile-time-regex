#ifndef CTRE_FINITE_AUTOMATA_H
#define CTRE_FINITE_AUTOMATA_H

#include "array.h"
#include "parse_table.h"  // for AST types
#include <iostream>

struct transition {
    int  src;
    int  dst;
    char char_to_match;
    bool is_epsilon;

    constexpr transition(int src = -1, int dst = -1, char c = '\0')
        : src(src), dst(dst), char_to_match(c), is_epsilon(c == '\0') {}

    constexpr bool match(char c) const {
        return c == char_to_match;
    }

    void print() const {
        if (is_epsilon)
            printf("%d --epsilon--> %d\n", src, dst);
        else
            printf("%d --%c--> %d\n", src, char_to_match, dst);
    }
};

template <int N_T, int N_FS>
class finite_automata {
  private:
    int idx_t = 0, idx_fs = 0;

  public:
    array<transition, N_T> transitions;
    array<int, N_FS>       final_states;

    constexpr finite_automata() {}

    constexpr finite_automata(array<transition, N_T> t, array<int, N_FS> fs)
        : transitions(t), final_states(fs) {
        this->sort();
    }

    // only use this when the object is default constructed
    constexpr void add_transition(const transition& t) {
        transitions[idx_t] = t;
        idx_t++;
    }

    // only use this when the object is default constructed
    constexpr void add_final_state(int fs) {
        final_states[idx_fs] = fs;
        idx_fs++;
    }

    constexpr int state_count() const {
        int max = -1;
        for (const transition& t : transitions) {
            int tmp = t.src > t.dst ? t.src : t.dst;
            max     = tmp > max ? tmp : max;
        }
        return max + 1;
    }

    constexpr void sort() {
        transitions = transitions.sorted(
            [](const transition& lhs, const transition& rhs) {
                if (lhs.src != rhs.src)
                    return lhs.src < rhs.src;
                else
                    return lhs.dst < rhs.dst;
            });

        final_states = final_states.sorted(
            [](const int& lhs, const int& rhs) {
                return lhs < rhs;
            });
    }

    // Since transitions are sorted, we can search for next
    // state from lower bound idx. Find that idx here.
    constexpr int lower_idx_of_src(int src) const {
        int left = 0, right = N_T - 1;
        while (left <= right) {
            int         mid = (left + right) / 2;
            const auto& t   = transitions[mid];

            if (src == t.src &&
                ((mid >= 1 && src > transitions[mid - 1].src) || mid == 0))
                return mid;

            if (src > t.src) {
                left = mid + 1;
            } else if (src < t.src) {
                right = mid - 1;
            } else {
                right--;
            }
        }
        return false;
    }

    constexpr bool is_final_state(int fs) const {
        int left = 0, right = N_FS - 1;
        while (left <= right) {
            int mid = (left + right) / 2;
            if (fs == final_states[mid])
                return true;

            if (fs > final_states[mid]) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        return false;
    }

    void print() const {
        for (int i = 0; i < N_T; i++) {
            transitions[i].print();
        }
        printf("Final States: ");
        for (int i = 0; i < N_FS; i++) {
            printf("%d ", final_states[i]);
        }
        printf("\n\n");
    }
};

//
// Basic FAs
//

static constexpr finite_automata<0, 1> FA_epsilon{ {}, { 0 } };

template <char C>
static constexpr finite_automata<1, 1> FA_char{ { { { 0, 1, C } } }, { 1 } };

//
// FA connector
//

template <auto& LHS, auto& RHS, auto&... FAs>
struct FA_concat {
    template <int N_T1, int N_FS1, int N_T2, int N_FS2>
    static constexpr auto f(const finite_automata<N_T1, N_FS1>& lhs, const finite_automata<N_T2, N_FS2>& rhs) {
        finite_automata<N_T1 + N_T2 + N_FS1, N_FS2> res;
        int                                         l_st_cnt = lhs.state_count();

        // copy lhs's transitions
        for (transition t : lhs.transitions) {
            res.add_transition(t);
        }

        // copy rhs's transitions
        for (transition t : rhs.transitions) {
            t.src += l_st_cnt;
            t.dst += l_st_cnt;
            res.add_transition(t);
        }

        // connect lhs's final states to rhs
        for (int fs : lhs.final_states) {
            res.add_transition({ fs, l_st_cnt });
        }

        // copy final states
        for (int fs : rhs.final_states) {
            res.add_final_state(fs + l_st_cnt);
        }

        res.sort();
        return res;
    }

    template <typename T1, typename T2, typename... Ts>
    static constexpr auto f(T1 t1, T2 t2, Ts... ts) {
        return f(f(t1, t2), ts...);
    }

    static constexpr auto res = f(LHS, RHS, FAs...);
};

template <auto& LHS, auto& RHS, auto&... FAs>
struct FA_alter {
    template <int N_T1, int N_FS1, int N_T2, int N_FS2>
    static constexpr auto f(const finite_automata<N_T1, N_FS1>& lhs, const finite_automata<N_T2, N_FS2>& rhs) {
        finite_automata<N_T1 + N_T2, N_FS1 + N_FS2> res;
        int                                         l_st_cnt = lhs.state_count();

        // copy lhs's transitions
        for (transition t : lhs.transitions) {
            res.add_transition(t);
        }

        // copy rhs's transitions and merge starting states
        for (transition t : rhs.transitions) {
            if (t.src != 0)
                t.src += l_st_cnt - 1;
            if (t.dst != 0)
                t.dst += l_st_cnt - 1;

            res.add_transition(t);
        }

        // copy final states
        for (int fs : lhs.final_states) {
            res.add_final_state(fs);
        }
        for (int fs : rhs.final_states) {
            res.add_final_state(fs + l_st_cnt - 1);
        }

        res.sort();
        return res;
    }

    template <typename T1, typename T2, typename... Ts>
    static constexpr auto f(T1 t1, T2 t2, Ts... ts) {
        return f(f(t1, t2), ts...);
    }

    static constexpr auto res = f(LHS, RHS, FAs...);
};

template <auto& FA>
struct FA_star {
    template <int N_T, int N_FS>
    static constexpr auto f(const finite_automata<N_T, N_FS>& fa) {
        finite_automata<N_T + N_FS, N_FS> res;

        for (transition t : fa.transitions) {
            res.add_transition(t);
        }

        for (int fs : fa.final_states) {
            res.add_transition({ fs, 0 });
        }

        res.sort();
        return res;
    }

    static constexpr auto res = f(FA);
};

//
// FA builder
//

template <typename... Ts>
constexpr auto& build_FA(concat<Ts...>) {
    return FA_concat<build_FA(Ts{})...>::res;
}

template <typename... Ts>
constexpr auto& build_FA(alter<Ts...>) {
    return FA_alter<build_FA(Ts{})...>::res;
}

template <typename T>
constexpr auto& build_FA(star<T>) {
    return FA_star<build_FA(T{})>::res;
}

template <char C>
constexpr auto& build_FA(ch<C>) {
    return FA_char<C>;
}

constexpr auto& build_FA(epsilon) {
    return FA_epsilon;
}

#endif