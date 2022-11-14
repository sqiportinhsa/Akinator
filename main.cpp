#include "akinator.h"
#include "Tree/tree.h"

int main(int argc, const char **argv) {
    const char *input_filename = get_input_name(argc, argv);

    Akinator akinator = {};

    if (!init_akinator(&akinator, input_filename)) {
        return -1;
    }

    run_akinator(&akinator);

    akinator_dtor(&akinator);

    return 0;
}