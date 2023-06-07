#include "headers.h"
#include "pinfo.h"
#include "util.h"

void pinfo_handler(char *tokens[])
{

    char add[size];
    char exe[size];
    strcpy(exe, "defunct :");
    sprintf(add, "/proc/%s/stat", tokens[1]);
    FILE *ptr = fopen(add, "r");
    if (ptr == NULL)
    {
        fprintf(stderr, "Cannot access the process \n");
        exit_code = 1;
        return;
    }
    int pid;
    char exec_name[1000];
    char st;
    fscanf(ptr, "%d ", &pid);
    fscanf(ptr, "%s ", exec_name);
    fscanf(ptr, "%c ", &st);
    sprintf(add, "/proc/%s/exe", tokens[1]);
    int t = readlink(add, exe, size);
    if (t != -1)
    {
        exe[t] = '\0';
    }
    else
    {

        strcat(exe, exec_name);
    }
    sprintf(add, "/proc/%s/status", tokens[1]);
    fclose(ptr);
    ;
    ptr = fopen(add, "r");
    if (ptr == NULL)
    {
        fprintf(stderr, "Cannot access the process \n");
        exit_code = 1;
        return;
    }
    char word[size];
    while (fscanf(ptr, " %499s", word) == 1)
    {
        if (strcmp(word, "VmSize:") == 0)
        {
            fscanf(ptr, " %499s", word);
            break;
        }
    }
    fclose(ptr);
    printf("pid -- %d\n", pid);
    printf("Process Status -- %c\n", st);
    printf("memory -- %s\n", word);
    printf("Executable Path -- %s\n", exe);
    printf("\n");
}
