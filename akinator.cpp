#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#include "akinator.h"
#include "Libs/file_reading.hpp"

const int Max_input_len    = 50;
const int Picture_name_len = 50;

/*--------------------------- INTERNAL FUNCTIONS DECLARATION -------------------------------------*/

//------------ PARSING INPUT ----------------//

static bool get_tree(Akinator *akinator);

static bool get_head(Akinator *akinator);

static bool get_left (Akinator *akinator, Tree_node *parent, size_t *ip);
static bool get_right(Akinator *akinator, Tree_node *parent, size_t *ip);

static bool get_node(Akinator *akinator, Tree_node *parent, size_t *ip, bool is_left);

//--------------- MODES ---------------------//

static int get_mode();

static void save_new_tree(Tree *tree);

//------------- GUESS MODE ------------------//

static void run_quess_mode(Akinator *akinator);

static Answers    ask_questions(Akinator *akinator, Tree_node **node);

static Tree_node* ask_question (Akinator *akinator, Tree_node *node);

static Answers get_answer();

static void celebrate_win(Answers ans);

static void add_character(Akinator *akinator, Tree_node *node);

//------------- GRAPHIC DUMP ----------------//

static void run_graph_dump(Tree *tree);

//------------- OTHER STATICS ---------------//

static bool get_data_base(Akinator *akinator, const char *input);

static void get_user_input(char *input);




/*-------------------------------- EXTERNAL FUNCTIONS --------------------------------------------*/

const char* get_input_name(int argc, const char **argv) {
    CLArgs args = parse_cmd_line(argc, argv);

    if (args.output != nullptr) {
        printf("Warning: unexpected flag -o given\n");
    }

    return args.input;
}

bool init_akinator(Akinator *akinator, const char *input_filename) {
    assert(akinator != nullptr);

    init_tree(&akinator->tree);

    StackCtr(&akinator->dontknow_nodes, 0);

    if (input_filename != nullptr) {

        if (!get_data_base(akinator, input_filename)) {

            return false;
        }

    } else {

        akinator->data_base = nullptr;
    }

    if (!get_tree(akinator)) {

        return false;
    }

    return true;
}

void run_akinator(Akinator *akinator) {
    int mode = 1;

    while (mode != 0) {
        mode = get_mode();

        switch (mode) {
        case Exit:
            save_new_tree(&akinator->tree);
            return;

        case Guess:
            run_quess_mode(akinator);
            break;

        case Graph_dump:
            run_graph_dump(&akinator->tree);
            break;
            
        default:
            printf("You entered non-existing mode number. Please, try again\n");
            continue;
        }
    }
}

void akinator_dtor(Akinator *akinator) {
    tree_dtor(&akinator->tree);
    StackDestr(&akinator->dontknow_nodes);
    free(akinator->data_base);

    akinator->data_base      = nullptr;
}


/*---------------------------------- PARSING INPUT FILE ------------------------------------------*/

static bool get_tree(Akinator *akinator) {
    assert(akinator != nullptr);

    if (akinator->data_base == nullptr) {

        init_head_node(&akinator->tree, "Someone");

        akinator->tree.head->is_saved = false;

        return true;
    }

    return get_head(akinator);
}


#define SKIP_SPACES(ip)                                 \
        for (; isspace(akinator->data_base[ip]); ++(ip));

#define SKIP_STRING(ip)                                 \
        for (; akinator->data_base[ip] != '"'; ++ip);   \
        ++ip;

#define SET_STRING_ENDING(ip)           \
        akinator->data_base[ip - 1] = '\0';\
        ++ip;

#define CHECK_SYM(sym, ip)                                                              \
        if (akinator->data_base[ip] != sym) {                                           \
            printf("Error: incorrect input file format.\nExpected: <%c>, got: <%c>\n",  \
                                                      sym, akinator->data_base[ip]);    \
            return false;                                                               \
        }                                                                               \
        ++ip;

#define CHECK_FOR_ENDING(ip)                    \
        if (akinator->data_base[ip] == '}') {   \
            (ip)++;                             \
            return true;                        \
        }


