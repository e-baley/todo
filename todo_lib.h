#ifndef _TODO_H
#define _TODO_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

extern const char* filename;
extern const char status_tab[3];
extern const struct option long_options[];

enum CMD_TYPE {VIEW, ADD, UPDATE};
enum TASK_STATUS {PENDING, IN_PROGRESS, DONE};

struct param {
    int command;
    int task_status;
    int task_id;
};

struct task {
    int id;
    int status;
    char* description;
    struct task* next_task;
};

struct param* define_command(int *argc, char** argv);
struct task* load_todos(const char* filename);
void display_todos(struct task* t, FILE* f, char* format, int status_filter, unsigned short int status_text);
void save_todos(struct task* t, const char* filename);
void free_todos(struct task* t);
struct task* add_todo(struct task* t, char** description);
char* create_description(char** ptr_str);
long get_description_length(char** ptr_str);
void update_todo(struct task* t, int task_id, int task_status);
void print_todos(struct task* t);

#endif
