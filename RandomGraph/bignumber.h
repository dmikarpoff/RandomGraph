#ifndef BIGNUMBER_H
#define BIGNUMBER_H

#define BIGNUMBER_BASE 10000

#include <vector>

class BigInteger {
 public:
    BigInteger(unsigned long val = 0);
    BigInteger& operator+=(const BigInteger& x);
    long double toDouble() const;
 private:
    std::vector<unsigned long> internal;
};

#endif // BIGNUMBER_H
