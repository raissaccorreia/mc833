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
#include "toolkit.h"

/* tamanho da fila de espera de clientes*/
#define LISTENQ 10
#define MAXDATASIZE 100
#define MAXLINE 4096

int main (int argc, char **argv) {
   int    listenfd, connfd;
   int port;
   struct sockaddr_in servaddr;
   char   buf[MAXDATASIZE];
   char   entrada[MAXLINE + 1];
   pid_t pid;

   /* checagem se o socket pode ser criado e criacao dele*/
   listenfd = Socket(AF_INET, SOCK_STREAM, 0);

   /* criando socket */
   port = atoi(argv[1]);
   servaddr = ServerSockaddrIn(AF_INET, INADDR_ANY, port);

   /* informar que é um servidor atraves do bind com o socket, -1 erro, 0 OK*/
   Bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

   /*esperar conexoes, nao tem backlog que no caso é o tamanho da fila no kernel
   para conexoes em andamento e completadas backlog default = 10*/
   Listen(listenfd, 10);

   for ( ; ; ) {
      socklen_t servaddr_len = sizeof(servaddr);
      connfd = Accept(listenfd, (struct sockaddr *) &servaddr,  &servaddr_len);

      /*criar processo filho para receber cada solicitacao de cliente*/
      if((pid = fork()) == 0) {
         while(1){
            write(connfd, buf, strlen(buf));
            //recebendo do cliente
            recv(connfd, entrada, sizeof(entrada), 0);
            //imprindo o que recebeu do cliente
            printf("%s\n", entrada);
            //caso de encerramento da conexao a pedido do cliente
            if(strcmp(entrada, "quit") == 0){
               close(connfd);
               exit(0);
            }
            //enviando de volta ao cliente
            send(connfd, entrada, sizeof(entrada), 0);
         }
         // done with this client
         close(connfd);
         exit(0);
      }
      close(connfd);
   }
   return(0);
}

/* Parte desativada: usada para imprimir IP e porta do servidor nas atividades anteriores
   char ipstr[INET6_ADDRSTRLEN];
   bzero(ipstr, 46);
   inet_ntop(AF_INET, &servaddr.sin_addr, ipstr, sizeof(ipstr));
   printf("Connection established successfully with %s:%i!\n", ipstr, ntohs(servaddr.sin_port)); */

   //escrevendo no log a conexao e a respectiva entrada, limpar a cada execucao
            //a ordem é do mais recente para o mais antigo
            /*log = fopen("log.txt", "a");
            if(log == NULL)
               printf("Erro, nao foi possivel abrir o arquivo\n");
            else{
               fprintf(log, "%s (IP = %s, Porta = %d) CONECTOU\n", entrada, ipstr, 8000);
            } */

/* desativado impressao de encerramento de conexão
               printf("encerrando conexão com este cliente!\n");
               fprintf(log, "%s (IP = %s, Porta = %d) DESCONECTOU\n", buf, ipstr, 8000); */

               //ticks = time(NULL);
            //snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));


   //time_t ticks;