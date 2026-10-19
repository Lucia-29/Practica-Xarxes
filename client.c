// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>

#define DEFAULT_PORT 8080
#define DEFAULT_DOMAIN "127.0.0.1"
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
	int sock = 0;
	struct sockaddr_in serv_addr;
	char buffer[BUFFER_SIZE] = {0};
	char cadena[BUFFER_SIZE] = "";

	char *domini;
	int port;
	char *endptr;

	if (argc == 1) {
		// Cap argument -> valors per defecte
		domini = DEFAULT_DOMAIN;
		port = DEFAULT_PORT;
		printf("Cap argument indicat. Utilitzant valors per defecte: %s %d\n", domini, port);
	} 
	else if (argc == 3) {
		domini = argv[1];

		errno = 0;
		long valor_port = strtol(argv[2], &endptr, 10);

		// Comprovacions de validesa
		if (errno != 0 || endptr == argv[2] || *endptr != '\0'
    || valor_port < 1 || valor_port > 65535) {
			fprintf(stderr, "Error: el port ha de ser un enter entre 1 i 65535.\n");
			exit(EXIT_FAILURE);
		}
		port = (int)valor_port;
	} 
	else {
		fprintf(stderr, "Ús: %s [nom_de_domini port]\n", argv[0]);
		exit(EXIT_FAILURE);
	}



	// Crear el socket
	// Afegiu comentari explicant els arguments, i de quines altres opcions hi ha
	// per al segon d'ells (ara SOCK_STREAM)
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\nError en crear el socket\n");
		return -1;
	}

	// AF_INET: indica que utilitzarem direccions IPv4.
	// SOCK_STREAM: indica que volem un sicket de tipus fluxe, que utilitza TCP.
	//0: deixa que el sistema operatiu triï el protocol predeterminat corresponent a aquesta combinació.
	// per a SOCK_STREAM tambe existeix altres opcions coom SOCK_DGRAM, que utilitza datagramas y normalment es combina amb UDP.

	// Afegiu comentari sobre què són aquests 3 paràmetres,
	// per què s'utilitzen els valors que hi ha,
	// i per què s'utilitza htons()

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	//inet_pton(AF_INET, domini, &serv_addr.sin_addr);	// Convertir adreça IPv4 a binari.
   // serv_addr.sin_family = AF_INET -> indica la familia de direccions que utilitzarem; IPv4.
   //serv_addr.sin_port = htons(port) -> indica el port que volem conectar. 
   // htons() converteix valors de 16 bits, com un port.
   // htonl() converteix valors de 32 bits, com una direcció IPv4 representada com enter.
   // 

   if (inet_pton(AF_INET, domini, &serv_addr.sin_addr) !=1){
	fprintf(stderr, "Error: l'adreça IPv4 no és vàlida. \n");
	close(sock);
	return EXIT_FAILURE;
   }

	// Connectar amb el servidor
	// Afegiu comentari explicant els arguments
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("\nConnexio fallida\n");
		close(sock);
		return -1;
	}

	// sock: es el descriptor del socket que hem utilitzat creat amb socket();
	// (struct sockaddr *)&serv_addr: es la direcció del servidor al que volem conectar. 
	// sizeof(serv_addr): indica el tamany, en bytes, de la estructura que conté la direcció del servidor. 

	printf("Connectat al servidor. Podeu començar a enviar missatges.\n");

	int option;

	while (1) {
		memset(buffer, 0, sizeof(buffer)); // Netegem el buffer

		// Menú principal
		// Cal que implementeu un petit servei remot amb almenys 4 funcionalitats noves
		// Definiu vosaltres mateixos les dades a enviar (demanar en el client, i analitzar al servidor) i rebre
		// Si per donar més sentit al servei cal alguna opció més, la podeu afegir
		// Deixeu l'opció 1 com a Enviar missatge i l'última per Sortir

		printf("Menú principal:\n");
		printf("1. Enviar missatge\n");
		printf("2. Cercar un perfum\n");
		printf("3. Afegir un perfum a la cistella\n");
		printf("4. Consultar el total de la cistella\n");
		printf("5. Buidar la cistella\n");
		printf("6. Sortir\n");
		printf("Opció: ");

		if(scanf("%d", &option)!=1)
		{
			printf("Entrada invàlida. Introdueix un número. \n");

			while (getchar() != '\n');
			continue;
		}
		while (getchar() != '\n');  // buida el buffer fins al salt de línia

		switch (option)
		{
			case 1:
			case 6:
		
				if (option==1){
					printf("Introdueix el missatge a enviar ('EXIT' per tancar el servidor i sortir): ");
					fgets(cadena, BUFFER_SIZE, stdin);
					cadena[strcspn(cadena, "\n")] = '\0';  // Eliminar \n final
				} else {
					strcpy(cadena, "EXIT");
				}

				// Enviar missatge al servidor
				// Afegiu control d'errors
				// Afegiu comentari explicant els arguments
				// sock: socket utilitzat per a la comunicació.
				// cadena: dades que volem enviar.
				// strlen(cadena): nombre de bytes del missatge, sense el '\0' final.
				// 0: no s'especifiquen opcions addicionals.
				if (send(sock, cadena, strlen(cadena), 0) < 0) {
   	 				perror("Error en enviar el missatge");
    				close(sock);
    				return EXIT_FAILURE;
				}

				// Llegir resposta del servidor
				// Afegiu un control d'errors al recv()
				// Afegiu comentari explicant què fa i per què s'utilitza memset
				// Afegiu comentari explicant els arguments de la crida a recv()

				memset(buffer, 0, BUFFER_SIZE);

				int bytes_rebuts = recv(sock, buffer, BUFFER_SIZE - 1, 0);
				if (bytes_rebuts <= 0) {
    				if (bytes_rebuts == 0) {
        				printf("El servidor ha tancat la connexió.\n");
    				} else {
        				perror("Error en rebre la resposta");
    				}

					close(sock);
					return EXIT_FAILURE;
				}

				buffer[bytes_rebuts] = '\0';
				printf("Resposta del servidor: %s\n", buffer);

				//memset: posa a cero el contingut del buffer anterior, per no barrejar les dades de diferents respostes. 
				//sock: descriptor del socket per el que rebem les dades. 
				// buffer: zona de memoria on guardem les dades rebudes. 
				// BUFFER_SIZE -1: Indica el número máxim de bytes que volem rebre en aquesta trucada.
				// 0: opcions adicionals de la trucada.



				if (strcmp(cadena, "EXIT") == 0) {
					close(sock);
					return 0;
				}
				break;
		

			case 2:
			{
				// Implementar Opció 2
				int id_perfum;
				int bytes_rebuts;

				printf("Introdueix l'ID del perfum que vols cercar: ");

				if(scanf("%d", &id_perfum) != 1)
				{
					printf("Entrada invàlida. Indrodueix un número. \n");
					while (getchar() != '\n');
					break;
				}
				while (getchar() != '\n');

				//preparar la petició per al servidor. 
				snprintf(cadena, sizeof(cadena), "SEARCH|%d", id_perfum);

				//Enviar la petició al servidor. 
				if (send(sock, cadena, strlen(cadena), 0) < 0)
				{
					perror("Error en enviar la cerca");
					close(sock);
					return EXIT_FAILURE;
				}

				//Netejar el buffer abans de rebre la resposta. 
				memset(buffer, 0, sizeof(buffer));

				//Rebre la resposta del servidor. 
				bytes_rebuts = recv(sock, buffer, BUFFER_SIZE - 1, 0);

				if(bytes_rebuts <= 0){
					if(bytes_rebuts == 0)
					{
						printf("El servidor ha tancat la connexió. \n");
					} else{
						perror("Error en rebre la resposta");
					}

					close(sock);
					return EXIT_FAILURE;
				}

				buffer[bytes_rebuts] = '\0';
				printf("Resposta del servidor: %s\n", buffer);

				break;
			}

			case 3:
			{
				// Implementar Opció 3
				int id_perfum;
				int bytes_rebuts;

				printf("Introduiex l'ID del perfum que vols afegir: ");

				if(scanf("%d", &id_perfum) != 1){
					printf("Entrada invàlida. Introdueix un número. \n");

					while (getchar() != '\n');

					break;
				}

				while (getchar() != '\n');

				snprintf(cadena, sizeof(cadena), "ADD|%d", id_perfum);

				if(send(sock, cadena, strlen(cadena), 0) < 0)
				{
					perror("Error en enviar la petició");
					close (sock);
					return EXIT_FAILURE;
				}

				memset(buffer, 0, sizeof(buffer));

				bytes_rebuts = recv(sock, buffer, BUFFER_SIZE -1, 0);

				if (bytes_rebuts <= 0)
				{
					if(bytes_rebuts == 0)
					{
						printf("El servidor ha tancat la connexió. \n");
					}else
					{
						perror("Error en rebre la resposta");
					}

					close (sock);
					return EXIT_FAILURE;
				}
					buffer[bytes_rebuts] = '\0';
					printf("Resposta del servidor: %s\n", buffer);

					break;
			}


			case 4:
			{
				// Implementar Opció 4
				int bytes_rebuts;

    			// Enviar la petició per consultar el total.
    			strcpy(cadena, "TOTAL");

    			if (send(sock, cadena, strlen(cadena), 0) < 0) {
       				perror("Error en enviar la petició del total");
        			close(sock);
        			return EXIT_FAILURE;
   				}

    			// Netejar el buffer abans de rebre la resposta.
    			memset(buffer, 0, sizeof(buffer));

    			// Rebre la resposta del servidor.
    			bytes_rebuts = recv(sock, buffer, BUFFER_SIZE - 1, 0);
				printf("DEBUG: bytes rebuts = %d\n", bytes_rebuts);

				if (bytes_rebuts <= 0) {
        			if (bytes_rebuts == 0){
            			printf("El servidor ha tancat la connexió.\n");
        			
					} else {
            			perror("Error en rebre el total");
        			}

        		close(sock);
        		return EXIT_FAILURE;
				}
    		

    			buffer[bytes_rebuts] = '\0';

				printf("Resposta del servidor: [%s]\n", buffer);
				printf("Primer byte rebut: %d\n", (unsigned char)buffer[0]);

    			break;
			}
		

			case 5:
			{
				int bytes_rebuts;

    			// Preparar la petició per buidar la cistella.
    			strcpy(cadena, "CLEAR");

    			// Enviar la petició al servidor.
    			if (send(sock, cadena, strlen(cadena), 0) < 0) {
        			perror("Error en enviar la petició");
       				close(sock);
        			return EXIT_FAILURE;
    			}

    			// Netejar el buffer abans de rebre la resposta.
   				memset(buffer, 0, sizeof(buffer));

   				// Rebre la resposta del servidor.
    			bytes_rebuts = recv(sock, buffer, BUFFER_SIZE - 1, 0);

    			if (bytes_rebuts <= 0) {
        			if (bytes_rebuts == 0) {
            			printf("El servidor ha tancat la connexió.\n");
        			} else {
            			perror("Error en rebre la resposta");
        			}

        			close(sock);
        			return EXIT_FAILURE;
    			}

    			buffer[bytes_rebuts] = '\0';

    			printf("Resposta del servidor: %s\n", buffer);

    			break;
			}

			default:
			{
				printf("Opció invàlida\n");
				break;
			}
		}
	}

	close(sock);
	return 0;
	
}
