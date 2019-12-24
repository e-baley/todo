#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "todo_lib.h"

const char* filename = ".todolist";
const char status_tab[3] = {" ~x"};
const struct option long_options[] = {
    {"done",        required_argument,  NULL,  'd'},
    {"progress",    required_argument,  NULL,  'p'},
    {NULL,          0,                  NULL,   0}
};

struct param* define_command(int *argc, char** argv) {
    struct param* p = (struct param*) calloc(1, sizeof(struct param));
    if(p == NULL)
        exit(-1);

    p->command = VIEW;

    int c;
    if(*argc > 1) {
        while((c = getopt_long(*argc, argv, "d:p:", long_options, NULL)) != -1) {

            switch(c) {
                case 'd':
                    p->command = UPDATE;
                    p->task_status = DONE;
                    p->task_id = atoi(optarg);
                    break;

                case 'p':
                    p->command = UPDATE;
                    p->task_status = IN_PROGRESS;
                    p->task_id = atoi(optarg);
                    break;
            }
        }

        if(p->command == VIEW) p->command = ADD;
    }

    return p;
}

struct task* load_todos(const char* filename) {
    FILE* f = fopen(filename, "a+");
    if(f == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(-1);
    }

    struct task* t_head = NULL;
    struct task* t_tail = NULL;
    struct task* t = NULL;
    char* line = NULL;
    size_t len = 0;
    ssize_t nread = 0;

    while((nread = getline(&line, &len, f)) != -1) {
        t = (struct task*) calloc(1, sizeof(struct task));
        if(t == NULL)
            exit(-1);
        
        t->description = (char*) calloc(len, sizeof(char));
        sscanf(line, "%d %d %[^\t\n]", &t->id, &t->status, t->description);

        if(t_head == NULL) t_head = t;
        if(t_tail != NULL) t_tail->next_task = t;
        t_tail = t;
    }

    free(line);
    fclose(f);

    return t_head;
}

void display_todos(struct task* t, FILE* f, char* format, int status_filter, unsigned short int status_text) {
    if(t == NULL)
        return;

    if(status_filter == -1 || status_filter == t->status) {
        if(status_text == 1)
            fprintf(f, format, t->id, status_tab[t->status], t->description);
        else
            fprintf(f, format, t->id, t->status, t->description);
    }

    display_todos(t->next_task, f, format, status_filter, status_text);
}

void save_todos(struct task* t, const char* filename) {
    FILE* f = fopen(filename, "w");
    if(f == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(-1);
    }

    display_todos(t, f, "%d %d %s\n", -1, 0);

    fclose(f);
}

void free_todos(struct task* t) {
    if(t != NULL) {
        free(t->description);
        free(t);

        if(t->next_task == NULL)
            return;

        free_todos(t->next_task);
    }
}

struct task* add_todo(struct task* t, char** description) { 
    struct task* t_head = t;
    while(t != NULL && t->next_task != NULL) t = t->next_task;

    struct task* n = (struct task*) calloc(1, sizeof(struct task*));
    if(n == NULL)
        exit(-1);

    n->id = (t != NULL) ? t->id + 1 : 1;
    n->status = PENDING;
    n->description = create_description(description);
    n->next_task = NULL;

    if(t_head == NULL)
        t_head = n;
    else
        t->next_task = n;

    return t_head;
}

char* create_description(char** ptr_str) {
    char* description = (char*) calloc(get_description_length(ptr_str), sizeof(char));
    if(description == NULL)
        exit(-1);

    strcpy(description, "");
    while(*(ptr_str) != NULL) {
        strcat(description, *(ptr_str));
        if(*(ptr_str+1) != NULL) strcat(description, " ");
        ++ptr_str;
    }
    
    return description;
}

long get_description_length(char** ptr_str) {
    long total_length = 0;
    
    while(*(ptr_str) != NULL) {
        total_length += strlen(*(ptr_str)) + 1;
        ++ptr_str;
    }

    return total_length;
}

void update_todo(struct task* t, int task_id, int task_status) {
    while(t != NULL && t->id != task_id && t->next_task != NULL) t = t->next_task;

    if(t != NULL && t->id == task_id) t->status = task_status;
}

void print_todos(struct task* t) {
    display_todos(t, stdout, "[%d] [%c] %s\n", IN_PROGRESS, 1);
    display_todos(t, stdout, "[%d] [%c] %s\n", PENDING, 1);
    display_todos(t, stdout, "[%d] [%c] %s\n", DONE, 1);
}

