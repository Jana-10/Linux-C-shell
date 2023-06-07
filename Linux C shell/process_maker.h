#ifndef UNTITLED_PROCESS_MAKER_H
#define UNTITLED_PROCESS_MAKER_H
void make_process(char *tokens[], int num, int bg, int *pipe, int, char *);
int remove_child(int pid);
void job_printer();
void sig(char *[], int);
void overkill_handler(char *tokens[], int n);
void fg_handler(char *tokens[], int n);
void bg_handler(char *tokens[], int n);

#endif
