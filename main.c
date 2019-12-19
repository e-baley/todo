#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* filename = ".todolist";

enum CMD_TYPE {VIEW, INSERT, COMPLETE};
enum TASK_STATUS {PENDING, IN_PROGRESS, DONE};

struct task {
    int id;
    int status;
    char* description;
    struct task* next_task;
};

int define_command(int *argc, char** argv);
struct task* load_todos(const char* filename);
void display_todos(struct task* t, FILE* f);
void save_todos(struct task* t, const char* filename);
void free_todos(struct task* t);
struct task* add_todo(struct task* t, char** description);
char* create_description(char** ptr_str);
long get_description_length(char** ptr_str);
void complete_todo(struct task* t, char** cmd);

int define_command(int *argc, char** argv) {
    if(*argc <= 1)
        return VIEW;

    if(*(argv+1)[0] == '-')
        return COMPLETE;

    return INSERT;
}

struct task* load_todos(const char* filename) {
    FILE* f = fopen(filename, "r+");
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
        t = (struct task*) malloc(sizeof(struct task));
        if(t == NULL)
            exit(-1);
        
        t->description = (char*) malloc(sizeof(char) * len);
        sscanf(line, "%d %d %[^\t\n]", &t->id, &t->status, t->description);

        if(t_head == NULL) t_head = t;
        if(t_tail != NULL) t_tail->next_task = t;
        t_tail = t;
    }

    free(line);
    fclose(f);

    return t_head;
}

void display_todos(struct task* t, FILE* f) {
    if(t == NULL)
        return;

    fprintf(f, "%d %d %s\n", t->id, t->status, t->description);

    display_todos(t->next_task, f);
}

void save_todos(struct task* t, const char* filename) {
    FILE* f = fopen(filename, "w");
    if(f == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(-1);
    }

    display_todos(t, f);

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
    while(t->next_task != NULL) t = t->next_task;

    struct task* n = (struct task*) malloc(sizeof(struct task*));
    if(n == NULL)
        exit(-1);

    n->id = t->id + 1;
    n->status = PENDING;
    n->description = create_description(description);
    n->next_task = NULL;

    if(t_head == NULL) t_head = n;
    t->next_task = n;

    return t_head;
}

char* create_description(char** ptr_str) {
    char* description = (char*) malloc(get_description_length(ptr_str) * sizeof(char));
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

void complete_todo(struct task* t, char** cmd) {
    int id = 0;
    while(*(cmd) != NULL) {
        if(strcmp(*(cmd), "-id") == 0 || strcmp(*(cmd), "-i") == 0) {
            id = atoi(*(cmd+1));
            ++cmd;
        }
        ++cmd;
    }

    while(t->id != id && t->next_task != NULL) t = t->next_task;

    if(t->id == id) t->status = DONE;
}

int main(int argc, char** argv) {
    
    int mode = define_command(&argc, argv);

    struct task* t = load_todos(filename);

    switch(mode) {
        case INSERT:
            t = add_todo(t, argv+1);
            save_todos(t, filename);
            break;

        case COMPLETE:
            complete_todo(t, argv+1);
            save_todos(t, filename);
            break;

        default:
            display_todos(t, stdout);
            break;
    }

    free_todos(t);

    return 0;
}
