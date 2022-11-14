#include "stack_logs.h"

#include <stdio.h>
#include <math.h>
#include <stdarg.h>

#include "stack_verification.h"

FILE *Logstream = stdout;

void RealDumpLogs(Stack *stk, FILE *logfile, const char *file, const char *func, int line, int errors) {
    if (logfile == nullptr) {
        return;
    }

    if (stk == nullptr) {
        Print(logfile, "Can't print logfile: pointer to stack is crushed");
        return;
    }

    Print(logfile, "Logs called in %s at %s(%d):\n", func, file, line);

    if (ErrorIsThere(errors, LOGS_PTR_CRASHED) || stk->logs == nullptr) {
        Print(logfile, "LOGS_PTR_CRASHED: information about creation of stack was lost.\n");

    } else {
        if (ErrorIsThere(errors, FILE_INF_CRASHED)) {
            Print(logfile, "FILE_INFO_CRASHED: cannot find information about file of creation\n");

            if (ErrorIsThere(errors, FUNC_INF_CRASHED)) {
                Print(logfile, "FUNC_INFO_CRASHED:");
                Print(logfile, "cannot find information about function of creation\n");
            } else {
                Print(logfile, "Stack [%p] created in function %s line %d.\n", &stk, 
                        stk->logs->func_of_creation, stk->logs->line_of_creation);
            }

        } else if (ErrorIsThere(errors, FUNC_INF_CRASHED)) {
            Print(logfile, "Stack [%p] created in file %s (line: %d).\n", &stk, 
                    stk->logs->file_of_creation, stk->logs->line_of_creation);
            Print(logfile, "FUNC_INFO_CRASHED:");
            Print(logfile, "cannot find information about function of creation\n");
            
        } else {
            Print(logfile, "Stack [%p] created at %s(%d) in function %s.\n", &stk, 
                    stk->logs->file_of_creation, stk->logs->line_of_creation, 
                    stk->logs->func_of_creation);
        }
    }

    if (ErrorIsThere(errors, SIZE_EXCEED_CAP)) {
        Print(logfile, "Error: size exceed capacity\n");
    }

    #ifdef HASH_VERIFICATION
    if (ErrorIsThere(errors, HASH_DISMATCH)) {
        Print(logfile, "Error: Hash dismatch. Data may be lost.");
    }
    #endif

    Print(logfile, "Stack info:\n");

    Print(logfile, "{\n");
    Print(logfile, "\t capacity = %zd\n", stk->capacity);
    Print(logfile, "\t size     = %zd\n", stk->size);
    Print(logfile, "\t data [%p]\n",       stk->data);

    if (ErrorIsThere(errors, UNEXPECTED_PSN)) {
        Print(logfile, "\t Data error: unexpected poison in element's cell\n");
    }
    if (ErrorIsThere(errors, UNEXPECTED_ELM)) {
        Print(logfile, "\t Data error: element in poisoned cell\n");
    }

    if (ErrorIsThere(errors, DATA_PTR_CRASHED)) {
        Print(logfile, "DATA_PTR_CRASHED: cannot print data, information was lost\n");
    } else {
        Print(logfile, "\t {\n");

        #ifdef CANARY_VERIFICATION
        Canary_t *l_border_ptr = (Canary_t*) ((char*)stk->data - sizeof(Canary_t));
        Canary_t *r_border_ptr = (Canary_t*) ((char*)stk->data + sizeof(Elem_t) * stk->capacity);

        if (ErrorIsThere(errors, L_BORDER_CHANGED)) {
            Print(logfile, "\t \t Left  Border = %zd (expected %zd)\n", *l_border_ptr, Border);
            Print(logfile, "errors: %d\n", errors);
        } else {
            Print(logfile, "\t \t Left  Border = %zd (OK)\n", *l_border_ptr);
        }
        #endif

        for (size_t i = 0; i < stk->size && i < stk->capacity; ++i) {
            Print(logfile, "\t \t*[%zd] = " PFORMAT,   i, stk->data[i]);
            #ifdef DEEP_VERIFICATION
            if (IsPoisoned(stk->data[i])) {
                Print(logfile, " (poisoned)");
            } else {
                Print(logfile, " (busy)");
            }
            #endif
            Print(logfile, "\n");
        }


        for (size_t i = stk->size; i < stk->capacity; ++i) {
            Print(logfile, "\t \t [%zd] = " PFORMAT, i, stk->data[i]);
            #ifdef DEEP_VERIFICATION
            if (IsPoisoned(stk->data[i])) {
                Print(logfile, " (poisoned)");
            } else {
                Print(logfile, " (busy)");
            }
            #endif
            Print(logfile, "\n");
        }

        #ifdef CANARY_VERIFICATION
        if (ErrorIsThere(errors, R_BORDER_CHANGED)) {
            Print(logfile, "\t \t Right Border = %zd (expected %llu)\n", *r_border_ptr, Border);
        } else {
            Print(logfile, "\t \t Right Border = %zd (OK)\n", *r_border_ptr);
        }
        #endif

        Print(logfile, "\t }\n");
    }

    Print(logfile, "}\n\n");

    fflush(logfile);
    fflush(stdout);
}

void Print(FILE *output, const char *format, ...) {
    va_list ptr = {};
    va_start(ptr, format);

    if (output == nullptr) {
        return;
    }

    #ifdef LOGS_TO_FILE

    vfprintf(output, format, ptr);

    #endif

    #ifdef LOGS_TO_CONSOLE
    #ifdef LOGS_TO_FILE

    vfprintf(stdout, format, ptr);

    #else

    vfprintf(output, format, ptr);

    #endif
    #endif

    va_end(ptr);
}

FILE* GetLogStream() {
    return Logstream;
}

void SetLogStream(FILE *stream) {
    Logstream = stream;
}
