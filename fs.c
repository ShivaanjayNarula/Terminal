#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "common.h"
#include "utils.h"
#include "fs.h"

TreeNode* create_tree_node(TreeNode* parent, const char* name)
{
    TreeNode* node = malloc(sizeof(TreeNode));
    node->name = strdup(name);
    node->type = '-';
    node->perm = 7;
    node->cdate = time_now();
    node->mdate = strdup(node->cdate);
    node->content.data = NULL;
    node->content.size = 0;
    node->parent = parent;
    node->child = NULL;
    node->link = NULL;
    return node;
}

const char* get_perm_str(int p)
{
    switch (p)
    {
        case 0: return "---";
        case 1: return "--x";
        case 2: return "-w-";
        case 3: return "-wx";
        case 4: return "r--";
        case 5: return "r-x";
        case 6: return "rw-";
        case 7: return "rwx";
    }
    return "rwx";
}

char* pwd_str(TreeNode* root, TreeNode* pwd)
{
    if(pwd == root)
    {
        return strdup("/");
    }
    
    char temp[4096] = "";
    TreeNode* curr = pwd;

    while(curr && curr != root)
    {
        char newtemp[4096];
        snprintf(newtemp, sizeof(newtemp), "/%s%s", curr->name, temp);
        strcpy(temp, newtemp);
        curr = curr->parent;
    }
    
    return strdup(temp);
}

TreeNode* find_on_pwd(TreeNode* pwd, const char* name)
{
    if(!pwd)
    {
        return NULL;
    }

    TreeNode* temp = pwd->child;
    while(temp)
    {
        if(strcmp(temp->name, name) == 0)
        {
            return temp;
        }
        temp = temp->link;
    }

    return NULL;
}

TreeNode* cd(TreeNode* root, TreeNode* pwd, const char* path)
{
    if(!path)
    {
        return pwd;
    }

    if(strcmp(path, "/") == 0)
    {
        return root;
    }

    if(strcmp(path, ".") == 0)
    {
        return pwd;
    }

    TreeNode* curr = (path[0] == '/') ? root : pwd;
    StringArray arr = split(path, '/');

    for(int i = 0; i < arr.size; i++)
    {
        if(strcmp(arr.data[i], "..") == 0)
        {
            if(curr->parent)
            {
                curr = curr->parent;
            }
            continue;
        }

        if(strcmp(arr.data[i], ".") == 0)
        {
            continue;
        }

        sync_node_lazy(root, curr);

        TreeNode* next = find_on_pwd(curr, arr.data[i]);
        if(!next || next->type != 'd')
        {
            printf("%sdirectory not found%s\n", RED, RESET);
            free_string_array(arr);
            return NULL;
        }

        curr = next;
    }

    free_string_array(arr);
    return curr;
}

void build_real_path(TreeNode* root, TreeNode* node, char* out)
{
    char* p = pwd_str(root, node);
    strcpy(out, p);
    free(p);
}

TreeNode* create(TreeNode* root, TreeNode* pwd, const char* path, char type)
{
    char* parent_path;
    char* name;

    split_name(path, &parent_path, &name);

    TreeNode* dir = cd(root, pwd, parent_path);
    free(parent_path);

    if(!dir)
    {
        free(name);
        return NULL;
    }

    sync_node_lazy(root, dir);

    if(find_on_pwd(dir, name))
    {
        printf("%salready exists%s\n", RED, RESET);
        free(name);
        return NULL;
    }

    TreeNode* node = create_tree_node(dir, name);
    node->type = type;

    if(!dir->child)
    {
        dir->child = node;
    }
    else
    {
        TreeNode* t = dir->child;
        while(t->link)
        {
            t = t->link;
        }
        t->link = node;
    }

    char real[2048];
    build_real_path(root, dir, real);
    strcat(real, "/");
    strcat(real, name);

    if(type == 'd')
    {
        mkdir(real, 0777);
    }
    else
    {
        FILE* f = fopen(real, "w");
        if(f)
        {
            fclose(f);
        }
    }

    free(name);
    return node;
}

TreeNode* find_node(TreeNode* root, TreeNode* pwd, const char* path)
{
    char* parent;
    char* name;

    split_name(path, &parent, &name);

    TreeNode* dir = cd(root, pwd, parent);
    free(parent);

    if(!dir)
    {
        free(name);
        return NULL;
    }

    sync_node_lazy(root, dir);

    TreeNode* res = find_on_pwd(dir, name);
    free(name);
    return res;
}

void print_ls(TreeNode* root, TreeNode* pwd)
{
    if(!pwd)
    {
        return;
    }

    sync_node_lazy(root, pwd);

    TreeNode* node = pwd->child;
    while (node)
    {
        printf("%c%s %s ", node->type, get_perm_str(node->perm), node->mdate);

        if(node->type == 'd')
        {
            printf("%s%s/%s\n", BLUE, node->name, RESET);
        }
        else
        {
            printf("%s%s%s\n", GREEN, node->name, RESET);
        }

        node = node->link;
    }
}

void print_tree(TreeNode* node, int depth)
{
    while (node)
    {
        for(int i = 0; i < depth; i++)
        {
            printf("│   ");
        }

        printf("├── ");

        if(node->type == 'd')
        {
            printf("%s%s/%s\n", BLUE, node->name, RESET);
        }
        else
        {
            printf("%s%s%s\n", GREEN, node->name, RESET);
        }

        print_tree(node->child, depth + 1);
        node = node->link;
    }
}