static bool get_head(Akinator *akinator) {
    assert(akinator != nullptr);
    
    size_t ip = 0;

    CHECK_SYM('{', ip);

    SKIP_SPACES(ip);

    CHECK_SYM('"', ip);

    init_head_node(&akinator->tree, &(akinator->data_base[ip]));

    akinator->tree.head->is_saved = true;

    SKIP_STRING(ip);

    SET_STRING_ENDING(ip);

    SKIP_SPACES(ip);

    CHECK_FOR_ENDING(ip);

    if (!get_left (akinator, akinator->tree.head, &ip)) {
        return false;
    }

    if (!get_right(akinator, akinator->tree.head, &ip)) {
        return false;
    }

    SKIP_SPACES(ip);

    CHECK_FOR_ENDING(ip);

    return false;
}

static bool get_left (Akinator *akinator, Tree_node *parent, size_t *ip) {
    return get_node(akinator, parent, ip, true);
}

static bool get_right(Akinator *akinator, Tree_node *parent, size_t *ip) {
    return get_node(akinator, parent, ip, false);
}

static bool get_node(Akinator *akinator, Tree_node *parent, size_t *ip, bool is_left) {

    assert(akinator != nullptr);
    assert(ip       != nullptr);
    assert(parent   != nullptr);

    SKIP_SPACES(*ip);

    CHECK_SYM('{', *ip);

    SKIP_SPACES(*ip);

    CHECK_SYM('"', *ip);

    Tree_node *node = nullptr;

    if (is_left) {
        node = init_left_node (&akinator->tree, parent, &(akinator->data_base[*ip]));
    } else {
        node = init_right_node(&akinator->tree, parent, &(akinator->data_base[*ip]));
    }

    node->is_saved = true;

    SKIP_STRING(*ip);

    SET_STRING_ENDING(*ip);

    SKIP_SPACES(*ip);

    CHECK_FOR_ENDING(*ip);

    if (!get_left (akinator, node, ip)) {
        return false;
    }
    
    if (!get_right(akinator, node, ip)) {
        return false;
    }

    SKIP_SPACES(*ip);

    CHECK_FOR_ENDING(*ip);

    return false;
}

/*------------------------------------ AKINATOR MODES --------------------------------------------*/

static int get_mode() {
    printf("To continue choose game mode:\n");
    printf("\t%d - Exit the game\n", Exit);
    printf("\t%d - Answer Akinator's questions and it will guess you character\n", Guess);
    printf("\t%d - Graph dump of questions tree\n", Graph_dump);

    int mode = 0;

    if (scanf("%d%*c", &mode) != 1) {
        printf("You failed mode choosing. Please try again.\n");
        return get_mode();
    }

    return mode;
}

//----------------- EXIT ------------------//

static void save_new_tree(Tree *tree) {
    printf("Do you wanna save tree before exit? [yes/no]\n");

    Answers ans = get_answer();

    if (ans != Yes) {
        return;
    }

    printf("Please, enter name of file for saving:\n");

    char answer[Max_input_len] = {};

    get_user_input(answer);

    FILE *output = fopen(answer, "w");

    if (output != nullptr) {
        text_database_dump(tree, output);
    }

    fclose(output);
}

//-------------- GUESS MODE ---------------//

static void run_quess_mode(Akinator *akinator) {
    printf("Quess a character and I will try to guess it.\n"
                 "Answer some questions about it, please.\n");

    Tree_node *node = akinator->tree.head;

    Answers ans = No;

    while (ans == No) {
        ans = ask_questions(akinator, &node);

        if (ans != No) {
            celebrate_win(ans);

            break;
        }

        if (akinator->dontknow_nodes.size != 0) {

            node = StackPop(&akinator->dontknow_nodes);

            node = node->right;

            continue;
        }

        add_character(akinator, node);

        break;
    }

}

