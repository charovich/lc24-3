// #define pow
U32 pow(U32 base, U32 len) {
    U32 res = 1;
    while (len--) {
        res *= base;
    }
    return res;
}
#define true 1
#define false 0
#define bool int

int _fltused = 0;
