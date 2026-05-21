#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "common.h"
#include "utils.h"
#include "fs.h"

/* ================= HELP ================= */

void print_help(void)
{
    printf("\n%sTerminal Simulator%s\n", GREEN, RESET);
    printf("--------------------------------\n");
    printf("help      show commands\n");
    printf("pwd       current path\n");
    printf("ls        list files\n");
    printf("tree      show tree\n");
    printf("cd        change dir\n");
    printf("mkdir     create dir\n");
    printf("touch     create file\n");
    printf("edit      edit file\n");
    printf("cat       read file\n");
    printf("rm        delete\n");
    printf("chmod     change permission\n");
    printf("history   command history\n");
    printf("clear     clear screen\n");
    printf("finder    open finder\n");
    printf("exit      quit\n\n");
}

/* ================= MAIN ================= */

int main(void)
{
    TreeNode* root = create_tree_node(NULL, "/");
    root->type = 'd';

    TreeNode* pwd = root;

    printf("%sLoading filesystem...%s\n", GREEN, RESET);
    sync_local_tree(root, "/", 0);
    printf("%sReady.%s\n", GREEN, RESET);

    print_help();

    StringArray history;
    history.data = NULL;
    history.size = 0;

    char cmd[1024];

    while(1)
    {
        char* p = pwd_str(root, pwd);
        printf("user@terminal:%s$ ", p);
        free(p);

        if(!fgets(cmd, sizeof(cmd), stdin))
        {
            break;
        }

        cmd[strcspn(cmd, "\n")] = 0;

        if(strlen(cmd) == 0)
        {
            continue;
        }

        history.data = realloc(history.data, sizeof(char*) * (history.size + 1));
        history.data[history.size++] = strdup(cmd);

        StringArray args = split(cmd, ' ');

        if(args.size == 0)
        {
            free_string_array(args);
            continue;
        }

        /* ================= COMMANDS ================= */

        if(strcmp(args.data[0], "exit") == 0)
        {
            free_string_array(args);
            break;
        }

        else if(strcmp(args.data[0], "help") == 0)
        {
            print_help();
        }

        else if(strcmp(args.data[0], "pwd") == 0)
        {
            char* s = pwd_str(root, pwd);
            printf("%s\n", s);
            free(s);
        }

        else if(strcmp(args.data[0], "ls") == 0)
        {
            print_ls(root, pwd);
        }

        else if(strcmp(args.data[0], "tree") == 0)
        {
            print_tree(pwd->child, 0);
        }

        else if(strcmp(args.data[0], "cd") == 0 && args.size > 1)
        {
            TreeNode* temp = cd(root, pwd, args.data[1]);
            if(temp)
            {
                pwd = temp;
            }
        }

        else if(strcmp(args.data[0], "mkdir") == 0 && args.size > 1)
        {
            create(root, pwd, args.data[1], 'd');
        }

        else if(strcmp(args.data[0], "touch") == 0 && args.size > 1)
        {
            create(root, pwd, args.data[1], '-');
        }

        else if(strcmp(args.data[0], "edit") == 0 && args.size > 1)
        {
            edit(root, pwd, args.data[1]);
        }

        else if(strcmp(args.data[0], "cat") == 0 && args.size > 1)
        {
            cat(root, pwd, args.data[1]);
        }

        else if(strcmp(args.data[0], "rm") == 0 && args.size > 1)
        {
            remove_node(root, pwd, args.data[1]);
        }

        else if(strcmp(args.data[0], "chmod") == 0 && args.size > 2)
        {
            chmod2(root, pwd, args.data[2], args.data[1]);
        }

        else if(strcmp(args.data[0], "clear") == 0)
        {
            system("clear");
        }

        else if(strcmp(args.data[0], "finder") == 0)
        {
#ifdef __APPLE__
            char real[2048];
            build_real_path(root, pwd, real);
            char finder_cmd[2500];
            snprintf(finder_cmd, sizeof(finder_cmd), "open \"%s\"", real);
            system(finder_cmd);
#endif
        }

        else if(strcmp(args.data[0], "history") == 0)
        {
            for(int i = 0; i < history.size; i++)
            {
                printf("%d %s\n", i + 1, history.data[i]);
            }
        }

        else
        {
            printf("%sunknown command%s\n", RED, RESET);
        }

        free_string_array(args);
    }

    free_string_array(history);
    free_tree(root);

    return 0;
}