static Answers ask_questions(Akinator *akinator, Tree_node **node) {

    assert(akinator != nullptr);
    assert( node    != nullptr);
    assert(*node    != nullptr);

    while ((*node)->left != nullptr && (*node)->right != nullptr) {
        *node = ask_question(akinator, *node);

        assert(*node != nullptr);
    }

    printf("Your character is %s? [yes/no/dn] (dn = don't know)\n", (*node)->data);

    Answers ans = get_answer();

    return ans;
}

static Tree_node* ask_question(Akinator *akinator, Tree_node *node) {
    assert(node != nullptr);

    printf("Your character %s? [yes/no/dn] (dn = don't know)\n", node->data);

    Answers ans = get_answer();

    switch (ans) {
        case Yes:

            return node->left;

        case No:

            return node->right;

        case DontKnow:

            StackPush(&akinator->dontknow_nodes, node);

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
        printf("Thank you for the game! As you can see, I'm really clever programm\n" 
               "(But not as clever as my creator). Can you give her a good mark please?^^\n");
    }

    if (ans == DontKnow) {
        printf("Don't you really know who you character is?\n" 
               "Maybe you wanna restart game with new character that you actually know?\n" 
               "Anyway thank you for the game! Hope you liked it :3\n");
    }
}

#define memory_allocate(ptr)                                                                  \
        char *ptr = (char*) calloc(Max_input_len, sizeof(char));                              \
        if (ptr == nullptr) {                                                                 \
            printf("Sorry, I can't add your character: there is no enougth memory");          \
            return;                                                                           \
        }


static void add_character(Akinator *akinator, Tree_node *node) {
    printf("I'm sorry but i don't know who was guessed. Stupid programm!\n"
           "Can you help me become better by telling who was you character? [yes/no]\n");

    Answers ans = get_answer();

    if (ans != Yes) {
        printf("What a pity! Anyway thank you for the game. Let's return to mode choosing.\n");
        return;
    }

    printf("Thank you! Enter your character's name please\n");

    memory_allocate(new_character_name);

    get_user_input(new_character_name);

    printf("Please, give the difference between %s and %s. ", node->data, new_character_name);
    printf("Unlike %s %s...\n", node->data, new_character_name);

    memory_allocate(difference);

    get_user_input(difference);

    char *old_character = node->data;

    init_left_node (&akinator->tree, node, new_character_name);
    init_right_node(&akinator->tree, node, old_character);

    node->data = difference;

    node->is_saved        = false;
    node->left->is_saved  = false;
    node->right->is_saved = true;

    printf("Thank you for help! Do you want to see new questions tree? [yes/no]\n");

    ans = get_answer();

    if (ans != Yes) {
        printf("Okay, let's return to mode choosing and have more fun!\n");
        return;
    }

    run_graph_dump(&akinator->tree);
}

#undef memory_allocate

//--------------- GRAPHIC DUMP ------------//

static void run_graph_dump(Tree *tree) {
    char picture_name[Picture_name_len] = {};

    generate_file_name(picture_name, "png");

    generate_graph_picture(tree, picture_name);

    printf("Picture is generated, you can get it by name %s\n", picture_name);
}


/*-------------------------------- OTHER STATIC FUNCTIONS ----------------------------------------*/

#define memory_allocate(ptr, size, type)                                                      \
        ptr = (type*) calloc(size, sizeof(type));                                             \
        if (ptr == nullptr) {                                                                 \
            printf("Error: can't run akinator - not enought memory\n");                       \
            return false;                                                                     \
        }

static bool get_data_base(Akinator *akinator, const char *input) {
    assert(akinator != nullptr);
    assert(input    != nullptr);

    size_t amount_of_symbols = count_elements_in_file(input);

    memory_allocate(akinator->data_base, amount_of_symbols, char);

    amount_of_symbols = read_file(akinator->data_base, amount_of_symbols, input);

    return true;
}

#undef memory_allocate

static void get_user_input(char *input) {
    assert(input != nullptr);

    fgets(input, Max_input_len, stdin);

    *(strchr (input, '\n')) = '\0';
}
