#include "akinator.h"
#include "tree.h"

int main(int argc, const char **argv) {
    const char input_filename = get_input_name(argc, argv);

    Akinator akinator = {};

    int errors = init_akinator(akinator, input_filename);

    if (errors) {
        return -1;
    }


}