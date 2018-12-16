//Farina Angelo 0000788991 esercitazione 03
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define LINE_LEGHT 128

int main(int argc, char **argv){

	struct hostent *host;
	struct sockaddr_in clientaddr, servaddr;
	int port, checkPort, sd, len = 0, wordcount = 0;
	char nomeFile[LINE_LEGHT];

	//------------- CONTROLLO ARGOMENTI -----------------------
	//--------- usage: client serverAddress serverPort --------
	if(argc != 3){
		printf("usage: %s serverAddress serverPort",argv[0]);
		exit(1);
	}

	//------------- INIZIALIZZAZIONE INDIRIZZO CLIENT ----------
	memset((char *) &clientaddr, 0, sizeof(struct sockaddr_in));
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_addr.s_addr = INADDR_ANY;
	clientaddr.sin_port = 0;

	//------------- INIZIALIZZAZIONE INDIRIZZO SERVER -----------
	memset((char *) &servaddr, 0, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	host = gethostbyname(argv[1]);

	//---------------- CONTROLLO PARAMETRI SERVER ---------------
	checkPort = 0;
	while(argv[2][checkPort] != '\0'){
		if(argv[2][checkPort] < '0' || argv[2][checkPort]> '9'){
			printf("Secondo argomento non intero: %s\n",argv[2]);
			printf("usage: %s serverAddress serverPort",argv[0]);
			exit(2);
		}
		checkPort++;
	}
	port = atoi(argv[2]);
	if(port < 1024 || port > 65535){
		printf("Porta %d scorretta...\n",port);
		exit(2);
	}
	if(host == NULL){
		printf("%s not found in /etc/hosts\n", argv[1]);
		exit(2);
	}
	else{
		servaddr.sin_addr.s_addr = ((struct in_addr *) (host->h_addr))->s_addr;
		servaddr.sin_port = htons(port);
	}

	//----------------------- CREAZIONE SOCKET ------------------------
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0){
		perror("apertura socket");
		exit(1);
	}
	printf("Client: socket creata, sd=%d\n",sd);

	//----------------------- BIND SOCKET ---------------------------
	if(bind(sd, (struct sockaddr *) &clientaddr, sizeof(clientaddr)) < 0){
		perror("bind socket");
		exit(0);
	}
	printf("Client: bind socket alla porta: %i\n", clientaddr.sin_port);


	//-------------- LOGICA PROGRAMMA, FILTRO BEN FATTO -------------
	printf("Dammi il nome di file, EOF per terminare: \n");
	while(gets(nomeFile)!=EOF){
		printf("Nome file: %s",nomeFile);
		len = sizeof(servaddr);

		if(sendto(sd, &nomeFile, strlen(nomeFile)+1, 0, (struct sockaddr *) &servaddr, len) < 0){
			perror("sendto");
			printf("Dammi il nome di file, EOF per terminare: \n");
			continue;
		}

		//----------------- RICEZIONE RISULTATO ----------------
		printf("Attesa del risultato...\n");
		if(recvfrom(sd,&wordcount,sizeof(wordcount),0,(struct sockaddr *) &servaddr,&len)){
			perror("recvfrom");
			continue;
		}
		if(wordcount < 0)
			printf("File non esistente...\n");
		else
			printf("Esito dell'operazione, numero caratteri: %d\n", wordcount);
		printf("Dammi il nome di file, EOF per terminare: \n");
	}
	close(sd);
	printf("Client: termino...\n");
	exit(0);
}
