#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils.h"

void free_string_array(StringArray arr)
{
    for(int i = 0; i < arr.size; i++)
    {
        free(arr.data[i]);
    }
    free(arr.data);
}

char* time_now(void)
{
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char* buf = malloc(64);
    strftime(buf, 64, "%d %b %H:%M", t);
    return buf;
}

StringArray split(const char* str, char delim)
{
    StringArray arr;
    arr.data = NULL;
    arr.size = 0;

    if(!str || strlen(str) == 0)
    {
        return arr;
    }

    char* copy = strdup(str);
    char d[2] = {delim, '\0'};
    char* token = strtok(copy, d);

    while(token)
    {
        arr.data = realloc(arr.data, sizeof(char*) * (arr.size + 1));
        arr.data[arr.size++] = strdup(token);
        token = strtok(NULL, d);
    }

    free(copy);
    return arr;
}

char* join(StringArray arr, char delim)
{
    if(arr.size == 0)
    {
        return strdup("");
    }

    int len = 0;
    for(int i = 0; i < arr.size; i++)
    {
        len += strlen(arr.data[i]) + 1;
    }

    char* res = malloc(len + 1);
    res[0] = '\0';

    for(int i = 0; i < arr.size; i++)
    {
        strcat(res, arr.data[i]);
        if(i != arr.size - 1)
        {
            int l = strlen(res);
            res[l] = delim;
            res[l + 1] = '\0';
        }
    }

    return res;
}

void split_name(const char* path, char** parent, char** name)
{
    StringArray arr = split(path, '/');

    if(arr.size == 0)
    {
        *parent = strdup("");
        *name = strdup("");
        return;
    }

    *name = strdup(arr.data[arr.size - 1]);
    arr.size--;
    *parent = join(arr, '/');

    free_string_array(arr);
}
