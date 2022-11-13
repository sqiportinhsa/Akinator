#include "stack_verification.h"
#include "stack_logs.h"

int RealStackVerificator(Stack *stk, const char *file, const char *func, int line) {
    FILE *logfile = GetLogStream();

    int errors = NO_ERROR;

    if (stk == nullptr) {
        errors |= STCK_PTR_CRASHED;

        if (errors != 0) {
            RealDumpLogs(stk, logfile, file, func, line, errors);
        }

        return errors;
    }

    if (stk->capacity < stk->size) {
        errors |= SIZE_EXCEED_CAP;
    }

    if (stk->data == nullptr) {
        errors |= DATA_PTR_CRASHED;
    }

    if (stk->logs == nullptr) {
        errors |= LOGS_PTR_CRASHED;
    }

    #ifdef DEEP_VERIFICATION

    if (!ErrorIsThere(errors, DATA_PTR_CRASHED) && !ErrorIsThere(errors, SIZE_EXCEED_CAP)) {
        for (size_t i = 0; i < stk->size; ++i) {
            if (IsPoisoned(stk->data[i])) {
                errors |= UNEXPECTED_PSN;
                break;
            }
        }

        if (!ErrorIsThere(errors, UNEXPECTED_PSN)) {
            for (size_t i = stk->size; i < stk->capacity; ++i) {
                if (!IsPoisoned(stk->data[i])) {
                    errors |= UNEXPECTED_ELM;
                    break;
                }
            }
        }
    }

    #endif

    #ifdef CANARY_VERIFICATION
    
    if (!ErrorIsThere(errors, DATA_PTR_CRASHED)) {
        Canary_t *l_border_ptr = (Canary_t*) ((char*)stk->data - sizeof(Canary_t));
        Canary_t *r_border_ptr = (Canary_t*) ((char*)stk->data + sizeof(Elem_t) * stk->capacity);

        if (*l_border_ptr != Border) {
            errors |= L_BORDER_CHANGED;
        }

        if (*r_border_ptr != Border) {
            errors |= R_BORDER_CHANGED;
        }
    }

    if (!ErrorIsThere(errors, LOGS_PTR_CRASHED)) {
        if (stk->logs->left_border != Border || stk->logs->right_border != Border) {
            errors |= LGS_BRDR_CHANGED;
        }
    }

    if (stk->left_border != Border || stk->right_border != Border) {
        errors |= STK_BRDR_CHANGED;
    }
    
    #endif

    #ifdef HASH_VERIFICATION

    if (!ErrorIsThere(errors, DATA_PTR_CRASHED) && !ErrorIsThere(errors, SIZE_EXCEED_CAP)) {
        size_t hash = StackHash(stk);

        if (hash == HASH_CALC_ERR) {
            errors |= HASH_CALC_ERR;
        }

        if (hash != stk->hash) {
            errors |= HASH_DISMATCH;
        }
    }

    #endif

    if (!ErrorIsThere(errors, LOGS_PTR_CRASHED)) {
        if (stk->logs->file_of_creation == nullptr) {
            errors |= FILE_INF_CRASHED;
        }
        if (stk->logs->func_of_creation == nullptr) {
            errors |= FUNC_INF_CRASHED;
        }
    }

    return errors;
}

int RealSafeStackVerificator(Stack *stk, const char *file, const char *func, int line) {
    FILE *logfile = GetLogStream();
    int errors = RealStackVerificator(stk, file, func, line);
    if (errors != 0) {
        RealDumpLogs(stk, logfile, file, func, line, errors);
    }
    return errors;
}

int ErrorIsThere(int errors, Error error) {
    return (errors & error);
}

size_t StackHash(Stack *stk) {
    if (stk->data == nullptr || stk->size > stk->capacity) {
        return (size_t) HASH_CALC_ERR;
    }

    size_t hash = Hash_base_const;
    for (size_t i = 0; i < stk->size; ++i) {
        hash = hash*Hash_mult_const + (size_t) stk->data[i];
    }
    return hash;
}
