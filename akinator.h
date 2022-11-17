#ifndef AKINATOR_H
#define AKINATOR_H

#include <stdio.h>
#include <stdlib.h>

#include "Libs/Stack/stack.h"

struct Akinator {
    Tree  tree           = {};
    Stack dontknow_nodes = {};
    char* data_base      = nullptr;
};

enum Game_modes {
    Exit = 0,
    Guess,
    Graph_dump,
    Definition,
};

enum Answers {
    No       = -1,
    DontKnow =  0,
    Yes      =  1,
};

const char* get_input_name(int argc, const char **argv);

bool init_akinator(Akinator *akinator, const char *input_filename);

void run_akinator(Akinator *akinator);

void akinator_dtor(Akinator *akinator);

#endif
