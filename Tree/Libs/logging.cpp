#include <stdarg.h>
#include <assert.h>

#include "logging.h"

FILE *Logstream = stdout;

FILE* GetLogStream() {
    return Logstream;
}

void SetLogStream(FILE *stream) {
    Logstream = stream;
}

void RealPrintToLogs(FILE *stream, const char *func, const char *file, int line, const char *format, ...) {
    assert(stream != nullptr);
    assert(format != nullptr);
    assert(func   != nullptr);
    assert(file   != nullptr);
    
    va_list ptr = {};
    va_start(ptr, format);
    fprintf(stream, "Message called at %s(%d) in file %s:", func, line, file);
    vfprintf(stream, format, ptr);
    va_end(ptr);
}

FILE* CreateLogFile(const char *name) {
    assert(name != nullptr);

    FILE *logfile = fopen(name, "w");

    if (logfile == nullptr) {
        return nullptr;
    }

    SetLogStream(logfile);

    fprintf(logfile, "<pre>");

    return logfile;
}

void init_cr_logs(Creation_logs *logs, const char *file, const char *func, int line) {
    assert(logs != nullptr);

    logs->file_of_creation = file;
    logs->func_of_creation = func;
    logs->line_of_creation = line;
}