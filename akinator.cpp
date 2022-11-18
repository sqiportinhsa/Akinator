#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

#include "akinator.h"
#include "Libs/file_reading.hpp"

const int Max_input_len    = 50;
const int Picture_name_len = 30;
const int Message_len      = 150;

/*--------------------------- INTERNAL FUNCTIONS DECLARATION -------------------------------------*/

//------------ PARSING INPUT ----------------//

static bool get_tree(Akinator *akinator);

static bool get_head(Akinator *akinator);

static bool get_left(Akinator *akinator, Tree_node *parent, size_t *ip);
static bool get_right(Akinator *akinator, Tree_node *parent, size_t *ip);

static bool get_node(Akinator *akinator, Tree_node *parent, size_t *ip, bool is_left);

//--------------- MODES ---------------------//

static int get_mode();

static void get_user_input(char *input);

static Answers get_answer();

static Tree_node* find_node(Tree_node *node, char *data);

static void print_and_read(const char *message, ...);

//---------------- EXIT ---------------------//

static void save_new_tree(Tree *tree);

//------------- GUESS MODE ------------------//

static void run_quess_mode(Akinator *akinator);

static Answers    ask_questions(Akinator *akinator, Tree_node **node);

static Tree_node* ask_question (Akinator *akinator, Tree_node *node);

static void celebrate_win(Answers ans);

static void add_character(Akinator *akinator, Tree_node *node);

//------------- GRAPHIC DUMP ----------------//

static void run_graph_dump(Tree *tree);

//------------ DEFINITION MODE --------------//

static void run_definition_mode(Tree *tree);

//------------ DIFFERENCE MODE --------------//

static void run_diff_mode(Tree *tree);

static void get_path(Tree_node *node, Stack *stk);

static bool print_commons    (const char *name1, const char *name2, Stack *stk1, Stack *stk2);

static void print_difference (const char *name1, const char *name2, Stack *stk1, Stack *stk2);

static bool print_common_prop (Stack *stk1, Stack *stk2, Tree_node *node1, Tree_node *node2);

static void print_properties  (Stack *stk, const char *name, const Tree_node *node);

//------------- OTHER STATICS ---------------//

static bool get_data_base(Akinator *akinator, const char *input);



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

            case Definition:
                run_definition_mode(&akinator->tree);
                break;

            case Difference:
                run_diff_mode(&akinator->tree);
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

    akinator->data_base = nullptr;
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

#define SKIP_STRING(ip)                                \
        for (; akinator->data_base[ip] != '"'; ++ip);  \
        ++ip;

#define SET_STRING_ENDING(ip)                 \
        akinator->data_base[ip - 1] = '\0';   \
        ++ip;

#define CHECK_SYM(sym, ip)                                                          \
    if (akinator->data_base[ip] != sym) {                                           \
        printf("Error: incorrect input file format.\nExpected: <%c>, got: <%c>\n",  \
               sym, akinator->data_base[ip]);                                       \
        return false;                                                               \
    }                                                                               \
    ++ip;

