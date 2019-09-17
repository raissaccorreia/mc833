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
   FILE *log;

   /* cricao do socket, caso seja sucedido*/
   if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("socket");
      exit(1);
   }

   /* inserindo as informacoes npid_t pid;o socket */
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family      = AF_INET;
   servaddr.sin_addr.s_addr = htonl(INADDR_ANY); /*sistema escolhe endereço */
   servaddr.sin_port        = htons(8000);   /* deixar 0 para sistema escolher porta */

   //Associa socket criado a porta local do sistema operacional.
   //E verificado se a porta já não está sendo utilizada
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

   for ( ; ; ) {

      /*obter dados e retirar da fila a 1ª conexao da fila de conexoes concluidas */
      if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) {
         perror("accept");
         exit(1);
      }

      if( (pid = fork()) == 0) {

         close(listenfd); /* child closes listening socket */
         ticks = time(NULL);
         snprintf(buf, sizeof(buf), "%.24s", ctime(&ticks));

         log = fopen("log.txt", "a");
         if(log == NULL)
            printf("Erro, nao foi possivel abrir o arquivo\n");
         else
            fprintf(log, "%s (IP = %s, Porta = %d) CONECTOU\n", buf, getIp(clientaddr, networkedIp), 8000);
         
         while(fgets(buf, sizeof(buf), stdin)){
            buf[MAXLINE-1] = '\0';
            int len = strlen(buf)+1;
            //encerramento do servidor
            if(strcmp(buf, "quit\n") == 0)
               exit(1);

            send(connfd, buf, len, 0);

            //encerra o cliente
            if(strcmp(buf, "exit\n") == 0){
               ticks = time(NULL);
               snprintf(buf, sizeof(buf), "%.24s", ctime(&ticks));
               fprintf(log, "%s (IP = %s, Porta = %d) DESCONECTOU\n", buf, getIp(clientaddr, networkedIp), 8000);
               recv(connfd, buf, sizeof(buf), 0);
               snprintf(buffer, sizeof(buf), "(IP = %s, Porta = %d) %s\n", getIp(clientaddr, networkedIp), 8000, buf);
               fputs(buffer, stdout);
               close(connfd); /* done with this client */
               exit(0);
            }
            recv(connfd, buf, sizeof(buf), 0);
            ticks = time(NULL);
            snprintf(buffer, sizeof(buffer), "%.24s", ctime(&ticks));
            printf("Cliente (IP = %s, Porta = %d): ", getIp(clientaddr, networkedIp), 8000);
            fprintf(log, "%s (IP = %s, Porta = %d), Comando: %s\n", buffer, getIp(clientaddr, networkedIp), 8000, buf);
            fputs(buf, stdout);
         }
         ticks = time(NULL);
         snprintf(buf, sizeof(buf), "%.24s", ctime(&ticks));
         fprintf(log, "%s (IP = %s, Porta = %d) DESCONECTOU\n", buf, getIp(clientaddr, networkedIp), 8000);
         fclose(log);
         close(connfd); /* done with this client */
         exit(0);
      }
      close(connfd);
   }
   return(0);
}

/* EXERCICIO 6 */
      // socklen_t len = sizeof(servaddr);
      // getpeername(listenfd, (struct sockaddr*)&servaddr, &len);      
      /*char ipstr[INET6_ADDRSTRLEN];
      bzero(ipstr, 46);
      inet_ntop(AF_INET, &servaddr.sin_addr, ipstr, sizeof(ipstr));
      printf("Connection established successfully with %s:%i!\n", ipstr, ntohs(servaddr.sin_port));
      //recebe caracter e envia de volta ao cliente
      char buffer[1024];
      recv(connfd,buffer, 1024,0);
      send(connfd,buffer,1024,0);
      printf("%s\n", buffer);

      ticks = time(NULL);
      snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
      write(connfd, buf, strlen(buf));
      close(connfd); */