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
   char   entrada[MAXLINE + 1];
   char   error[MAXLINE + 1];
   struct sockaddr_in servaddr;
   socklen_t len = sizeof(servaddr);

   if (argc != 2) {
      strcpy(error,"uso: ");
      strcat(error,argv[0]);
      strcat(error," <IPaddress>");
      perror(error);
      exit(1);
   }
   /*criacao do socket */
   if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket error");
      exit(1);
   }

   bzero(&servaddr, sizeof(servaddr));     
  
   servaddr.sin_family = AF_INET;
   servaddr.sin_port   = htons(8000);
   if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
      perror("inet_pton error");
      exit(1);
   }   

   /*cliente se conecta com o servidor remeto, 0 ok, -1 erro */
   if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
      perror("connect error");
      exit(1);
   }

   /* enquanto o servidor enviar linha nao vazia realiza a leitra, se nao EOF */
   while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
      recvline[n] = 0;
      if (fputs(recvline, stdout) == EOF) {
         perror("fputs error");
         exit(1);
      }      
      getsockname(sockfd, (struct sockaddr *) &servaddr, &len);
      printf("Peer IP adress: %s\n", inet_ntoa(servaddr.sin_addr));
      printf("Local port: %u\n", servaddr.sin_port);
      //lenda da entrada padrao
      scanf("%s", entrada);
      printf("Input do Clientee: %s\n", entrada);
      //enviando ao servidor
      send(sockfd, entrada, sizeof(entrada), 0);
      //receba do servidor
      recv(sockfd, entrada, sizeof(entrada), 0);
      printf("%s\n", entrada);
   } 
   if (n < 0) {
      perror("read error");
      exit(1);
   }

   exit(0);
}
