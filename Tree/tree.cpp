#include <stdio.h>

#include "tree.h"
#include "file_reading.hpp"


static void generate_node_code(Tree_node *node);

static Colors get_colors(Tree_node *node);

static void generate_file_name(char *filename, const char *extension);


static const int max_file_with_graphviz_code_name_len = 30;
static const int max_generation_png_command_len = 100;
static const int max_png_file_name_len = 30;


#define memory_allocate(ptr, size, type)                                                      \
        ptr = (type*) calloc(size, sizeof(type));                                             \
        if (ptr == nullptr) {                                                                 \
            dump_tree(tree, "can't allocate memory: not enought free mem\n");                 \
            tree_dtor(tree);                                                                  \
            return NOT_ENOUGTH_MEM;                                                           \
        }


int real_tree_init(Tree* tree, const char *file, const char *func, int line) {
    memory_allocate(tree->logs, 1, Creation_logs);

    init_cr_logs(tree->logs, file, func, line);

    return NO_TREE_ERR;
}

static Tree_node init_node(Tree_node *parent, bool is_left, char *data) {
    assert(parent != nullptr);

    Tree_node *node = nullptr;

    memory_allocate(node, 1, Tree_node);

    node->left  = nullptr;
    node->right = nullptr;
 
    node->data  = data;

    node->parent = parent;

    if (is_left) {

        parent->left  = node;

    } else {

        parent->right = node;

    }

    return node;
}

Tree_node init_right_node(Tree_node *parent, char *data) {
    return init_node(parent, false, data);
}

Tree_node init_left_node(Tree_node *parent, char *data) {
    return init_node(parent, true, data);
}

int tree_dtor(Tree *tree) {
    assert(tree != nullptr);

    free(tree->logs);
    
    free_node(tree->head);

    tree->head      = nullptr;
    tree->logs      = nullptr;
}

void free_node(Tree_node *node) {
    if (node == nullptr) {
        return;
    }

    free_node(node->left);
    free_node(node->right);

    free(node);
}

int init_head_node(Tree *tree, char *data) {
    assert(tree != nullptr);

    memory_allocate(tree->head, 1, sizeof(Tree_node));

    tree->head->parent = nullptr;

    tree->head->right  = nullptr;
    tree->head->left   = nullptr;

    tree->head->data = data;
}

void real_dump_tree(const Tree *tree, const char *file, const char *func, int line, 
                                                               const char *message, ...) {
    
    FILE *output = GetLogStream();

    fprintf(output, "<b>Tree dump called in %s(%d), function %s: ", file, line, func);

    va_list ptr = {};
    va_start(ptr, message);
    vfprintf(output, message, ptr);
    va_end(ptr);

    fprintf(output, "\n</b>");
    
    if (tree == nullptr) {
        fprintf(output, "Can't dump tree from nullptr pointer\n");
        return errors;
    }

    fprintf(output, "Tree [%p] ", tree);

    if (tree->logs == nullptr) {
        fprintf(output, "without creation info (logs ptr in nullptr):\n");
    } else {
        fprintf(output, "created at %s(%d), function %s:\n", list->cr_logs->file_of_creation, 
                                                             list->cr_logs->line_of_creation, 
                                                             list->cr_logs->func_of_creation);
    }

    fflush(output);

    if (tree->head == nullptr) {
        fprintf(output, "\tCan't print data: tree root does not exist\n");
    } else {
        fprintf(output, "\tTree data visualisation:\n");
        generate_graph_code(tree);
    }

    fprintf(output, "\n");
    fprintf(output, "\n<hr>\n");
    fflush(output);

}

void generate_graph_code(Tree *tree) {
    char code_filename[max_file_with_graphviz_code_name_len] = {};
    generate_file_name(code_filename, "dot");

    FILE *code_output = fopen(code_filename, "w");

    Print_code("digraph G{\n");
    Print_code("rankdir=LR;");
    Print_code("node [shape=record,style=\"filled\"];\n");
    Print_code("splines=ortho;\n");

    generate_node_code(tree->head);

    Print_code("}");

    fclose(code_output);

    char command[max_generation_png_command_len] = {};

    char png_file_name[max_png_file_name_len] = {};

    generate_file_name(png_file_name, "png");

    sprintf(command, " c:\\GitHub\\Akinator\\Libs\\Graphviz\\bin\\dot.exe %s -o %s -T png", 
                                                             code_filename, png_file_name);


    if (system(command) != 0) {
        PrintToLogs("Error: can't generate picture. Text dump:\n");
        dump_list_data(list, GetLogStream());
        return CANNOT_GENER_PIC;
    }

    #ifdef LOGS_TO_HTML
    fprintf(GetLogStream(), "\n<img src=\"%s\">\n", png_file_name);
    #else
    fprintf(GetLogStream(), "Picture is generated. You can find it by name %s.\n", png_file_name);
    #endif

    return NO_TREE_ERRORS;
}

static void generate_node_code(Tree_node *node) {
    Colors node_colors = get_colors(node);

    Print_node(node, node_colors);
    
    if (node->parent) {
        Print_arrow(node, node_colors);
    }

    if (node->left) {
        generate_node_code(node->left);
    }

    if (node->right) {
        generate_node_code(node->right);
    }
}

static Colors get_colors(Tree_node *node) {
    Colors colors = {};

    if (node->is_saved) {
        colors.fill  = SAVED_FILL__COLOR;
        colors.frame = SAVED_FRAME_COLOR;
    } else {
        colors.fill  = UNSAVED_FILL__COLOR;
        colors.frame = UNSAVED_FRAME_COLOR;
    }

    return colors;
}

static void generate_file_name(char *filename, const char *extension)  {
    static int file_with_graphviz_code_counter = 0;
    sprintf(filename, "Graphs/graph_%d.%s", file_with_graphviz_code_counter, extension);
    ++file_with_graphviz_code_counter;
}
