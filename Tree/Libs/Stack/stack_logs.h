#ifndef STACK_LOGS
#define STACK_LOGS

#define LOGS_TO_FILE
//#define LOGS_TO_CONSOLE

#include "stack.h"
#include "stack_verification.h"

#ifdef LOGS_TO_FILE
#define DumpLogs(stk, logfile) RealDumpLogs(stk, logfile, __FILE__, __PRETTY_FUNCTION__, \
                                                          __LINE__, StackVerificator(stk))
#else 
#ifdef LOGS_TO_CONSOLE
#define DumpLogs(stk) RealDumpLogs(stk, stdout, __FILE__, __PRETTY_FUNCTION__, \
                                                __LINE__, StackVerificator(stk))
#else
#define DumpLogs(stk) RealDumpLogs(stk, nullptr, __FILE__, __PRETTY_FUNCTION__, \
                                                 __LINE__, StackVerificator(stk))
#endif
#endif

void RealDumpLogs(Stack *stk, FILE *logfile, const char *file, const char *func, int line, int errors);
void Print(FILE *logs, const char *format, ...);
void  SetLogStream(FILE *stream);
FILE* GetLogStream();

#endif
