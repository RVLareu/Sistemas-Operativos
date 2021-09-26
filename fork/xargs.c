#include <stdio.h>
#include <unistd.h>      
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>


#ifndef NARGS
#define NARGS 4
#endif

int main(int argc, char* argv[]) {

    FILE *stream;
    char *buff = NULL;
    size_t len = 0;
    ssize_t read;

    stream = fopen(argv[2], "r");
    if (stream == NULL) {
        stream = stdin;
    }

    while ((read = getline(&buff, &len, stream)) != -1) {
        strtok(buff, "\n");
        char *args[] = {argv[1], buff, NULL};
        
        int i = fork();
        if (i == 0) {
            execvp(argv[1], args);
        } else {
           wait(NULL);
        }
    }
    free(buff);
    fclose(stream);
    exit(0);
    return 0;

}