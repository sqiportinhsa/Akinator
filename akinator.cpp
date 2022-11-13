#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "akinator.h"

/*------------------------------ EXTERNAL FUNCTIONS ----------------------------------------------*/

const char* get_input_name(int argc, const char **argv) {
    CLArgs args = parse_cmd_line(argc, argv);

    if (args.output != nullptr) {
        printf("Warning: unexpected flag -o given\n");
    }

    return args.input;
}

int init_akinator(Akinator *akinator, char *input_filename) {
    assert(akinator != nullptr);

    init_tree(akinator->tree);

    if (input_filename != nullptr) {

        get_data_base(akinator->data_base, input_filename);

    } else {

        akinator->data_base = nullptr;
    }

    get_tree(akinator);
}

int run_akinator(Akinator *akinator)


/*---------------------------- PARSING INPUT FILE ------------------------------------------------*/

static bool get_tree(Akinator *akinator) {
    assert(akinator != nullptr);

    if (akinator->data_base == nullptr) {

        init_head_node(akinator->tree, "Someone");

        return true;
    }
    
    size_t ip = 0;

    return get_head(akinator, &ip);
}


#define SKIP_SPACES(ip)                               \
        for (; isspace(akinator->data_base[ip]); ++ip);

#define SKIP_STRING(ip)                                                                   \
        for (; isalnum(akinator->data_base[ip]) || ispunct(akinator->data_base[ip]); ++ip);

#define SET_STRING_ENDING(ip)           \
        akinator->data_base[ip++] = '\0';

#define CHECK_SYM(sym)                                          \
        if (akinator->data_base[ip++] != sym) {                 \
            printf("Error: incorrect input file format\n");     \
            return false;                                       \
        }

#define CHECK_FOR_ENDING(ip)                    \
        if (akinator->data_base[ip] == '}') {   \
            return true;                        \
        }


static bool get_head(Akinator *akinator, size_t *ip) {
    assert(akinator != nullptr);
    assert(ip       != nullptr);

    CHECK_SYM('{');

    SKIP_SPACES(*ip);

    CHECK_SYM('"');

    init_head_node(akinator->tree, &(akinator->data_base[ip]));

    SKIP_STRING(*ip);

    SET_STRING_ENDING(*ip);

    SKIP_SPACES(*ip);

    CHECK_FOR_ENDING(*ip);

    if (!get_left (akinator->tree.head, ip)) {
        return false;
    }

    if (!get_right(akinator->tree.head, ip)) {
        return false;
    }

    SKIP_SPACES(*ip);

    CHECK_FOR_ENDING(*ip);

    return false;
}

static bool get_left (Akinator *akinator, Tree_node *parent, size_t *ip) {
    return get_node(akinator, parent, ip, true);
}

static bool get_right(Akinator *akinator, Tree_node *parent, size_t ip) {
    return get_node(akinator, parent, ip, false);
}

static bool get_node(Akinator *akinator, Tree_node *parent, size_t *ip, bool is_left) {

    assert(akinator != nullptr);
    assert(ip       != nullptr);
    assert(parent   != nullptr);

    SKIP_SPACES(*ip);

    CHECK_SYM('{');

    SKIP_SPACES(*ip);

    CHECK_SYM('"');

    Tree_node *node = nullptr;

    if (is_left) {
        node = init_left_node (parent, &(akinator->data_base[ip]));
    } else {
        node = init_right_node(parent, &(akinator->data_base[ip]));
    }

    SKIP_STRING(*ip);

    SET_STRING_ENDING(*ip);

    SKIP_SPACES(*ip);

    CHECK_FOR_ENDING(*ip);

    if (!get_left (node, ip)) {
        return false;
    }
    
    if (!get_right(node, ip)) {
        return false;
    }

    CHECK_FOR_ENDING(*ip);

    return false;
}

/*-------------------------- OTHER STATIC FUNCTIONS ----------------------------------------------*/

static void get_data_base(Akinator *akinator, const char *input) {
    assert(akinator != nullptr);
    assert(input    != nullptr);

    size_t amount_of_symbols = count_elements_in_file(input);

    memory_allocate(akinator->data_base, amount_of_symbols, char);

    amount_of_symbols = read_file(akinator->data_base, amount_of_symbols, input);
}