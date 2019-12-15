#include <stdio.h>
#include <stdlib.h>

const char* filename = ".todolist";

enum CMD_TYPE {VIEW, INSERT, COMPLETE};

int define_command(int *argc, char** argv);
int insert_todo(FILE* f, char** todo);

int define_command(int *argc, char** argv) {
    if(*argc <= 1)
        return VIEW;

    if(*(argv+1)[0] == '-')
        return COMPLETE;

    return INSERT;
}

int insert_todo(FILE* f, char** todo) {
    if(f == NULL) return -1;

    while(1) {
        fprintf(f, "%s", *(todo));
        todo = todo+1;
        if(*(todo) == NULL) break;
        fprintf(f, " ");
    }
    fprintf(f, "\n");
    
    return 0;
}

int view_todo(FILE* f) {
    if(f == NULL) return -1;

    char* line = NULL;
    size_t len = 0;
    ssize_t nread = 0;

    while((nread = getline(&line, &len, f)) != -1)
        fwrite(line, nread, 1, stdout);

    free(line);
    return 0;
}

int main(int argc, char** argv) {
    FILE* f = fopen(filename, "a+");
    
    int mode = define_command(&argc, argv);

    if(f == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return -1;
    }

    switch(mode) {
        case INSERT:
            insert_todo(f, argv+1);
            break;

        case COMPLETE:
            break;

        default:
            view_todo(f);
            break;
    }

    if(fclose(f) == EOF) {
        fprintf(stderr, "Error closing file %s\n", filename);
        return -1;
    }

    return 0;
}
