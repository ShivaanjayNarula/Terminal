#ifndef FS_H
#define FS_H

#include "utils.h"

/* ================= TREE NODE ================= */

typedef struct TreeNode {
    char* name;
    char type; /* d or - */
    int perm;
    char* cdate;
    char* mdate;
    StringArray content;
    struct TreeNode* parent;
    struct TreeNode* child;
    struct TreeNode* link;
} TreeNode;

TreeNode* create_tree_node(TreeNode* parent, const char* name);
const char* get_perm_str(int p);
char* pwd_str(TreeNode* root, TreeNode* pwd);
TreeNode* find_on_pwd(TreeNode* pwd, const char* name);
TreeNode* cd(TreeNode* root, TreeNode* pwd, const char* path);
void build_real_path(TreeNode* root, TreeNode* node, char* out);
TreeNode* create(TreeNode* root, TreeNode* pwd, const char* path, char type);
TreeNode* find_node(TreeNode* root, TreeNode* pwd, const char* path);
void print_ls(TreeNode* root, TreeNode* pwd);
void print_tree(TreeNode* node, int depth);
void cat(TreeNode* root, TreeNode* pwd, const char* path);
void edit(TreeNode* root, TreeNode* pwd, const char* path);
void free_tree(TreeNode* node);
void remove_node(TreeNode* root, TreeNode* pwd, const char* path);
void chmod2(TreeNode* root, TreeNode* pwd, const char* path, const char* mode);
void sync_local_tree(TreeNode* current, const char* real_path, int depth);
void sync_node_lazy(TreeNode* root, TreeNode* node);

#endif /* FS_H */
