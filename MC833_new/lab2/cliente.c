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
#include <time.h>

#define MAXLINE 4096
#define MAXDATASIZE 100

int main(int argc, char **argv) {
   int    sockfd;
   char   recvline[MAXLINE + 1];
   char   error[MAXLINE + 1];
   char   result;
   struct sockaddr_in servaddr;
   socklen_t len = sizeof(servaddr);
   char   buf[MAXDATASIZE];

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
   getsockname(sockfd, (struct sockaddr *) &servaddr, &len);
   printf("Server IP adress: %s\n", inet_ntoa(servaddr.sin_addr));
   printf("Server port: %u\n", servaddr.sin_port);

   /* enquanto o servidor enviar linha nao vazia realiza a leitra, se nao EOF */
   while ((len = recv(sockfd, buf, sizeof(buf), 0))) {     
      
      if (fputs(recvline, stdout) == EOF) {
         perror("fputs error");
         exit(1);
      }

      if(strcmp(buf, "exit\n") == 0){
          time_t ticks = time(NULL);
          snprintf(buf, sizeof(buf), "%.24s\n", ctime(&ticks));
          fputs(buf, stdout);
          send(sockfd, "Cliente encerrado.", sizeof(buf), 0);
          exit(0);
        }
        //execl("/bin", buf, 0, 0);
        if((result = system(buf)) == -1) {
              perror("system error");
        }
        send(sockfd, buf, sizeof(buf), 0);
      }     
   exit(0);
}
