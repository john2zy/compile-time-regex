#ifndef CTRE_STACK_H
#define CTRE_STACK_H

//  the top is on the left side of parameter packs
template <typename... Ts>
class stack {};

//  return type when stack is empty
struct stack_empty {};

//  Since we don't want to pass in template parameter like
//  this: decltype(top(st)), we add an empty return
//  statement to each function so that it actually returns
//  an object.

template <typename... Ts, typename... OBJs>
constexpr auto push(stack<Ts...>, OBJs...) -> stack<OBJs..., Ts...> {
    return {};
}

template <typename T, typename... Ts>
constexpr auto pop(stack<T, Ts...>) -> stack<Ts...> {
    return {};
}

//  We need to pop on empty stack at least once in the last
//  step of parsing, so we just ignore this error.
constexpr auto pop(stack<>) -> stack<> {
    return {};
}

template <typename T, typename... Ts>
constexpr auto top(stack<T, Ts...>) -> T {
    return {};
}

constexpr auto top(stack<>) -> stack_empty {
    return {};
}

#endif