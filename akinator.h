#ifndef AKINATOR_H
#define AKINATOR_H

#include "Tree/tree.h"

struct Akinator {
    Tree  tree           = {};
    Stack dontknow_nodes = {};
    char* data_base      = nullptr;
};

enum Game_modes {
    Exit = 0,
    Guess,

};

enum Answers {
    No       = -1,
    DontKnow =  0,
    Yes      =  1,
};

const char* get_input_name(int argc, const char **argv);

int init_akinator(Tree *tree, char *input_filename);

#endif
