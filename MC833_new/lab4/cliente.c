#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "toolkit.h"

#define MAXLINE 4096

int max(int a, int b){
  if(a > b)
    return a;

  return b;
}

int main(int argc, char **argv) {
   int    sockfd, n;
   char * ip;
   unsigned int port;
   char   recvline[MAXLINE + 1];
   char   message[MAXLINE + 1];
   char   error[MAXLINE + 1];
   struct sockaddr_in servaddr;
   fd_set rset ;

/*confirmando numero de parametros passados, apenas IP do servidor*/
   if (argc != 3) {
      strcpy(error,"uso: ");
      strcat(error,argv[0]);
      strcat(error," <IPaddress>");
      perror(error);
      exit(1);
   }
   /*criacao do socket e checagem de sua possibilidade*/
   sockfd = Socket(AF_INET, SOCK_STREAM, 0);

   /*atribuição de porta do servidor ao socket criado*/
   ip = argv[1];
   port = atoi(argv[2]);
   servaddr = ClientSockaddrIn(AF_INET, ip, port);

   /*cliente se conecta com o servidor remeto, 0 ok, -1 erro */
   Connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

   /* enquanto o servidor enviar linha nao vazia realiza a leitra, se nao EOF */
   FD_ZERO(&rset);  
   for ( ; ; ) {        
     FD_SET(fileno(stdin), &rset);
     FD_SET(sockfd, &rset);
     int maxfdp1 = max(fileno(stdin), sockfd) + 1;
     select(maxfdp1, &rset, NULL, NULL, NULL);

     //pegando recvline do arquivo
     if (FD_ISSET(sockfd, &rset)){       
       if (fgets(recvline, MAXLINE, stdin) == NULL){
          exit(0);
        }
     }     
    //escrevendo mensagem para o servidor
     if (FD_ISSET(fileno(stdin), &rset)){
       if(fgets(message, MAXLINE, stdin) != NULL){
         write(sockfd, message, strlen(message));
       } else if(feof(stdin)){
         shutdown(sockfd, SHUT_WR);
         return 0;
       }
       fputs(message, stdout);
     }     
   }
   //para o caso de um erro de leitura
   if (n < 0) {
      perror("read error");
      exit(1);
   }
   exit(0);
}