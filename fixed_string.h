#ifndef CTRE_FIXED_STRING_H
#define CTRE_FIXED_STRING_H

template <int N>
class fixed_string {
  private:
    char data[N] = {};

  public:
    constexpr fixed_string(const char (&str)[N]) {
        for (int i = 0; i < N; i++) {
            data[i] = str[i];
        }
    }
    constexpr int size() const {
        return N;
    }

    constexpr char operator[](int idx) const {
        return data[idx];
    }
};

// deduction guide
template <int N>
fixed_string(const char str[N]) -> fixed_string<N>;

#endif