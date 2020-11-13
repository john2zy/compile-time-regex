#ifndef CTRE_UTILITY_H
#define CTRE_UTILITY_H

#include <type_traits>
#include <utility>

//  just call it on the varible you want to print
//  and use grep to grab the error meassages:
//      g++ ... 2>&1 | grep error(.*)ctprint
//  compiler should tell you what the type of parameter is
template <typename>
void ctprint(void) {
    return;
}

#endif