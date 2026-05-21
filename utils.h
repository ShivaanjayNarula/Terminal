#ifndef UTILS_H
#define UTILS_H

/* ================= STRING ARRAY ================= */

typedef struct {
    char** data;
    int size;
} StringArray;

void free_string_array(StringArray arr);
StringArray split(const char* str, char delim);
char* join(StringArray arr, char delim);
void split_name(const char* path, char** parent, char** name);

/* ================= TIME ================= */

char* time_now(void);

#endif /* UTILS_H */
