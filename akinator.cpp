#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#include "akinator.h"

const int Max_input_len = 50;

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

void run_akinator(Akinator *akinator) {
    int mode = 1;

    while (mode != 0) {
        mode = get_mode();

        switch (mode) {
        case Exit:
            return;

        case Guess:
            run_quess_mode(akinator);
            break;

        default:
            printf("You entered non-existing mode number. Please, try again\n");
            continue;
        }
    }
}


/*---------------------------------- PARSING INPUT FILE ------------------------------------------*/

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

/*------------------------------------ AKINATOR MODES --------------------------------------------*/

static int get_mode() {
    printf("To continue choose game mode:\n");
    printf("\t%d - Exit the game\n", Exit);
    printf("\t%d - Answer Akinator's questions and it will guess you character\n", Guess);

    int mode = 0;

    if (scanf("%d", &mode) != 1) {
        printf("You failed mode choosing. Please try again.\n");
        return get_mode();
    }

    return mode;
}

//-------------- GUESS MODE ---------------//

static void run_quess_mode(Akinator *akinator) {
    printf("Quess a character and I will try to guess it.\n"
                 "Answer some questions about it, please.\n");

    Tree_node *node = akinator->tree.head;

    Answers ans = No;

    while (ans == No) {
        ans = ask_questions(akinator, node);

        if (ans != No) {
            celebrate_win(ans);

            break;
        }

        if (akinator->dontknow_nodes.size != 0) {

            node = StackPop(akinator->dontknow_nodes);

            continue;
        }

        add_new_node(akinator);
    }

}

static Tree_node ask_questions(const Akinator *akinator, Tree_node *node) {

    assert(akinator != nullptr);
    assert(node     != nullptr);

    while (node.left != nullptr && node.right != nullptr) {
        node = ask_question(node);

        assert(node != nullptr);
    }

    printf("Your character is %s? [yes/no/dn] (dn = don't know)\n", node->data);

    Answers ans = get_answer();

    return ans;
}

static Tree_node ask_question(const Akinator *akinator, Tree_node *node) {
    assert(node != nullptr);

    printf("Your character %s?", node->data);

    Answers ans = get_answer();

    switch (ans) {
        case Yes:

            return node->right;

        case No:

            return node->left;

        case DontKnow:

            StackPush(akinator->dontknow_nodes, node);

            return node->left;

        default:
            break;
    }

    return nullptr;
}

static Answers get_answer() {
    char answer[Max_input_len] = {};

    get_user_input(answer);

    if (strcasecmp(answer, "yes") == 0) {
        return Yes;

    } else if (strcasecmp(answer, "no") == 0) {
        return No;

    } else if (strcasecmp(answer, "dn") == 0) {
        return DontKnow;

    }

    printf("Sorry, I can't understand your answer. It would be \"Don't know\"\n");

    return DontKnow;
}

static void celebrate_win(Answers ans) {
    if (ans == Yes) {
        printf("Thank you for the game! As you can see, I'm really clever programm " 
               "(but not as clever as my creator).\n Can you give her a good mark please?^^\n");
    }

    if (ans == DontKnow) {
        printf("Don't you really know who you character is?\n" 
               "Maybe you wanna restart game with new character that you actually know?\n" 
               "Anyway thank you for the game! Hope you liked it :3");
    }
}


/*-------------------------------- OTHER STATIC FUNCTIONS ----------------------------------------*/

static void get_data_base(Akinator *akinator, const char *input) {
    assert(akinator != nullptr);
    assert(input    != nullptr);

    size_t amount_of_symbols = count_elements_in_file(input);

    memory_allocate(akinator->data_base, amount_of_symbols, char);

    amount_of_symbols = read_file(akinator->data_base, amount_of_symbols, input);
}

static void get_user_input(char *input) {
    assert(input != nullptr);

    fgets(input, Max_input_len, stdin);
}