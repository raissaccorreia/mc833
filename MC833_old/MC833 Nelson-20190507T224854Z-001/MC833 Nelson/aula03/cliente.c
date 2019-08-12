#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLINE 4096

int main(int argc, char **argv) {
      int    sockfd, n;
      char   recvline[MAXLINE + 1];
      char   error[MAXLINE + 1];
      struct sockaddr_in servaddr, my_addr;
      char myIP[16];
      unsigned int myPort;
//Garantir que  sejam apenas 2 argumentos no main IP e Porta
      if (argc != 2) {
          strcpy(error,"uso: ");
          strcat(error,argv[0]);
          strcat(error," <IPaddress>");
          perror(error);
          exit(1);
      }
//Criacao do socket
      sockfd = socket(AF_INET, SOCK_STREAM, 0);

//Inserindo as informacoes no socket
      bzero(&servaddr, sizeof(servaddr));
      servaddr.sin_family = AF_INET;
      servaddr.sin_port   = htons(8000);

//Realizando a conexao com o servidor
      inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
      connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

// Obter meu endereço e porta
	    bzero(&my_addr, sizeof(my_addr));
	    socklen_t len = sizeof(my_addr);
    	getsockname(sockfd, (struct sockaddr *) &my_addr, &len);
    	inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));
    	myPort = ntohs(my_addr.sin_port);

//Saida
	   printf("Local ip address: %s\n", myIP);
     printf("Local port : %u\n", myPort);
//Responsavel pelo envio de um caractere, ou o buffer todo
     char buffer[1024];
     strcpy(buffer,"H");
     send(sockfd,buffer,13,0);
//recendo o caractere de volta do servidor e o imprime
     recv(sockfd, buffer, 1024, 0);
     printf("%s\n", buffer);
//Imprime a data ou o que o servidor enviar
     while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
         recvline[n] = 0;
         if (fputs(recvline, stdout) == EOF) {
             exit(1);
         }
     }
     if (n < 0) {
        exit(1);
    }
    exit(0);
}
