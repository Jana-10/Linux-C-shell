#ifndef UNTITLED_UTIL_H
#define UNTITLED_UTIL_H
char showDir[size];
char homeDir[size];
char currDir[size];
void get_raw_address(char *new_address, char *cd_location, const char *curr_dir, const char *home_dir);

void printGreen();
int exit_code;
void printBlue();
void printYellow();
void printCyan();
void welcomeMessage();
void resetColor();
void clearScreen();
int max(int a, int b);
int min(int a, int b);
char *trim_whitespace(char *);
void pwd_handler();
void echo_handler(char *tokens[], int num);
void cd_handler(char *[]);
char *shellname();
void updateShowDir();

#endif
