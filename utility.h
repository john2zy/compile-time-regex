#ifndef CTRE_UTILITY_H
#define CTRE_UTILITY_H

template <typename T1, typename T2>
struct is_same_t {
    enum {
        result = 0
    };
};

template <typename T>
struct is_same_t<T, T> {
    enum {
        result = 1
    };
};

//  just call it on the varible you want to print
//  and use grep to grab the error meassages:
//      g++ ... 2>&1 | grep error(.*)ctprint
//  compiler should tell you what the type of parameter is
template <typename>
void ctprint(void) {
    return;
}

#endif