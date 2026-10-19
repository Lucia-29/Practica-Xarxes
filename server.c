// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <stddef.h>
#define DEFAULT_PORT 8080
#define BUFFER_SIZE 1024
#define NUM_PERFUMS (sizeof(catalog)/sizeof(catalog[0]))


struct Perfum{
	int id;
	char nom[50];
	float preu;
};

struct Perfum catalog[] = {
	{1, "Chanel Nº5", 95.0},
	{2, "Dior Sauvage", 85.0},
	{3, "Gucci Bloom", 75.0},
	{4, "Yves Saint Laurent Black Opium", 90.0},
	{5, "Versace Eros", 80.0}
};

struct ElementCistella{
	int id_perfum;
	int unitats;
};

struct ElementCistella cistella[NUM_PERFUMS] ={0};

int main(int argc, char *argv[]) {
	int sock, new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	char buffer[BUFFER_SIZE] = {0};

	int port;
	char *endptr;

	if (argc == 1) {
		// Cap argument -> port per defecte
		port = DEFAULT_PORT;
		printf("Cap port indicat. Utilitzant valor per defecte: %d\n", port);
	}
	else if (argc == 2) {
		errno = 0;
		port = strtol(argv[1], &endptr, 10);

		// Comprovacions
		if (errno != 0 || *endptr != '\0' || port < 1 || port > 65535) {
			fprintf(stderr, "Error: el port ha de ser un enter entre 1 i 65535.\n");
			exit(EXIT_FAILURE);
		}
	}
	else {
		fprintf(stderr, "Ús: %s [port]\n", argv[0]);
		exit(EXIT_FAILURE);
	}


	// Crear el socket
	// Afegiu comentari explicant els arguments
	
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error en crear el socket");
		exit(EXIT_FAILURE);
	}
 // AF_INET: es el domini, que utilitza direccions IPv4.
 // SOCK_STREAM: indica el tipus del socket; en aquest cas, un socket de flux (TCP).
 // 0: Indica el protocol; 0 selecciona automáticament el protocol predeterminat adecuat. 

 

	// Afegiu comentari explicant què són aquests 3 paràmetres,
	// per què s'utilitzen htons() i htonl(),
	// i per què s'utilitzen els valors que hi ha
	memset(&address, 0, sizeof(address)); // Initzialitza tota l'estructura
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);	// Per a INADDR_ANY no faria falta htonl(), però li posem per coherència
	address.sin_port = htons(port);

	//address.sin_family = AF_INET; indica la familia de direccions que utilitzare, AF_INET significa IPv4.
	//address.sin_addr.s_addr = htonl(INADDR_ANY);indica en que dirrecció IP local escoltarà el servidor. 
	//INADDR_ANY significa que acceptará connexions dirigides a cualsevol de les direccions IPv4 locals de la màquina. 
	//address.sin_port = htons(port); indica el port on s'escoltarà el servidor. 

	// htonl -> converteix un valor de 32 bits, com una direcció IPv4 expresada en enter, a l'ordre de bytes de xarxa (big-endian).
	// htons -> converteix un valor de 16 bits, com el port, a l'ordre de bytes de xarxa (big-endian). 
	//Això és necessari perquè els protocols de xarxa utilitzen l'ordre de bytes de xarxa, que pot diferir de l'ordre de bytes del sistema local.

	// Enllaçar el socket al port especificat
	// Afegiu comentari explicant els arguments
	if (bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("Error en fer el bind");
		close(sock);
		exit(EXIT_FAILURE);
	}

	// sock: el descriptor del socket que hem creat.
	//(struct sockaddr *)&address: la direcció local que volem associar.
	// sizeof(address): la mida de l'estructura que conté la direcció. 

	// Escoltar connexions entrants
	// Afegiu comentari explicant els arguments
	
	if (listen(sock, 3) < 0) {
		perror("Error en escoltar");
		close(sock);
		exit(EXIT_FAILURE);
	}

	//sock: es el descriptor del socket del servidor ue volem possar en mode de escolta. 
	// 3: es el nombre màxim de connexions pendents que poden estar en cua abans de que el servidor comenci a rebutjar noves connexions. 


	printf("Servidor en funcionament, esperant connexions...\n");

	while (1) {
		// Acceptar connexions de clients
		// Afegiu comentari explicant els arguments i per què hi ha i cal new_socket si ja tenim sock
		
		if ((new_socket = accept(sock, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
			perror("Error en acceptar la connexió");
			close(sock);
			exit(EXIT_FAILURE);
		}

		// sock: Socket principal que está escuchando. 
		// (struct sockaddr*)&address: Estructura on es guarden les dades de la direcció del client que es conecta. 
		//addrlen: Direcció de la variable que indica el tamany de l'estructura; accept() pot actualitzarla. 

		// Sock espera conexions entrars, en canvi new_sockt s'utilitza per enviar y escriure misatges al client. 



		 while (1) {
			memset(buffer, 0, BUFFER_SIZE);
			
			// Llegir missatge del client
			if (recv(new_socket, buffer, BUFFER_SIZE, 0) <= 0) {
				printf("Client desconnectat (PID: %d)\n", getpid());
				break;
			}

			// Aquí haureu d'implementar l'anàlisi de la cadena rebuda per saber l'operació,
			// i si n'hi ha els arguments, executar-la i tornar el(s) resultat(s)
			// A continuació hi ha el codi corresponent a l'opció d'Enviar missatge, amb el retorn d'una cadena fixa,
			// i la de Sortida de la connexió/bucle si és el cas.
			// Modifiqueu la cadena de retorn per tal que sigui la que diu l'enunciat.
			
			// Comprovar si el client vol tancar la connexió
			if (strcmp(buffer, "EXIT") == 0) {
				printf("Tancant connexió amb el client (PID: %d)...\n", getpid());

				send(new_socket, "Connexió tancada\n", strlen("Connexió tancada\n"), 0);
				break;	// Sortir del bucle
			} 
			
			else if(strncmp(buffer, "MSG|", 4) == 0){
				char resposta [BUFFER_SIZE];

				snprintf(resposta, sizeof(resposta),"mistge rebut: %s", buffer +4);
				send(new_socket, resposta, strlen(resposta), 0);
			}
			else if (strncmp(buffer, "SEARCH|", 7) == 0){
				char resposta [BUFFER_SIZE];
				char *endptr;
				long id;

				//obtenir l'identificador del perfum. 
				errno = 0;
				id = strtol(buffer + 7, &endptr, 10);

				if(errno != 0 || endptr == buffer +7 || *endptr != '\0'){
					snprintf(resposta, sizeof(resposta), "Error: identificador de perfum no valid.");
				} else {
					int trobat = 0;

					for(size_t i = 0; i < NUM_PERFUMS; i++){
						if(catalog[i].id == id){
							snprintf(resposta, sizeof(resposta), "Perfum: %s | Preu: %.2f €", catalog[i].nom, catalog[i].preu);
							trobat = 1;
							break;
						}
					}
					if (!trobat){
						snprintf(resposta, sizeof(resposta), "No existeix cap perfum amb aquest ID.");
					}
				}

				send(new_socket, resposta, strlen(resposta), 0);
			}
			else if (strncmp (buffer, "ADD|", 4) == 0)
			{
				char resposta[BUFFER_SIZE];
				char *endptr;
				long id;

				//obtenir id perfum
				errno = 0;
				id = strtol(buffer +4, &endptr, 10);

				if (errno != 0 || endptr == buffer +4 || *endptr != '\0')
				{
					snprintf(resposta, sizeof(resposta), "Error: identificador de perfum no valid.");

				} else{
					int trobat = 0;

					//cercar el perfum al catàleg. 
					for (size_t i=0; i< NUM_PERFUMS; i++)
					{
						if(catalog[i].id == id)
						{
							cistella[i].id_perfum = catalog[i].id;
							cistella[i].unitats++;

							snprintf(resposta, sizeof(resposta), "Afegit a la cistella: %s", catalog[i].nom);

							trobat = 1;
							break;
						}
					}

					if(!trobat)
					{
						snprintf(resposta, sizeof(resposta), "No existeix cap perfm amb aquest ID.");
					}
				}
				if (send(new_socket, resposta, strlen(resposta), 0) < 0)
				{
					perror("Error en enviar la resposta");
					break;
				}
			}
			else if(strcmp(buffer, "TOTAL") == 0)
			{
				char resposta[BUFFER_SIZE];
				float total = 0.0;

				for(size_t i =0; i < NUM_PERFUMS; i++)
				{
					total += cistella[i].unitats * catalog[i].preu;
				}
				printf("Total calculat: %.2f\n", total);

				snprintf(resposta, sizeof(resposta), "Total de la cistella: %.2f €", total);
				printf("Enviant resposta: %s\n", resposta);

				printf("Primer byte de resposta: %d\n", (unsigned char)resposta[0]);

				ssize_t bytes_enviats = send(new_socket, resposta, strlen(resposta), 0);
				printf("Bytes enviats: %zd\n", bytes_enviats);
				
				if(bytes_enviats < 0)
				{
					perror("Error en enviar el total");
					break;
				}
				

			}
			else if(strcmp(buffer, "CLEAR") == 0)
			{
				char resposta[BUFFER_SIZE];

				for (size_t i=0; i < NUM_PERFUMS; i++)
				{
					cistella[i].unitats = 0;
				}
				
				snprintf(resposta, sizeof(resposta), "La cistella s'ha buidat correctament.");

				if(send(new_socket, resposta, strlen(resposta), 0) < 0)
				{
					perror("Error en enviar la resposta");
					break;
				}
			} 
			
			else {
				send(new_socket, "Missatge rebut\n", strlen("Missatge rebut\n"), 0);
			}
		}

		close(new_socket); // Tancar la connexió amb el client
	}

	close(sock);
	return 0;
}
