#include <stdio.h>

const char* filename = ".todolist";

enum CMD_TYPE {VIEW, INSERT, COMPLETE};

int define_command(int *argc, char** argv);

int define_command(int *argc, char** argv) {
    if(*argc <= 1)
        return VIEW;

    if(*(argv+1)[0] == '-')
        return COMPLETE;

    return INSERT;
}

int main(int argc, char** argv) {
    FILE* f = fopen(filename, "a+");
    
    int mode = define_command(&argc, argv);
    printf("mode %d\n", mode);

    if(f == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return -1;
    }

    if(fclose(f) == EOF) {
        fprintf(stderr, "Error closing file %s\n", filename);
        return -1;
    }

    return 0;
}
