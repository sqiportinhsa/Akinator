#ifndef TREE_H
#define TREE_H

#include "../Libs/logging.h"


static const char *UNSAVED_FILL__COLOR = "#BEA0A0";
static const char *UNSAVED_FRAME_COLOR = "#361C1C";

static const char *SAVED_FILL__COLOR = "#9AA5BB";
static const char *SAVED_FRAME_COLOR = "#232D42";

static const char   *SAVED_ARROW_COLOR = "#54303c";
static const char *UNSAVED_ARROW_COLOR = "#303C54";


struct Tree_node {
    bool       is_saved = false;
    char*      data     = nullptr;
    Tree_node* right    = nullptr;
    Tree_node* left     = nullptr;
    Tree_node* parent   = nullptr;
};

struct Tree {
    Tree_node*       head      = nullptr;
    Creation_logs*   logs      = nullptr;
};

struct Colors {
    const char* frame = SAVED_FRAME_COLOR;
    const char* fill  = SAVED_FILL__COLOR;
    const char* arrow = SAVED_ARROW_COLOR;
};

enum Tree_err {
    NO_TREE_ERR = 0,
    NOT_ENOUGHT_MEM = 1,
    CANNOT_GENER_PIC = 2,
};



#define init_tree(tree) real_tree_init(tree, __FILE__, __PRETTY_FUNCTION__, __LINE__);

#define dump_tree(tree, message, ...) real_dump_tree(tree, __FILE__, __PRETTY_FUNCTION__, __LINE__,\
                                                                           message, ##__VA_ARGS__);

#define Print_code(format, ...)                    \
        fprintf(code_output, format, ##__VA_ARGS__);


#define Print_node(node, node_colors)                                                      \
        Print_code("node%p [label=\"{%s}\",fillcolor=\"%s\",color=\"%s\"];\n",             \
                                     node, node->data, node_colors.fill, node_colors.frame);

#define Print_arrow(node, node_colors)                                                       \
        Print_code("node%p->node%p [color=\"%s\"];\n", node->parent, node, node_colors.arrow);


int real_tree_init(Tree* tree, const char *file, const char *func, int line);

Tree_node* init_right_node(Tree *tree, Tree_node *parent, char *data);
Tree_node* init_left_node (Tree *tree, Tree_node *parent, char *data);
 
int init_head_node(Tree *tree, char *data);

void free_node(Tree_node *node);


void tree_dtor(Tree *tree);

void real_dump_tree(const Tree *tree, const char *file, const char *func, int line, 
                                                               const char *message, ...);

void generate_graph_picture(const Tree *tree, char *picture_name);

void text_database_dump(Tree *tree, FILE *output);

void generate_file_name(char *filename, const char *extension);

#endif
