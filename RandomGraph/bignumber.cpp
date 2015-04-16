#include <cmath>

#include "bignumber.h"

BigInteger::BigInteger(unsigned long val) {
    while (val > 0) {
        internal.push_back(val % BIGNUMBER_BASE);
        val /= BIGNUMBER_BASE;
    }
}

BigInteger& BigInteger::operator+=(const BigInteger& x) {
    internal.resize(std::max(internal.size(),
                             x.internal.size()));
    for (size_t i = 0; i < x.internal.size(); ++i)
        internal[i] += x.internal[i];
    for (size_t i = 0; i < internal.size(); ++i) {
        unsigned long to_next = internal[i] / BIGNUMBER_BASE;
        if (to_next != 0) {
            if (i == internal.size() - 1)
                internal.push_back(to_next);
            else
                internal[i + 1] += to_next;
        }
        internal[i] %= BIGNUMBER_BASE;
    }
    return *this;
}

long double BigInteger::toDouble() const {
    long double res = 0.0;
    for (size_t i = internal.size() - 1; i >= 0 && i != -1; --i) {
        res *= BIGNUMBER_BASE;
        res += internal[i];
    }
    return res;
}
