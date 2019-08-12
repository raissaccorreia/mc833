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

int main (int argc, char **argv) {
      int    listenfd, connfd;
      struct sockaddr_in servaddr, addr;
      char   buf[MAXDATASIZE];
      time_t ticks;

//Criacao do socket
      if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
           perror("socket");
           exit(1);
      }

//inserindo informacoes no socket
      bzero(&servaddr, sizeof(servaddr));
      servaddr.sin_family      = AF_INET;
      servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
      servaddr.sin_port        = htons(8000);

//Associa socket criado a porta local do sistema operacional.
//E verificado se a porta já não está sendo utilizada
      if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
          perror("bind");
          exit(1);
      }

//Esperando conexao
      if (listen(listenfd, LISTENQ) == -1) {
          perror("listen");
          exit(1);
      }

//aceitando as conexoes enquanto vierem
      for ( ; ; ) {
          if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) {
               perror("accept");
               exit(1);
          }
//imprime o IP e a porta do cliente
          socklen_t len = sizeof(addr);
          getpeername(listenfd, (struct sockaddr*)&addr, &len);
          printf("Peer IP address: %s\n", inet_ntoa(addr.sin_addr));
          printf("Peer port      : %d\n", ntohs(addr.sin_port));
//recebe um caractere e envia de volta ao cliete
          char buffer[1024];
          recv(connfd, buffer, 1024, 0);
          send(connfd,buffer,1024,0);
          printf("%s\n", buffer);
//envia a data para o cliente
          ticks = time(NULL);
          snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
          write(connfd, buf, strlen(buf));

          close(connfd);
      }
   return(0);
}
