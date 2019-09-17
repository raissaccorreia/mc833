#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define LISTENQ 10
#define MAXDATASIZE 100
#define MAXLINE 4096

int main (int argc, char **argv) {
   int    listenfd, connfd;
   struct sockaddr_in servaddr;
   char   buf[MAXDATASIZE];
   char   entrada[MAXLINE + 1];
   pid_t pid;
   time_t ticks;

   /* checagem se o socket pode ser criado */
   if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("socket");
      exit(1);
   }

   /* criando socket */
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family      = AF_INET;
   servaddr.sin_addr.s_addr = htonl(INADDR_ANY); /*sistema escolhe endereço */
   servaddr.sin_port        = htons(8000);   /* deixar 0 para sistema escolher porta */

   /* informar que é um servidor, -1 erro, 0 OK*/
   if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
      perror("bind");
      exit(1);
   }

   /*esperar conexoes, nao tem backlog que no caso é o tamanho da fila no kernel
   para conexoes em andamento e completadas*/
   if (listen(listenfd, LISTENQ) == -1) {
      perror("listen");
      exit(1);
   }

   char ipstr[INET6_ADDRSTRLEN];
   bzero(ipstr, 46);
   inet_ntop(AF_INET, &servaddr.sin_addr, ipstr, sizeof(ipstr));
   printf("Connection established successfully with %s:%i!\n", ipstr, ntohs(servaddr.sin_port));

   for ( ; ; ) {
      /*obter dados e retirar da fila a 1ª conexao da fila de conexoes concluidas */
      if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) {
         perror("accept");
         exit(1);
      }
      if( (pid = fork()) == 0) {
         ticks = time(NULL);
         snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
         write(connfd, buf, strlen(buf));
         //recebendo do cliente
         recv(connfd, entrada, sizeof(entrada), 0);
         printf("%s\n", entrada);
         //enviando de volta ao cliente
         send(connfd, entrada, sizeof(entrada), 0);
         //executando comando enviado pelo cliente
         system(entrada);
         close(connfd); /* done with this client */
         exit(0);
      }
      close(connfd);
   }
   return(0);
}
