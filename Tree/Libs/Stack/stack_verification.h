#ifndef VERIFICATION
#define VERIFICATION

#include "stack.h"

// #define DEEP_VERIFICATION
// #define CANARY_VERIFICATION
// #define HASH_VERIFICATION
// #define SAFEMODE

#define StackVerificator(stk) \
    RealStackVerificator(stk, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define SafeStackVerificator(stk) \
    RealSafeStackVerificator(stk, __FILE__, __PRETTY_FUNCTION__, __LINE__)

int RealStackVerificator(Stack *stk, const char *file, const char *func, int line);
int RealSafeStackVerificator(Stack *stk, const char *file, const char *func, int line);
int ErrorIsThere(int errors, Error error);
size_t StackHash(Stack *stk);

#endif