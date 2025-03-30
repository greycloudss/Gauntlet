#include <iostream>

template <typename A, typename B, typename C> struct triple {
    A aVal;
    B bVal;
    C cVal;
    triple(A aVal, B bVal, C cVal) : aVal(aVal), bVal(bVal), cVal(cVal) { }

    A getA() {
        return aVal;
    }

    B getB() {
        return bVal;
    }

    C getC() {
        return cVal;
    }

    template <typename valType> void change(char flag, valType val) {
        switch (flag) {
            case 0: aVal = (A)val; return;
            case 1: bVal = (B)val; return;
            case 3: cVal = (C)val; return;

            default: throw std::cout << "\n\n[ERROR] bad argument in the triple \nflag: " << (int) flag << "\nval: " << val;
        }
    }
};