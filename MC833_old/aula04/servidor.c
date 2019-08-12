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

char* getIp(struct sockaddr_in clientaddr, char *networkedIp){
      inet_ntop(AF_INET, &(clientaddr.sin_addr), networkedIp, INET6_ADDRSTRLEN);
      char *ip = inet_ntoa(clientaddr.sin_addr);

      return ip;
}

int main (int argc, char **argv) {
      int    listenfd, connfd;
      struct sockaddr_in servaddr, clientaddr;
      char   buf[MAXDATASIZE], buffer[MAXDATASIZE];
      time_t ticks;
      pid_t pid;
      char networkedIp[16];
      int port;
      FILE *log;

//Criacao do socket
      if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
           perror("socket");
           exit(1);
      }

//inserindo informacoes no socket
      port = atoi(argv[1]);
      bzero(&servaddr, sizeof(servaddr));
      servaddr.sin_family      = AF_INET;
      servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
      servaddr.sin_port        = htons(port);

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
          if( (pid = fork()) == 0) {

              printf("Dados do cliente que conectou:\n");
              printf("\tEndereco IP:%s\n", getIp(clientaddr, networkedIp));
              printf("\tPorta:%d\n\n", port);

              printf("Dados locais:\n");
              printf("\tEndereco IP:%s\n", getIp(servaddr, networkedIp));
              printf("\tPorta:%d\n\n", port);

              close(listenfd); /* child closes listening socket */
              ticks = time(NULL);
              snprintf(buf, sizeof(buf), "%.24s", ctime(&ticks));

              log = fopen("log.txt", "a");
            	if(log == NULL)
            		printf("Erro, nao foi possivel abrir o arquivo\n");
            	else{
            		fprintf(log, "%s (IP = %s, Porta = %d) CONECTOU\n", buf, getIp(clientaddr, networkedIp), port);
            	}

              while(fgets(buf, sizeof(buf), stdin)){
                  buf[MAXLINE-1] = '\0';
                  int len = strlen(buf)+1;

                  //encerra o servidor
                  if(strcmp(buf, "quit\n") == 0){
                    //TODO
                  }
                  send(connfd, buf, len, 0);

                  //encerra o cliente
                  if(strcmp(buf, "exit\n") == 0){
                    ticks = time(NULL);
                    snprintf(buf, sizeof(buf), "%.24s", ctime(&ticks));
                    fprintf(log, "%s (IP = %s, Porta = %d) DESCONECTOU\n", buf, getIp(clientaddr, networkedIp), port);
                    recv(connfd, buf, sizeof(buf), 0);
                    snprintf(buffer, sizeof(buf), "(IP = %s, Porta = %d) %s\n", getIp(clientaddr, networkedIp), port, buf);
                    fputs(buffer, stdout);
                    close(connfd); /* done with this client */
                    exit(0);
                  }
                  recv(connfd, buf, sizeof(buf), 0);
                  ticks = time(NULL);
                  snprintf(buffer, sizeof(buffer), "%.24s", ctime(&ticks));
                  printf("Cliente (IP = %s, Porta = %d): ", getIp(clientaddr, networkedIp), port);
                  fprintf(log, "%s (IP = %s, Porta = %d), Comando: %s\n", buffer, getIp(clientaddr, networkedIp), port, buf);
                  fputs(buf, stdout);
              }
              ticks = time(NULL);
              snprintf(buf, sizeof(buf), "%.24s", ctime(&ticks));
              fprintf(log, "%s (IP = %s, Porta = %d) DESCONECTOU\n", buf, getIp(clientaddr, networkedIp), port);

              fclose(log);
              close(connfd); /* done with this client */
              exit(0);
          }
          close(connfd);
      }
   return(0);
}
