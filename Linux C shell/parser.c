//
// Created by Pulak Malhotra on 15/09/20.
//

#include "parser.h"
#include "headers.h"
#include "util.h"
#include "pinfo.h"
#include "kill.h"
#include "process_maker.h"
#include <signal.h>
#include "history.h"

int tokenize(const char *token, char *string, char *tokens[100])
{
    int len_tok = strlen(token);
    int len_str = strlen(string);
    int count = 0;
    int start = 0;
    for (int i = 0; i < len_str - len_tok;)
    {
        bool found = true;
        for (int j = 0; j < len_tok; j++)
        {
            if (token[j] != string[i + j])
            {
                found = false;
                break;
            }
        }
        if (found)
        {

            for (int k = i; k < i + len_tok; k++)
                string[k] = '\0';
            tokens[count] = malloc(size);
            strcpy(tokens[count], string + start);
            count++;
            tokens[count] = malloc(size);
            strcpy(tokens[count], token);
            count++;
            start = i + len_tok;
            i += len_tok;
        }
        else
        {
            i += 1;
        }
    }
    tokens[count] = malloc(size);
    strcpy(tokens[count], string + start);
    count++;

    return count;
}

void changeInput(char *token, char *file)
{
    if (strcmp(token, ">") == 0)
    {

        int new_fd;
        if ((new_fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
        {
            exit_code = 1;
            perror("cannot redirect output");
        }
        else
        {
            close(STDOUT_FILENO);
            dup(new_fd);
            close(new_fd);
        }
    }
    else if (strcmp(token, ">>") == 0)
    {

        int new_fd;
        if ((new_fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644)) == -1)
        {
            exit_code = 1;
            perror("cannot redirect output");
        }
        else
        {
            close(STDOUT_FILENO);
            dup(new_fd);
            close(new_fd);
        }
    }
    else if (strcmp(token, "<") == 0)
    {

        int new_fd;

        if ((new_fd = open(file, O_RDONLY)) == -1)
        {
            perror("cannot redirect input");
            exit_code = 1;
        }
        else
        {
            close(STDIN_FILENO);
            dup(new_fd);
            close(new_fd);
        }
    }
}

void fixInput(int in, int out)
{
    dup2(in, 0);
    close(in);
    dup2(out, 1);
    close(out);
}

void redirectionHandler(char *input, int bg, int *pipe, int prev_open)
{
    char *tokens[1000];
    int num_tokens = 0;
    char input2[size];
    strcpy(input2, input);
    tokens[0] = strtok(input, " \t\n");
    while (tokens[num_tokens] != NULL)
    {
        tokens[++num_tokens] = strtok(NULL, " \t");
    }
    if (num_tokens == 0)
    {
        return;
    }
    char tokens_append[100][1000];
    int n = 0;
    for (int i = 0; i < num_tokens; i++)
    {
        char *new_tokens[100];
        int c = tokenize(">>", tokens[i], new_tokens);
        for (int j = 0; j < c; j++)
        {
            if (strcmp(new_tokens[j], "") != 0)
                strcpy(tokens_append[n++], new_tokens[j]);
            free(new_tokens[j]);
        }
    }
    // >
    char tokens_append_out[100][1000];
    num_tokens = n;
    n = 0;
    for (int i = 0; i < num_tokens; i++)
    {

        char *new_tokens[100];
        if (strcmp(tokens_append[i], ">>") == 0)
        {
            strcpy(tokens_append_out[n++], tokens_append[i]);
            continue;
        }
        int c = tokenize(">", tokens_append[i], new_tokens);
        for (int j = 0; j < c; j++)
        {
            if (strcmp(new_tokens[j], "") != 0)
            {
                strcpy(tokens_append_out[n++], new_tokens[j]);
            }
            free(new_tokens[j]);
        }
    }

    char tokens_final[100][1000];
    num_tokens = n;
    n = 0;
    for (int i = 0; i < num_tokens; i++)
    {

        char *new_tokens[100];
        int c = tokenize("<", tokens_append_out[i], new_tokens);
        for (int j = 0; j < c; j++)
        {
            if (strcmp(new_tokens[j], "") != 0)
                strcpy(tokens_final[n++], new_tokens[j]);
            free(new_tokens[j]);
        }
    }

    char *command_tokens[1000];
    int num_word_command = 0;
    for (int i = 0; i < n; i++)
    {
        char *word = tokens_final[i];
        if (strcmp(word, ">") == 0 || strcmp(word, ">>") == 0 || strcmp(word, "<") == 0)
        {
            if (i + 1 == n || tokens_final[i + 1] == NULL)
            {
                fprintf(stderr, "unexpected token after %s \n", word);
                exit_code = 1;
                return;
            }
            changeInput(word, tokens_final[i + 1]);
            i++;
        }
        else
        {
            command_tokens[num_word_command] = malloc(size);
            strcpy(command_tokens[num_word_command], tokens_final[i]);
            num_word_command++;
        }
    }
    processInput(command_tokens, num_word_command, bg, pipe, prev_open, input2);
    for (int i = 0; i < num_word_command; i++)
        free(command_tokens[i]);
}

void pipeChecker(char *cmd, int bg)
{
    int pipee = 0;
    for (int i = 0; i < strlen(cmd); i++)
        if (cmd[i] == '|')
            pipee++;
    if (pipee == 0)
    {
        exit_code = 0;
        redirectionHandler(cmd, bg, NULL, -1);
        return;
    }
    else if (cmd[0] == '|' || cmd[strlen(cmd) - 1] == '|')
    {
        fprintf(stderr, "Pipe does not have both ends \n");
        exit_code = 1;
        return;
    }
    char *commands[1000];
    int n = 0;
    char *t = strtok(cmd, "|");
    while (t != NULL)
    {
        commands[n] = malloc(size);
        strcpy(commands[n], t);
        t = strtok(NULL, "|");
        n++;
    }

    int out = dup(1);
    int in = dup(0);
    int prev_open = -1;
    for (int i = 0; i < n - 1; i++)
    {
        int pipes[2];
        if (pipe(pipes) == -1)
        {

            return;
        }
        if (prev_open != -1)
        {
            dup2(prev_open, 0);

            close(prev_open);
        }
        dup2(pipes[1], 1);

        close(pipes[1]);

        redirectionHandler(commands[i], bg, pipes, prev_open);
        prev_open = pipes[0];
        free(commands[i]);
    }
    dup2(out, 1);

    close(out);
    if (prev_open != -1)
    {
        dup2(prev_open, 0);

        close(prev_open);
    }
    exit_code = 0;
    redirectionHandler(commands[n - 1], bg, NULL, prev_open);
    dup2(in, 0);
    close(in);
}