void cat(TreeNode* root, TreeNode* pwd, const char* path)
{
    TreeNode* node = find_node(root, pwd, path);

    if(!node)
    {
        printf("%sfile not found%s\n", RED, RESET);
        return;
    }

    char real[2048];
    build_real_path(root, node->parent, real);
    strcat(real, "/");
    strcat(real, node->name);

    FILE* f = fopen(real, "r");
    if(!f)
    {
        return;
    }

    char line[1024];
    while(fgets(line, sizeof(line), f))
    {
        printf("%s", line);
    }

    fclose(f);
}

void edit(TreeNode* root, TreeNode* pwd, const char* path)
{
    TreeNode* node = find_node(root, pwd, path);

    if(!node)
    {
        node = create(root, pwd, path, '-');
    }

    if(!node)
    {
        return;
    }

    char real[2048];
    build_real_path(root, node->parent, real);
    strcat(real, "/");
    strcat(real, node->name);

    FILE* f = fopen(real, "w");
    if(!f)
    {
        return;
    }

    printf("%sEnter text (type \\n to save)%s\n", YELLOW, RESET);

    char line[1024];

    free_string_array(node->content);
    node->content.data = NULL;
    node->content.size = 0;

    while (1)
    {
        if(!fgets(line, sizeof(line), stdin))
        {
            break;
        }

        line[strcspn(line, "\n")] = 0;

        if(strcmp(line, "\\n") == 0)
        {
            break;
        }

        node->content.data = realloc(node->content.data, sizeof(char*) * (node->content.size + 1));
        node->content.data[node->content.size++] = strdup(line);

        fprintf(f, "%s\n", line);
    }

    fclose(f);

    free(node->mdate);
    node->mdate = time_now();
}

void free_tree(TreeNode* node)
{
    if(!node)
    {
        return;
    }

    free_tree(node->child);
    free_tree(node->link);

    free(node->name);
    free(node->cdate);
    free(node->mdate);

    free_string_array(node->content);
    free(node);
}

void remove_node(TreeNode* root, TreeNode* pwd, const char* path)
{
    TreeNode* node = find_node(root, pwd, path);

    if(!node)
    {
        printf("%snot found%s\n", RED, RESET);
        return;
    }

    char real[2048];
    build_real_path(root, node->parent, real);
    strcat(real, "/");
    strcat(real, node->name);

    if(node->type == 'd')
    {
        rmdir(real);
    }
    else
    {
        remove(real);
    }

    TreeNode* parent = node->parent;

    if(parent->child == node)
    {
        parent->child = node->link;
    }
    else
    {
        TreeNode* temp = parent->child;
        while(temp && temp->link != node)
        {
            temp = temp->link;
        }

        if(temp)
        {
            temp->link = node->link;
        }
    }

    node->child = NULL;
    node->link = NULL;

    free_tree(node);
}

void chmod2(TreeNode* root, TreeNode* pwd, const char* path, const char* mode)
{
    TreeNode* node = find_node(root, pwd, path);

    if(!node)
    {
        printf("%snot found%s\n", RED, RESET);
        return;
    }

    node->perm = atoi(mode);

    char real[2048];
    build_real_path(root, node->parent, real);
    strcat(real, "/");
    strcat(real, node->name);

    chmod(real, 0777);
}

void sync_local_tree(TreeNode* current, const char* real_path, int depth)
{
    if(depth > 2)
    {
        return;
    }

    DIR* dir = opendir(real_path);
    if(!dir)
    {
        return;
    }

    struct dirent* entry;
    while((entry = readdir(dir)))
    {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char full[2048];
        snprintf(full, sizeof(full), "%s/%s", real_path, entry->d_name);

        struct stat st;
        if(stat(full, &st) == 0)
        {
            TreeNode* node = create_tree_node(current, entry->d_name);
            node->type = S_ISDIR(st.st_mode) ? 'd' : '-';

            if(!current->child)
            {
                current->child = node;
            }
            else
            {
                TreeNode* t = current->child;
                while(t->link)
                {
                    t = t->link;
                }
                t->link = node;
            }

            if(node->type == 'd')
            {
                sync_local_tree(node, full, depth + 1);
            }
        }
    }

    closedir(dir);
}

void sync_node_lazy(TreeNode* root, TreeNode* node)
{
    if(!node || node->type != 'd')
    {
        return;
    }

    char real_path[2048];
    build_real_path(root, node, real_path);

    DIR* dir = opendir(real_path);
    if(!dir)
    {
        if(errno == EACCES || errno == EPERM)
        {
            printf("%sPermission denied: Cannot access '%s'%s\n", RED, real_path, RESET);
        }
        return;
    }

    struct dirent* entry;
    while((entry = readdir(dir)))
    {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        if(find_on_pwd(node, entry->d_name))
        {
            continue;
        }

        char full[2048];
        snprintf(full, sizeof(full), "%s/%s", real_path, entry->d_name);

        struct stat st;
        if(stat(full, &st) == 0)
        {
            TreeNode* child = create_tree_node(node, entry->d_name);
            child->type = S_ISDIR(st.st_mode) ? 'd' : '-';

            if(!node->child)
            {
                node->child = child;
            }
            else
            {
                TreeNode* t = node->child;
                while(t->link)
                {
                    t = t->link;
                }
                t->link = child;
            }
        }
    }

    closedir(dir);
}
