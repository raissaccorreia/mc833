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
      int    sockfd, len;
      char   error[MAXLINE + 1];
      struct sockaddr_in servaddr;
      char myIP[16], *ip, *result;
      unsigned int port;
      char   buf[MAXDATASIZE];

//Garantir que  sejam apenas 2 argumentos no main IP e Porta
      if (argc != 3) {
          strcpy(error,"uso: ");
          strcat(error,argv[0]);
          strcat(error," <IPaddress>");
          perror(error);
          exit(1);
      }
//Criacao do socket
      sockfd = socket(AF_INET, SOCK_STREAM, 0);

//Inserindo as informacoes no socket
      port = atoi(argv[2]);
      bzero(&servaddr, sizeof(servaddr));
      servaddr.sin_family = AF_INET;
      servaddr.sin_port   = htons(port);

//Realizando a conexao com o servidor
      inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
      connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

      inet_ntop(AF_INET, &(servaddr.sin_addr), myIP, INET6_ADDRSTRLEN);
      ip = inet_ntoa(servaddr.sin_addr);

      printf("Dados do servidor que me conectei:\n");
      printf("\tEndereco IP:%s\n", ip);
      printf("\tPorta:%d\n\n", port);

      while((len = recv(sockfd, buf, sizeof(buf), 0))){
        fputs(buf, stdout);
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
