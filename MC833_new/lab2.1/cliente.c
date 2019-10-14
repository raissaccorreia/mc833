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
   char   myIP[16];
   unsigned int myPort;
   struct sockaddr_in servaddr;

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
   /* */
   bzero(&servaddr, sizeof(servaddr));

   socklen_t len = sizeof(servaddr);
   int sname = getsockname(sockfd, (struct sockaddr *) &servaddr, &len);

   servaddr.sin_family = AF_INET;
   servaddr.sin_port   = htons(8000);
   if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr ) <= 0) {
      perror("inet_pton error");
      exit(1);
   }
   myPort = ntohs(servaddr.sin_port);
   // printf("Local ip adress: %d\n", sname);
   printf("Peer IP adress: %d\n", inet_ntoa(servaddr.sin_addr));
   printf("Local port: %u\n", servaddr.sin_addr);

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
   }

   if (n < 0) {
      perror("read error");
      exit(1);
   }

   exit(0);
}
