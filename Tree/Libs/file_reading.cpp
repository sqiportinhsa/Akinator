#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>

#include "file_reading.hpp"

CLArgs parse_cmd_line(int argc, const char **argv) {
    assert(argc >= 1);
    assert(argv != nullptr);

    // Ignore first arg
    argc--;
    argv++;

    CLArgs args = {};

    args.input  = nullptr;
    args.output = nullptr;

    for (int i = 0; i < argc; ++i) {
        // -o: output filename
        if (strcmp(argv[i], "-o") == 0) {
            ++i;

            if (i >= argc) {
                fprintf(stderr, "Warning: -o flag requires output file name\n");
                break;
            }

            args.output = argv[i];
        }

        // -i: input filename
        if (strcmp(argv[i], "-i") == 0) {
            ++i;

            if (i >= argc) {
                fprintf(stderr, "Warning: -i flag requires input file name\n");
                break;
            }

            args.input = argv[i];
        }
    }

    return args;
}

size_t count_elements_in_file(const char file_name[]) {
    assert(file_name != nullptr && "nullptr to file_name");
    struct stat a = {};
    stat(file_name, &a);
    return a.st_size + 1;
}

size_t read_file(char text[], size_t amount_of_symbols, const char* file_name) {
    assert(text  != nullptr && "text  is nullptr");
    
    FILE *input = fopen(file_name, "rb");

    if (input == nullptr) {
        return 0;
    }

    size_t nread = fread(text, sizeof(char), amount_of_symbols, input);

    fclose(input);

    size_t real_amount_of_symbols = nread + 1;

    text[real_amount_of_symbols - 1] = '\0';

    return real_amount_of_symbols;
}

int count_strings(char text[], size_t amount_of_symbols) {
    assert(text != nullptr && "text is nullptr");

    int amount_of_strings = 0;
    for (size_t i = 0; i < amount_of_symbols; ++i) {
        if (text[i] == '\n' || text[i] == '\0') {
            ++amount_of_strings;
        }
    }
    return amount_of_strings;
}

int get_val(char *ptr_to_first_elem, int *ptr_to_val) {
    assert(ptr_to_first_elem != nullptr && "ptr_to_first_elem is nullptr");
    assert(ptr_to_val        != nullptr && "ptr_to_val is nullptr");

    int val_len = 0;

    sscanf(ptr_to_first_elem, "%d%n", ptr_to_val, &val_len);

    return val_len;
}

void skip_spaces(char **pointer) {
    assert(pointer != nullptr && "pointer is nullptr");

    for (; **pointer == ' '; ++(*pointer));
}

size_t skip_spaces(char *pointer) {
    assert(pointer != nullptr && "pointer in nullptr");

    size_t i = 0;
    for (; *(pointer + i) == ' '; ++i);

    return i;
}

void skip_to_newline(char **pointer) {
    assert(pointer != nullptr && "pointer is nullptr");

    for (; **pointer != '\n' && **pointer != '\0'; ++(*pointer));
}

size_t skip_to_newline(char *pointer) {
    assert(pointer != nullptr && "pointer is nullptr");

    size_t i = 0;

    for (; *(pointer + i) != '\n' && *(pointer + i) != '\0'; ++i);

    return i;
}

size_t skip_spaces_and_void_lines(char *pointer) {
    assert (pointer != nullptr && "pointer is nullptr");

    size_t i = 0;

    for (; *(pointer + i) == '\n' || *(pointer + i) == ' '; ++i);

    return i;
}
