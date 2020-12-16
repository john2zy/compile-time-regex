# CTRE

My recreation of [compile time regex library](https://github.com/hanickadot/compile-time-regular-expressions) (CTRE) for learning purposes.

I followed the essence of the original author's [presentations](https://www.compile-time.re).

## Usage

Since this is targeted at C++17, we need a fixed string object with linkage to pass in the pattern.

Only supports `*` `+` `|`.

```c++
#include <match.h>

static constexpr fixed_string fstr("a(ab|cd)+");
bool result = match<fstr>("acdabab");
```