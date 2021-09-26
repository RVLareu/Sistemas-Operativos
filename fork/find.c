#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

//si no la declaro el compilador no la toma, a diferencia de strstr
char * strcasestr(const char*, const char*);


int recursiveFind (DIR *parent, char* pathToPrint, char* subString, char* (*locateSubString)(const char*,const char*)) {
    
    int fd;
    struct dirent *entry;
    DIR *directory;
    
    while ((entry = readdir(parent)) != NULL) {
        //DIRECTORIO
        if (entry->d_type == DT_DIR) {
            //Si es . o .. continue
            if ((strstr(entry->d_name, ".") != NULL) || (strstr(entry->d_name, "..")) != NULL) {
                continue;
            }

            char newPath[PATH_MAX];
            //Si el nomber del directiorio contiene substring, imprimo
            if (locateSubString(entry->d_name, subString) != NULL) {
                printf("%s/%s\n", pathToPrint, entry->d_name);
            }
            //concateno el path por el que venía + / + el nombre del dir actual -> queda en newPath
            snprintf(newPath, PATH_MAX, "%s/%s", pathToPrint, entry->d_name);
            //abro el directiorio en el que estoy parado
            fd = openat(dirfd(parent), entry->d_name, O_DIRECTORY); 
            if (fd > 0) {
                //paso de int a DIR*
                directory = fdopendir(fd);
                //llamo recursivamente con el directorio abierto, el camino hasta ahora, la substring y la funcion de busqueda de string
                recursiveFind(directory, newPath, subString, locateSubString);
                closedir(directory);

            } else {
                perror("error en openat");
            }
        //ARCHIVO -> si tiene la substring imprimo y finalizo    
        } else if (locateSubString(entry->d_name, subString) != NULL) {
            printf("%s/%s\n", pathToPrint, entry->d_name);
        }
    }
    return 0;
}

int main (int argc, char *argv[]) {
    DIR *dir = opendir(".");
    if (dir == NULL) {
        perror("error con opendir");
        exit(-1);
    }

    if (argc > 2) {
        recursiveFind(dir,".", argv[2], strcasestr);
    } else {
        recursiveFind(dir,".", argv[1], strstr);
    }

    return 0;
}