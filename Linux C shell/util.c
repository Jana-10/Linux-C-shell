#include "headers.h"
#include "util.h"

int max(int a, int b)
{
    return a > b ? a : b;
}

int min(int a, int b)
{
    return a < b ? a : b;
}

void get_raw_address(char *new_address, char *cd_location, const char *curr_dir, const char *home_dir)
{
    if (cd_location[0] == '/')
    {

        strcpy(new_address, cd_location);
    }
    else if (cd_location[0] == '~')
    {
        strcpy(new_address, home_dir);
        strcat(new_address, cd_location + 1);
    }
    else
    {

        strcpy(new_address, curr_dir);
        if (new_address[strlen(new_address) - 1] != '/')
            strcat(new_address, "/");
        strcat(new_address, cd_location);
    }
}

void printGreen()
{
    printf("%s", "\033[1m\033[32m");
}

void printBlue()
{
    printf("%s", "\033[1m\033[34m");
}

void printCyan()
{
    printf("%s", "\033[1m\033[36m");
}

void resetColor()
{
    printf("%s", "\033[1m\033[0m");
}

void printYellow()
{
    printf("%s", "\033[1m\033[33m");
}

void clearScreen()
{
    printf("\e[1;1H\e[2J");
}
