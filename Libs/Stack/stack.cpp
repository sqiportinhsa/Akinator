#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"
#include "stack_verification.h"
#include "stack_logs.h"

int StackCtrWithLogs(Stack *stk, size_t n_elem, int line, const char* func, const char* file) {
    stk->logs = (Logs*) calloc(1, sizeof(Logs));

    if (stk->logs == nullptr) {
        return MEMORY_EXCEED;
    }

    stk->logs->file_of_creation = file;
    stk->logs->func_of_creation = func;
    stk->logs->line_of_creation = line;

    stk->logs->left_border  = Border;
    stk->logs->right_border = Border;

    stk->data = (Elem_t*) calloc(n_elem * sizeof(Elem_t) + 2 * sizeof(Canary_t), sizeof(char));

    if (stk->data == nullptr) {
        return MEMORY_EXCEED;
    }

    stk->data = (Elem_t*) ((char*)stk->data + sizeof(Canary_t));

    stk->capacity = n_elem;
    stk->size     = 0;
    stk->hash     = Hash_base_const;

    Canary_t *l_border_ptr = (Canary_t*) ((char*)stk->data - sizeof(Canary_t));
    Canary_t *r_border_ptr = (Canary_t*) ((char*)stk->data + sizeof(Elem_t) * stk->capacity);

    *l_border_ptr = Border;
    *r_border_ptr = Border;

    stk->left_border  = Border;
    stk->right_border = Border;

    PoisonCells(stk, n_elem);

    return SafeStackVerificator(stk);
}

int StackDestr(Stack *stk) {
    int errors = StackVerificator(stk);

    #ifdef SAFEMODE
    errors |= SafeStackVerificator(stk);
    #endif
    
    stk->size = 0;
    errors |= PoisonCells(stk, stk->capacity);
    errors |= ResizeStack(stk, 0);

    free((char*)stk->data - sizeof(Canary_t));
    stk->data = nullptr;

    free(stk->logs);
    stk->logs = nullptr;

    return errors;
}

Error PoisonCells(Stack *stk, size_t n_cells) {
    Error err = NO_ERROR;
    if (stk->size + n_cells > stk->capacity) {
        err = SIZE_EXCEED_CAP;
    }

    for (size_t i = 0; i < n_cells && stk->size + i < stk->capacity; ++i) {
        (stk->data)[stk->size + i] = Poisoned_cell;
    }

    return err;
}

int StackPush(Stack *stk, Elem_t value) {
    int errors = NO_ERROR;

    #ifdef SAFEMODE
    errors |= SafeStackVerificator(stk);
    #endif

    if (stk->size >= stk->capacity) {
        errors |= ResizeStack(stk, stk->capacity * 2 + 1);
    }

    if (IsPoisoned(stk->data[stk->size - 1]) || !IsPoisoned(stk->data[stk->size])) {
        errors |= UNEXPECTED_PSN;
    }
    stk->data[stk->size] = value;
    ++(stk->size);

    stk->hash = stk->hash*33 + (size_t) value;

    #ifdef SAFEMODE
    SafeStackVerificator(stk);
    #endif
    
    return errors;
}

Elem_t StackPop(Stack *stk, int *err) {
    if (stk == nullptr) {
        return Poisoned_cell;
    }

    int errors = NO_ERROR;

    errors |= SafeStackVerificator(stk);

    if (stk->size == 0) {
        errors |= EMPTY_STACK;

        if (err != nullptr) {
            *err = errors;
        }

        return Poisoned_cell;
    }

    --(stk->size);

    Elem_t popped_el = stk->data[stk->size];

    stk->hash = (stk->hash - (size_t) stk->data[stk->size]) / Hash_mult_const;
    
    errors |= PoisonCells(stk, 1);
    errors |= ResizeStack(stk, stk->capacity);

    if (err != nullptr) {
        *err = errors;
    }

    return popped_el;
}

Error ResizeStack(Stack *stk, size_t capacity) {
    if (stk == nullptr) {
        return STCK_PTR_CRASHED;
    }

    SafeStackVerificator(stk);

    Canary_t *l_border_ptr = (Canary_t*) ((char*)stk->data - sizeof(Canary_t));

    if (stk->capacity < capacity) {
        stk->data = (Elem_t*) realloc(l_border_ptr, capacity * sizeof(Elem_t) 
                                                         + 2 * sizeof(Canary_t));
        if (stk->data == nullptr) {
            return MEMORY_EXCEED;
        }

        size_t old_capacity = stk->capacity;

        stk->data = (Elem_t*) ((char*)stk->data + sizeof(Canary_t));
        stk->capacity = capacity;

        PoisonCells(stk, capacity - old_capacity);
    }

    if (stk->size < capacity / 2) {
        stk->data = (Elem_t*) realloc(l_border_ptr, capacity * sizeof(Elem_t) / 2 
                                                         + 2 * sizeof(Canary_t));
        if (stk->data == nullptr) {
            return MEMORY_EXCEED;
        }

        stk->data = (Elem_t*) ((char*)stk->data + sizeof(Canary_t));
        stk->capacity = capacity / 2;
    }
    
    Canary_t *r_border_ptr = (Canary_t*) ((char*)stk->data + sizeof(Elem_t) * stk->capacity);
    *r_border_ptr = Border;

    return NO_ERROR;
}

int IsPoisoned(Elem_t el1) {
    if (memcmp(&el1, &Poisoned_cell, sizeof(Elem_t)) == 0) {
        return 1;
    }
    return 0;
}
