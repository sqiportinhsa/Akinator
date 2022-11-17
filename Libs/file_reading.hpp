#ifndef FILE_READING
#define FILE_READING

#include <stdio.h>

struct CLArgs {
    const char *input;
    const char *output;
};

CLArgs parse_cmd_line(int argc, const char **argv);

size_t count_elements_in_file(const char file_name[]);

size_t read_file(char text[], size_t amount_of_symbols, const char* file_name);

int count_strings(char text[], size_t amount_of_symbols);

int get_val(char *ptr_to_first_elem, int *ptr_to_val);

void   skip_spaces(char **pointer);
size_t skip_spaces(char  *pointer);

void   skip_to_newline(char **pointer);
size_t skip_to_newline(char *pointer);

size_t skip_spaces_and_void_lines(char *pointer);

void debug_text_dump(char *text, size_t ip);

#endif
