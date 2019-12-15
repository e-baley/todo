#include <stdio.h>
#include <stdlib.h>

const char* filename = ".todolist";
const char* filename_tmp = ".todolisttmp";

enum CMD_TYPE {VIEW, INSERT, COMPLETE};

int define_command(int *argc, char** argv);
int insert_todo(FILE* f, char** todo);
int view_todo(FILE* f, FILE* out, int skip_first_row);
int get_current_id(FILE* f);

int define_command(int *argc, char** argv) {
    if(*argc <= 1)
        return VIEW;

    if(*(argv+1)[0] == '-')
        return COMPLETE;

    return INSERT;
}

int insert_todo(FILE* f, char** todo) {
    if(f == NULL) return -1;

    FILE* out = fopen(filename_tmp, "w");
    if(out == NULL) return -1;

    int id_todo = get_current_id(f)+1;
    fprintf(out, "%d\n", id_todo);
    view_todo(f, out, 1);
    
    fprintf(out, "%d-", id_todo);
    while(1) {
        fprintf(out, "%s", *(todo));
        todo = todo+1;
        if(*(todo) == NULL) break;
        fprintf(out, " ");
    }
    fprintf(out, "\n");

    fclose(out);
    remove(filename);
    rename(filename_tmp, filename);
   
    return 0;
}

int view_todo(FILE* f, FILE* out, int skip_first_row) {
    if(f == NULL) return -1;

    char* line = NULL;
    size_t len = 0;
    ssize_t nread = 0;

    if(skip_first_row == 1) getline(&line, &len, f);
    while((nread = getline(&line, &len, f)) != -1)
        fwrite(line, 1, nread, out);

    free(line);
    return 0;
}

int get_current_id(FILE* f) {
    if(f == NULL) return -1;

    fseek(f, 0L, SEEK_SET);

    int id = 0;
    fscanf(f, "%d", &id);

    return id;
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
            view_todo(f, stdout, 0);
            break;
    }

    if(fclose(f) == EOF) {
        fprintf(stderr, "Error closing file %s\n", filename);
        return -1;
    }

    return 0;
}
