#include "Tree/tree.h"

struct Akinator {
    Tree  tree      = {};
    char* data_base = nullptr;
}

int init_akinator(Tree *tree, char *input_filename);

const char* get_input_name(int argc, const char **argv);