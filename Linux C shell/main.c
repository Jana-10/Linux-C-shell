#include "headers.h"
#include "util.h"
#include "history.h"
#include "kill.h"
#include "parser.h"
#include <signal.h>

char *shellname();

char *shellName;
char showDir[size];
char homeDir[size];
char currDir[size];

void updateShowDir()
{
    int homeDirLen = (int)strlen(homeDir);
    if (strlen(currDir) < homeDirLen)
    {
        strcpy(showDir, currDir);
        if (showDir[strlen(showDir) - 1] == '/')
        {
            showDir[strlen(showDir) - 1] = '\0';
        }
        return;
    }
    for (int i = 0; i < homeDirLen; i++)
    {
        if (homeDir[i] != currDir[i])
        {
            strcpy(showDir, currDir);
            return;
        }
    }
    strcpy(showDir, "~/");
    strcat(showDir, currDir + homeDirLen);
    if (showDir[strlen(showDir) - 1] == '/')
    {
        showDir[strlen(showDir) - 1] = '\0';
    }
}

char *shellname()
{
    char login[1000];
    char hostname[1000];

    if (getlogin_r(login, 1000) == 0)
    {
    }
    else
    {
        perror("Cant get login name");
    }

    struct utsname sysName;
    if (uname(&sysName) == 0)
    {
    }
    else
    {
        perror("Cant get system name");
    }

    if (gethostname(hostname, 1000) == 0)
    {
    }
    else
    {
        perror("Cant get hostname");
    }
    char *user_name = (char *)malloc(500);
    strcpy(user_name, login);
    strcat(user_name, "@");
    strcat(user_name, hostname);
    strcat(user_name, ":");
    return user_name;
}

void cd_handler(char *token[])
{
    char cd_location[size];
    strcpy(cd_location, token[1]);
    char new_address[size];
    get_raw_address(new_address, cd_location, currDir, homeDir);
    struct stat stats_dir;
    if (stat(new_address, &stats_dir) == 0 && (S_IFDIR & stats_dir.st_mode))
    {
        if (chdir(new_address) == -1)
        {
            printf("cd : directory does not exist\n");
        }
        if (getcwd(currDir, size) == NULL)
        {
            printf("cd : getcwd failed\n");
        }
        strcat(currDir, "/");
        updateShowDir();
    }
    else
    {
        printf("cd : directory does not exist: %s\n", token[1]);
    }
}

void pwd_handler()
{
    printf("%s\n", currDir);
}

void echo_handler(char *tokens[], int n)
{
    for (int i = 1; i < n; i++)
    {
        printf("%s", tokens[i]);
    }
    printf("\n");
}

void processInput(char **tokens, int num_tokens, int bg, int *pipe, int prev_open, char *oldcommand)
{

    if (strcmp(tokens[0], "cd") == 0)
    {
        if (num_tokens == 1)
        {
            tokens[1] = malloc(4);
            strcpy(tokens[1], "~");
        }
        cd_handler(tokens);
    }
    else if (strcmp(tokens[0], "pwd") == 0)
    {
        pwd_handler();
    }
    else if (strcmp(tokens[0], "ls") == 0)
    {
        ls_handler(tokens, num_tokens, currDir, homeDir);
    }
    else if (strcmp(tokens[0], "echo") == 0)
    {
        echo_handler(tokens, num_tokens);
    }
    else if (strcmp(tokens[0], "exit") == 0)
    {
        killbg();
        _exit(0);
    }
    else if (strcmp(tokens[0], "clear") == 0)
    {
        clearScreen();
    }
    else if (strcmp(tokens[0], "pinfo") == 0)
    {
        if (num_tokens == 1)
        {
            tokens[1] = malloc(10);
            sprintf(tokens[1], "%d", getpid());
        }
        pinfo_handler(tokens);
    }
    else if (strcmp(tokens[0], "history") == 0)
    {
        if (num_tokens == 1)
        {
            show_history(10);
        }
        else
        {
            if (strtol(tokens[1], NULL, 10) <= 0 || strtol(tokens[1], NULL, 10) > 20)
            {
                fprintf(stderr, "history <int n> \n n > 0 && n <= 20\n");
                exit_code = 1;
                return;
            }
            show_history(atoi(tokens[1]));
        }
    }
    else if (strcmp(tokens[0], "jobs") == 0)
    {
        job_printer();
    }
    else if (strcmp(tokens[0], "sig") == 0)
    {
        sig(tokens, num_tokens);
    }
    else if (strcmp(tokens[0], "fg") == 0)
    {
        fg_handler(tokens, num_tokens);
    }
    else if (strcmp(tokens[0], "bg") == 0)
    {
        bg_handler(tokens, num_tokens);
    }
    else
        make_process(tokens, num_tokens, bg, pipe, prev_open, oldcommand);
}

void get_commands(char *line)
{
    char *command;
    char line2[size], line3[size];
    strcpy(line2, line);
    strcpy(line3, line);
    command = strtok(line, ";&");
    int c = 0;
    while (command != NULL)
    {
        c++;
        command = strtok(NULL, ";&");
    }
    char *commands[c + 1];
    int i = 0;
    if (c <= 0)
        return;
    char *beg = line2;
    commands[0] = strtok(line2, ";&");

    while (commands[i] != NULL && strcmp(commands[i], "") != 0)
    {
        i++;
        commands[i] = strtok(NULL, ";&");
    }
    for (int j = 0; j < c; j++)
    {
        bool bg = false;
        if (line3[strlen(commands[j]) + (commands[j] - beg)] == '&')
        {
            bg = true;
        }
        //redirectionHandler(commands[j], bg);
        int backup_stdout = dup(STDOUT_FILENO);
        int backup_stdin = dup(STDIN_FILENO);
        pipeChecker(commands[j], bg);
        fixInput(backup_stdin, backup_stdout);
    }
}

void rip_child(int signum)
{
    if (signum == SIGCHLD)
        kill_check();
}

void exit_2()
{
    killbg();
    _exit(0);
}

char *trim_whitespace(char *line)
{
    int t = 0;
    for (int i = 0; i < strlen(line); i++)
    {
        if (line[i] == ' ' || line[i] == '\t' || line[i] == '\n')
        {
            t++;
        }
        else
        {
            break;
        }
    }
    for (int i = 0; i < t; i++)
    {
        line++;
    }
    for (int i = strlen(line) - 1; i >= 0; i--)
    {
        if (line[i] == ' ' || line[i] == '\t' || line[i] == '\n')
        {
            line[i] = '\0';
        }
        else
        {
            break;
        }
    }
    return line;
}

int main()
{
    clearScreen();
    shellName = shellname();
    if (getcwd(homeDir, size) == NULL)
    {
        perror("getcwd failed");
    }
    if (homeDir[strlen(homeDir) - 1] != '/')
    {
        strcat(homeDir, "/");
    }
    signal(SIGCHLD, rip_child);
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    strcpy(currDir, homeDir);
    updateShowDir();

    while (1)
    {
        printCyan();
        printf("%s", shellName);
        printGreen();
        printf("%s ", showDir);
        printYellow();
        printf("$ ");
        resetColor();
        char *line = malloc(size);
        size_t t = size;
        char *line2 = line;
        if (fgets(line, size, stdin) == NULL)
        {
            exit_2();
        }
        size_t ln = strlen(line) - 1;
        if (*line && line[ln] == '\n')
            line[ln] = '\0';
        line = trim_whitespace(line);
        add_history(line);
        get_commands(line);
        free(line2);
        printYellow();
        resetColor();
    }
}
