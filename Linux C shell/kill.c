#include "headers.h"
#include "kill.h"
#include "ctype.h"
#include <signal.h>
#include "process_maker.h"

void kill_check()
{
    int status;
    int reaped_rc;
    struct dirent *dir_stuff;
    DIR *dir = opendir("/proc");
    if (dir == NULL)
    {
        //perror("cannot access /proc");
        return;
    }
    int pids[size];
    char *namess[size];
    int child = 0;
    while ((dir_stuff = readdir(dir)) != NULL)
    {
        if (isdigit(dir_stuff->d_name[0]))
        {
            char add[1000];
            sprintf(add, "/proc/%s/stat", dir_stuff->d_name);
            FILE *f = fopen(add, "r");
            if (f == NULL)
            {
                continue;
            }
            int pid, ppid;
            char state;
            char name[size];
            fscanf(f, "%d %s", &pid, name);
            fscanf(f, " %c ", &state);
            fscanf(f, " %d", &ppid);
            if (ppid == (int)getpid())
            {
                pids[child] = pid;
                namess[child] = strdup(name);
                child++;
            }
            fclose(f);
        }
    }
    closedir(dir);

    while ((reaped_rc = waitpid(-1, &status, WNOHANG)) > 0)
    {
        char stat[200];
        int job = remove_child(reaped_rc);
        if (WIFEXITED(status))
        {
            int t = WEXITSTATUS(status);
            sprintf(stat, "normally with status %d", t);
        }
        else if (WIFSIGNALED(status))
        {
            int t = WTERMSIG(status);
            sprintf(stat, "with signal %d", t);
        }
        else
        {
            sprintf(stat, "exited somehow");
        }

        char text[size];
        char name[1000];
        strcpy(name, " ");
        for (int i = 0; i < child; i++)
        {
            if (pids[i] == reaped_rc)
            {
                strcpy(name, namess[i]);
                break;
            }
        }
        int len = sprintf(text, "\n-[%d]%s (%d) has exited %s\n", job, name, reaped_rc, stat);
        write(2, text, len);
    }
}

void killbg()
{
    int status;
    struct dirent *dir_stuff;
    DIR *dir = opendir("/proc");
    if (dir == NULL)
    {
        //perror("cannot access /proc");
        return;
    }
    int child = 0;
    while ((dir_stuff = readdir(dir)) != NULL)
    {
        if (isdigit(dir_stuff->d_name[0]))
        {
            char add[1000];
            sprintf(add, "/proc/%s/stat", dir_stuff->d_name);
            FILE *f = fopen(add, "r");
            if (f == NULL)
            {
                continue;
            }
            int pid, ppid;
            char state;
            char name[size];
            fscanf(f, "%d %s", &pid, name);
            fscanf(f, " %c ", &state);
            fscanf(f, " %d", &ppid);
            if (ppid == (int)getpid())
            {
                child++;
                kill(pid, SIGKILL);
            }
            fclose(f);
        }
    }
    closedir(dir);
    while (waitpid(-1, &status, WNOHANG) > 0)
    {
    }
}
