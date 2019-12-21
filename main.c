#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "todo_lib.h"

int main(int argc, char** argv) {
    
    struct param* p = define_command(&argc, argv);

    struct task* t = load_todos(filename);

    switch(p->command) {
        case ADD:
            t = add_todo(t, argv+1);
            save_todos(t, filename);
            break;

        case UPDATE:
            update_todo(t, p->task_id, p->task_status);
            save_todos(t, filename);
            break;

        default:
            print_todos(t);
            break;
    }

    free(p);
    free_todos(t);

    return 0;
}
