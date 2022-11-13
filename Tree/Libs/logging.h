#ifndef LOGGING
#define LOGGING

#include <stdio.h>

typedef struct {
    int          line_of_creation;
    const char*  file_of_creation;
    const char*  func_of_creation;
} Creation_logs;

#define PrintToLogs(format, ...) RealPrintToLogs(GetLogStream(), __PRETTY_FUNCTION__,    \
                                                __FILE__, __LINE__, format, ##__VA_ARGS__)

void  SetLogStream(FILE *stream);
FILE* GetLogStream();
void  RealPrintToLogs(FILE *stream, const char *func, const char *file, 
                                    int line, const char *format, ...);
FILE *CreateLogFile(const char *name);

void init_cr_logs(Creation_logs *logs, const char *file, const char *func, int line);

#endif