#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>

#include "basic.h"
#include "socket_helper.h"

#define MAXLINE 4096
#define EXIT_COMMAND "exit\n"

int max(int a, int b){
  if(a > b)
    return a;

  return b;
}

void doit(int sockfd);

int main(int argc, char **argv) {
   int    port, sockfd;
   char * ip;
   char   error[MAXLINE + 1];
   struct sockaddr_in servaddr;

   if (argc != 3) {
      strcpy(error,"uso: ");
      strcat(error,argv[0]);
      strcat(error," <IPaddress, Port>");
      perror(error);
      exit(1);
   }


   ip = argv[1];
   port = atoi(argv[2]);

   sockfd = Socket(AF_INET, SOCK_STREAM, 0);

   servaddr = ClientSockaddrIn(AF_INET, ip, port);

   Connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

   doit(sockfd);

   exit(0);
}

void doit(int sockfd) {
   char   message[MAXLINE + 1];
   char   response[MAXLINE + 1];
   fd_set rset;

   while (1) {
     FD_ZERO(&rset);
     FD_SET(fileno(stdin), &rset);
     FD_SET(sockfd, &rset);
     int maxfdp1 = max(fileno(stdin), sockfd) + 1;
     select(maxfdp1, &rset, NULL, NULL, NULL);

     if (FD_ISSET(sockfd, &rset)){
       if (fgets(message, MAXLINE, stdin) == NULL){
          printf("str_cli: server terminated prematurely");
          exit(0);
        }
     }

     if (FD_ISSET(fileno(stdin), &rset)){
       if(fgets(response, MAXLINE, stdin) != NULL){
         write(sockfd, response, strlen(response));
       } else if(feof(stdin)){
         shutdown(sockfd, SHUT_WR);
         return;
       }

       fputs(response, stdout);
     }
   }
 }
