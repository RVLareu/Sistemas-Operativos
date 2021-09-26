#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void leerFiltrar(int *fd);

int
main(int argc, char *argv[])
{
/////////// Leo el número a calcular //////////////
    int n =atoi(argv[1]);

    /////////// creo pipe a derecha //////////////
    int fds[2];
    int r = pipe(fds);
    if (r == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

    /////////// Coloco arreglo de numeros naturales en pipe //////////////   
    for(int i = 2; i <= n; i++) {
        r = write(fds[1], &i, sizeof(i));
        if (r < 0) {
            perror("Error en write");
            _exit(-1);
        }
    }

    /////////// termino escritura y cierro //////////////   
    close(fds[1]);


    /////////// Leo y filtro en el siguiente proceso //////////////   

    leerFiltrar(&fds[0]);


    return 0;
}


void leerFiltrar(int *fd) {
      
    int fds[2];
    int r = pipe(fds);
    
    /////////// Creo proceso hijo ////////////// 
    int i = fork();
	if (i < 0) {
		printf("Error en fork! %d\n", i);
		_exit(-1);

	}   

    if (i == 0) {
        /////////// PROCESO HIJO //////////////   


        //valor a leer del pipe
        int siguiente;
        //primo       
        int p;

        /////////// Leo del pipe y extraigo el primo //////////////   
        int err = read(*fd, &p, sizeof(p));
        if (err == 0){
            _exit(0);
        }
        printf("%d\n", p);  

        /////////// Mientras el pipe siga abierto //////////////   
        while(err != 0) {
            err = read(*fd, &siguiente, sizeof(siguiente));
			if (err < 0) {
				perror("Error en read");
				_exit(-1);
			}
            
            if(siguiente % p != 0){
                 
                r = write(fds[1], &siguiente, sizeof(siguiente));
                if (r < 0) {
                    perror("Error en write");
                    _exit(-1);
                }
            }
        }
        /////////// cierro el de lectura que me llamó, cierro por el que escribí//////////////  
        close(fds[1]);
        close(*fd);
        /////////// llamo recursivamente con la direccion del file descriptor de lectura //////////////   
        leerFiltrar(&fds[0]);
        _exit(0);
      
        
    } else {
        close(*fd);
        close(fds[0]);
        close(fds[1]);
        /////////// PROCESO PADRE espera //////////////   
        wait(NULL);
    }
    _exit(0);
}