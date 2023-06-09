
#include "headers.h"
#include "history.h"
#include "util.h"

#define history_file "/tmp/.shell_history"

void add_history(char tokens[])
{
    if (strcmp(tokens, "") == 0)
        return;
    ;
    FILE *f;
    f = fopen(history_file, "a");
    fclose(f);
    f = fopen(history_file, "r");
    char *lines[100];
    int n = 0;

    if (f != NULL)
    {

        lines[n] = malloc(size);
        size_t s = size;
        while (getline(&lines[n], &s, f) != -1)
        {
            if (strcmp(lines[n], "\n") != 0)
                lines[++n] = malloc(size);
        }
    }
    fclose(f);

    f = fopen(history_file, "w");
    int i;
    for (i = max(0, n - 20); i < n; i++)
    {
        fprintf(f, "%s", lines[i]);
    }

    char new[size];
    sprintf(new, "%s\n", tokens);
    if (i == 0 || strcmp(new, lines[i - 1]) != 0)
    {
        fprintf(f, "%s", new);
    }
    fclose(f);
    for (i = 0; i <= n; i++)
    {
        free(lines[i]);
    }
}

void show_history(int no)
{
    FILE *f;
    f = fopen(history_file, "a");
    fclose(f);
    f = fopen(history_file, "r");
    char *lines[100];
    int n = 0;
    if (f != NULL)
    {
        lines[n] = malloc(size);
        size_t s = size;
        while (getline(&lines[n], &s, f) != -1)
        {
            if (strcmp(lines[n], "\n") != 0)
                lines[++n] = malloc(size);
        }
    }
    fclose(f);
    for (int i = max(0, n - no); i < n; i++)
    {
        printf("%s\n", lines[i]);
        free(lines[i]);
    }
}