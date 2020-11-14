#ifndef CTRE_ARRAY_H
#define CTRE_ARRAY_H

template <typename T, int N>
struct array {
    T _data[N] = {};

    constexpr int size() const {
        return N;
    }

    constexpr T operator[](int idx) const {
        return _data[idx];
    }

    constexpr T& operator[](int idx) {
        return _data[idx];
    }

    constexpr const T* begin() const {
        return _data;
    }

    constexpr const T* end() const {
        return &(_data[N]);
    }

    // in ascending order
    template <typename CMP>
    constexpr auto sorted(CMP cmp) const {
        array<T, N> res = *this;

        if constexpr (N < 2)
            return res;
        else {
            for (int i = 1; i < N; i++) {
                for (int j = i; j > 0; j--) {
                    if (!cmp(res._data[j - 1], res._data[j])) {
                        T tmp            = res._data[j - 1];
                        res._data[j - 1] = res._data[j];
                        res._data[j]     = tmp;
                    } else {
                        break;
                    }
                }
            }
            return res;
        }
    }
};

#endif