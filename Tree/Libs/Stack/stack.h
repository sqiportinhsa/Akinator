#ifndef STACK
#define STACK

#include <stdio.h>
#include <math.h>

typedef int Elem_t;
const Elem_t Poisoned_cell = 57;

#define PFORMAT "%-10d"

typedef unsigned long long Canary_t;
const Canary_t Border = 0XBAAD7004;

const size_t Hash_base_const = 5381;
const size_t Hash_mult_const = 33;

typedef struct {
    Canary_t     left_border;
    int          line_of_creation;
    const char*  file_of_creation;
    const char*  func_of_creation;
    Canary_t     right_border;
} Logs;

typedef struct {
    Canary_t  left_border;
    Elem_t*   data;
    size_t    size;
    size_t    capacity;
    size_t    hash;
    Logs*     logs;
    Canary_t  right_border;
} Stack;

typedef enum {
    NO_ERROR         = 0,
    HASH_CALC_ERR    = 1,
    HASH_DISMATCH    = 1 << 1,
    EMPTY_STACK      = 1 << 2,
    MEMORY_EXCEED    = 1 << 3,
    UNEXPECTED_PSN   = 1 << 4,
    UNEXPECTED_ELM   = 1 << 5,
    SIZE_EXCEED_CAP  = 1 << 6,
    STCK_PTR_CRASHED = 1 << 7,
    DATA_PTR_CRASHED = 1 << 8,
    LOGS_PTR_CRASHED = 1 << 9,
    FILE_INF_CRASHED = 1 << 10,
    FUNC_INF_CRASHED = 1 << 11,
    R_BORDER_CHANGED = 1 << 12,
    L_BORDER_CHANGED = 1 << 13,
    OPEN_LOGFILE_ERR = 1 << 14,
    STK_BRDR_CHANGED = 1 << 15,
    LGS_BRDR_CHANGED = 1 << 16,
} Error;

#define StackCtr(stk, n_elem)                                                 \
        StackCtrWithLogs(stk, n_elem, __LINE__, __PRETTY_FUNCTION__, __FILE__)

int StackCtrWithLogs(Stack *stk, size_t n_elem, int line, const char* func, const char* file);
int StackDestr(Stack *stk);
int StackPush(Stack *stk, Elem_t value);
Elem_t StackPop(Stack *stk, int *err = nullptr);
Error ResizeStack(Stack *stk, size_t capacity);
Error PoisonCells(Stack *stk, size_t n_cells);
int IsPoisoned(Elem_t el1);

#endif
