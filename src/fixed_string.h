#ifndef CTRE_FIXED_STRING_H
#define CTRE_FIXED_STRING_H

template <int N>
class fixed_string {
  private:
    // exclude "\0"
    char data[N - 1] = {};

  public:
    constexpr fixed_string(const char (&str)[N]) {
        for (int i = 0; i < N - 1; i++) {
            data[i] = str[i];
        }
    }
    constexpr int size() const {
        return N - 1;
    }

    constexpr char operator[](int idx) const {
        return data[idx];
    }
};

// deduction guide
template <int N>
fixed_string(const char str[N]) -> fixed_string<N>;

#endif