#define CHECK_FOR_ENDING(ip)               \
    if (akinator->data_base[ip] == '}') {  \
        (ip)++;                            \
        return true;                       \
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

    if (!get_left(akinator,  akinator->tree.head, &ip)) {
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

        node = init_left_node(&akinator->tree, parent, &(akinator->data_base[*ip]));

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

    print_and_read("To continue choose game mode:\n");

    printf("\t%d - Exit the game\n", Exit);
    printf("\t%d - Answer Akinator's questions and it will guess you character\n", Guess);
    printf("\t%d - Graph dump of questions tree\n", Graph_dump);
    printf("\t%d - Get character's definition\n", Definition);
    printf("\t%d - Get difference in characters definitions\n", Difference);

    int mode = 0;

    if (scanf("%d%*c", &mode) != 1) {

        printf("You failed mode choosing. Please try again.\n");

        while (getchar() != '\n');

        return get_mode();
    }

    return mode;
}

//---------------- Common -----------------//

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

static void get_user_input(char *input) {
    assert(input != nullptr);

    fgets(input, Max_input_len, stdin);

    *(strchr(input, '\n')) = '\0';
}

static Tree_node* find_node(Tree_node *node, char *data) {

    assert(node != nullptr);
    assert(data != nullptr);

    if (strcasecmp(data, node->data) == 0) {
        return node;
    }

    if (node->left == nullptr || node->right == nullptr) {
        return nullptr;
    }

    Tree_node *ans = nullptr;

    ans = find_node(node->left, data);

    if (ans != nullptr) {
        return ans;
    }

    ans = find_node(node->right, data);

    if (ans != nullptr) {

        return ans;
    }

    return nullptr;
}

static void print_and_read(const char *message, ...) {
    va_list ptr = {};
    va_start(ptr, message);
    vprintf(message, ptr);

    char text[Message_len] = {};
    char cmd[Message_len]  = {};

    vsprintf(text, message, ptr);

    sprintf(cmd, "echo '%s' | festival --tts", text);

    system(cmd);

    va_end(ptr);
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
    assert(node != nullptr);
    assert(*node != nullptr);

    while ((*node)->left != nullptr && (*node)->right != nullptr)
    {
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

#define memory_allocate(ptr)                                                     \
    char *ptr = (char*) calloc(Max_input_len, sizeof(char));                     \
    if (ptr == nullptr) {                                                        \
        printf("Sorry, I can't add your character: there is no enougth memory"); \
        return;                                                                  \
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

//------------- DEFINITION MODE -----------//

static void run_definition_mode(Tree *tree) {
    printf("Enter name of character that i need to define:\n");

    char name[Max_input_len] = {};

    get_user_input(name);

    Tree_node *found = find_node(tree->head, name);

    if (found == nullptr) {
        printf("Sorry, I can't find this character.\n"
               "You can add it using guess mode by answering guestions about it.\n");
        return;
    }

    printf("%s ", found->data);

    while (found->parent->parent != nullptr) {

        if (found == found->parent->left) {
            printf("%s, ", found->parent->data);
        } else {
            printf("not %s, ", found->parent->data);
        }

        found = found->parent;
    }

    if (found == found->parent->left) {
        printf("%s.\n", found->parent->data);
    } else {
        printf("not %s.\n", found->parent->data);
    }
}

//------------- DIFFERENCE MODE -----------//

static void run_diff_mode(Tree *tree) {
    assert(tree != nullptr);

    printf("Give me two characters and I will say what do they have in common "
           "and what differences do they have. Enter first character:...\n");

    char name1[Max_input_len] = {};
    char name2[Max_input_len] = {};

    get_user_input(name1);

    printf("Enter second character:...\n");
    
    get_user_input(name2);

    Tree_node *node1 = find_node(tree->head, name1);
    Tree_node *node2 = find_node(tree->head, name2);

    if (node1 == nullptr) {
        printf("Sorry, I don't know character %s. :(\n"
               "You can add it by answering questions about it in guess mode.\n", name1);
        return;
    }

    if (node2 == nullptr) {
        printf("Sorry, I don't know character %s. :(\n"
               "You can add it by answering questions about it in guess mode.\n", name2);
        return;
    }

    if (strcasecmp(name1, name2) == 0) {
        printf("Characters are the same. You can get their definition in definition mode\n");
        return;
    }

    Stack stk1 = {};
    Stack stk2 = {};

    StackCtr(&stk1, 0);
    StackCtr(&stk2, 0);

    get_path(node1, &stk1);
    get_path(node2, &stk2);

    if (!print_commons(name1, name2, &stk1, &stk2)) {
        return;
    }

    print_difference(name1, name2, &stk1, &stk2);
}

static void get_path(Tree_node *node, Stack *stk) {

    assert(node != nullptr);
    assert(stk != nullptr);

    while (node->parent != nullptr) {

        StackPush(stk, node);

        node = node->parent;
    }
}

static bool print_commons(const char *name1, const char *name2, Stack *stk1, Stack *stk2) {

    assert(name1 != nullptr);
    assert(name2 != nullptr);
    assert(stk1  != nullptr);
    assert(stk2  != nullptr);

    if (stk1->size == 0 || stk2->size == 0) {
        printf("Sorry, but at least one of your characters is not a character, but it's property\n");

        return false;
    }

    Tree_node *node1 = StackPop(stk1);
    Tree_node *node2 = StackPop(stk2);

    if (node1 != node2) {

        printf("Characters %s and %s have nothing in common.", name1, name2);

        StackPush(stk1, node1);
        StackPush(stk2, node2);

        return true;
    }

    node1 = StackPop(stk1);
    node2 = StackPop(stk2);

    printf("%s like %s ", name1, name2);

    return print_common_prop(stk1, stk2, node1, node2);
}

static bool print_common_prop(Stack *stk1, Stack *stk2, Tree_node *node1, Tree_node *node2) {

    assert(stk1  != nullptr);
    assert(stk2  != nullptr);
    assert(node1 != nullptr);
    assert(node2 != nullptr);

    while (node1 == node2) {

        if (stk1->size == 0 || stk2->size == 0) {
            printf("%s. ", node1->parent->parent->data);

            return false;
        }

        printf("%s, ", node1->parent->parent->data);

        node1 = StackPop(stk1);
        node2 = StackPop(stk2);
    }

    printf("%s.", node1->parent->data);

    StackPush(stk1, node1);
    StackPush(stk2, node2);

    return true;
}

static void print_difference(const char *name1, const char *name2, Stack *stk1, Stack *stk2) {
    assert(name1 != nullptr);
    assert(name2 != nullptr);
    assert(stk1 != nullptr);
    assert(stk2 != nullptr);

    assert(stk1->size != 0);
    assert(stk2->size != 0);

    Tree_node *node1 = StackPop(stk1);
    Tree_node *node2 = StackPop(stk2);

    if (node1 == node2) {
        printf("There is no differences: characters are the same.\n");
        return;
    }

    printf("Unlike %s, %s ", name2, name1);

    print_properties(stk1, name1, node1);

    printf("In the same time %s ", name2);

    print_properties(stk2, name2, node2);
}

static void print_properties(Stack *stk, const char *name, const Tree_node *node) {
    assert(stk  != nullptr);
    assert(name != nullptr);

    while (stk->size != 0) {
        printf("%s, ", node->data);

        node = StackPop(stk);
    }

    printf("%s.\n", node->data);
}

/*-------------------------------- OTHER STATIC FUNCTIONS ----------------------------------------*/

#define memory_allocate(ptr, size, type)                            \
    ptr = (type*) calloc(size, sizeof(type));                       \
    if (ptr == nullptr) {                                           \
        printf("Error: can't run akinator - not enought memory\n"); \
        return false;                                               \
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
