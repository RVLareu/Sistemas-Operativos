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

    if (argc <= 1) {
        pritnf("Faltan argumentos");
        return 0;
    }
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

    /* ESTO SERíA CON NARGS implementado si es que entendí bien la funcionalidad
    int numLineRead = 0;
    char *buff = NULL;
    char buffToPass[40] = ""; 
    while (1) {
        read = getline(&buff, &len, stream); 
        strcat(buffToPass, buff); 
        numLineRead +=1;
        if (read == -1 || numLineRead == NARGS) {
            if (read == -1 && numLineRead == 1) break;
            numLineRead = 0;
            char *args[] = {argv[1], buffToPass, NULL}; 
            int i = fork();
            if (i == 0) {
                execvp(argv[1], args);
            } else {
            wait(NULL);
            }
        }

    }
    */

    free(buff);
    fclose(stream);
    exit(0);
    return 0;

}

