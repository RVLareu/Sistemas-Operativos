#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int
main(void)
{

	printf("Hola, soy PID %d:\n", getpid());

	////////// Timer para aleatoriedad //////////////

	time_t timer;
	srand(time(&timer));

	////////// Creo primer Pipe//////////////

	int fdsp[2];
	int rp = pipe(fdsp);
	
	if (rp == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	////////// Creo segundo Pipe//////////////

	int fdsc[2];
	int rc = pipe(fdsc);
	
	if (rc == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}


	printf("  - primer pipe me devuelve: [%d,%d] \n", fdsp[0],fdsp[1]);
	printf("  - segundo pipe me devuelve: [%d,%d] \n\n", fdsc[0],fdsc[1]);


	////////// Creo Proceso Hijo//////////////	


	int i = fork();

	if (i < 0) {
		printf("Error en fork! %d\n", i);
		_exit(-1);

	}

	long int recibido;

	if (i == 0) {
			//////////Proceso Hijo//////////////	
			
			printf("Donde fork me devuelve 0:\n");
			printf("  - getpid me devuelve: %d\n", getpid());
			printf("  - getppid me devuelve: %d\n", getppid());


			rc = read(fdsp[0], &recibido, sizeof(recibido));
			if (rc < 0) {
				perror("Error en read");
				_exit(-1);
			}
			printf("  - recibo valor %ld vía fd=%d\n", recibido, fdsp[0]);
			
			rc = write(fdsc[1], &recibido, sizeof(recibido));
			if (rc < 0) {
				perror("Error en write");
				_exit(-1);
			}
			printf("  - reenvío valor en fd=%d y termino\n\n",fdsc[1]);
			//termino al proceso hijo
			_exit(0);

	} else {
			//////////Proceso Padre//////////////	
			
			long int rand = random();
			printf("Donde fork me devuelve %d:\n", i);
			printf("  - getpid me devuelve: %d\n", getpid());
			printf("  - getppid me devuelve: %d\n", getppid());
			printf("  - random me devuelve: %ld\n", rand);

			rp = write(fdsp[1], &rand, sizeof(rand));
			if (rp < 0) {
				perror("Error en write");
				_exit(-1);
			}
			printf("  - envío valor %ld a través de fd=%d\n\n",rand, fdsp[1]);
	}
	
	rp = read(fdsc[0], &recibido, sizeof(recibido));

	printf("Hola, de nuevo PID %d\n", getpid());
	printf("- recibí valor %ld vía fd=%d\n", recibido, fdsc[0]);
	

	close(fdsp[0]);
	close(fdsp[1]);	
	close(fdsc[0]);
	close(fdsc[1]);

	_exit(0);

	return 0;
}