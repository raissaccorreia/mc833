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
#include<wait.h> 
#include<signal.h>

// #define LISTENQ 10
#define MAXDATASIZE 100
#define MAXLINE 4096

int main (int argc, char **argv) {
   int    listenfd, connfd, backlogSize, stat; /*adicionado para lab3 item 2 */
   struct sockaddr_in servaddr;
   char   buf[MAXDATASIZE];
   char   entrada[MAXLINE + 1];
   pid_t pid;
   time_t ticks;
   FILE *log;

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
   scanf("%d", &backlogSize); /*adicionado para lab3 item 2 */
   if (listen(listenfd, backlogSize) == -1) {
      perror("listen");
      exit(1);
   }

   char ipstr[INET6_ADDRSTRLEN];
   bzero(ipstr, 46);
   inet_ntop(AF_INET, &servaddr.sin_addr, ipstr, sizeof(ipstr));
   printf("Connection established successfully with %s:%i!\n", ipstr, ntohs(servaddr.sin_port));
   sleep(10);
   clock_t t;
   for ( ; ; ) {
      /*obter dados e retirar da fila a 1ª conexao da fila de conexoes concluidas */
      if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) {
         perror("accept");
         exit(1);
      }
      if((pid = fork()) == 0) {
         while(1){            
            ticks = time(NULL);
            snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
            write(connfd, buf, strlen(buf));
            
            //recebendo do cliente
            recv(connfd, entrada, sizeof(entrada), 0);            
            printf("%s\n", entrada);
            t = clock();

            //escrevendo no log a conexao e a respectiva entrada, limpar a cada execucao
            //a ordem é do mais recente para o mais antigo
            log = fopen("log.txt", "a");
            if(log == NULL)
               printf("Erro, nao foi possivel abrir o arquivo\n");
            else{
               fprintf(log, "%s (IP = %s, Porta = %d) CONECTOU\n", entrada, ipstr, 8000);
            }

            //caso de encerramento da conexao a pedido do cliente
            if(strcmp(entrada, "quit") == 0){
               printf("encerrando conexão com este cliente!\n");
               fprintf(log, "%s (IP = %s, Porta = %d) DESCONECTOU\n", buf, ipstr, 8000);                 
               close(connfd);
               exit(0);
            }

            //enviando de volta ao cliente
            send(connfd, entrada, sizeof(entrada), 0);

            //executando comando enviado pelo cliente
            system(entrada);
         }
         fclose(log);
         sleep(10); /*adicionado para lab3 item 2 */
         close(connfd); /* done with this client */
         if((clock() - t)/CLOCK_PER_SEC > 10){            
            /*matando processo zumbi */
            kill(pid, SIGINT); 
            wait(&stat); 
            if (WIFSIGNALED(stat))
               psignal(WTERMSIG(stat), "Child term due to");
            }
         exit(0);
      }
      close(connfd);     
   }
   return(0);
}
