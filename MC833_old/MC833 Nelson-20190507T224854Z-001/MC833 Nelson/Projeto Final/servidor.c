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
#define VIDAS_SP 6
#define VIDAS_MP 3

char palavras[MAXLINE][MAXDATASIZE];
int contador = 0;

char* getIp(struct sockaddr_in clientaddr, char *networkedIp){
      inet_ntop(AF_INET, &(clientaddr.sin_addr), networkedIp, INET6_ADDRSTRLEN);
      char *ip = inet_ntoa(clientaddr.sin_addr);

      return ip;
}

// flag = 0: inicializa o dicionario de palvras
// flag = 1: imprime linhas do arquivo passado na tela
int printArquivo(char *nome, int flag){
      int i;
      char linha[MAXDATASIZE];
      FILE *arq;

      arq = fopen(nome, "r");

      if (arq == NULL)  // Se houve erro na abertura
      {
         printf("Problemas na abertura do arquivo\n");
         return 0;
      }
      i = 0;
      while (!feof(arq))
      {
          if (fgets(linha, 100, arq)){  // Se foi possível ler
              if(flag){
                  printf("%s",linha);
              } else {
                  strcpy(palavras[i], linha);
              }
          }
          i++;
      }
      fclose(arq);
      return i-1;
}

// inicializa um vetor
void initVetor(int *v, int max){
      for(int i = 0; i < max; i++){
          v[i] = 0;
      }
}

// verifica se ainda ha palavras nao usadas
int verificaVetor(int *v, int max){
      for(int i = 0; i <= max; i++){
          if(v[i] == 0){
              return 1;  //ainda ha palavras nao usadas
          }
      }
      return 0;  //todas as palavras ja foram usadas
}

// escolhe a proxima palavra a ser jogada
int escolhePalavra(int numPalavras, int *palavrasUsadas){
      int i = rand() % numPalavras;

      //verfica se ainda ha palavras nao usadas
      if(!verificaVetor(palavrasUsadas, numPalavras)){
          return -1;
      }

      // procura uma palavra ainda nao usada
      while(palavrasUsadas[i]){
          i = rand() % numPalavras;
      }

      palavrasUsadas[i] = 1;  //marca palavra como usada
      return i;
}

// imprime o menu do jogo
void menu(){
      printf("\nBem vindo ao jogo da forca!\n");
      printf("-----\n");
      printf("1) Iniciar partida simples\n");
      printf("2) Ser carrasco ao iniciar partida\n");
      printf("3) jogar no modo multiplayer\n");
      printf("-h para ler manual\n\n");

}

// quando o jogador perde todas as vidas
void gameover(int len, int vidas, int atual){
      printf("Forca! Você fez %d tentativas incorretas...\n", vidas);
      printf("A palavra correta era %s, você perdeu!\n\n", palavras[atual]);
}

// verifica se a letra jogada eh valida e se ja foi usada
int procuraAlfabeto(char alfabeto[26], char *letra, int usadas[26]){
      for(int i = 0; i < 26; i++){
          if(alfabeto[i] == *letra){
              if(!usadas[i]){
                  usadas[i] = 1;
                  return 1;   //letra ainda nao usada
              } else {
                  return 2;   //letra ja usada
              }
          }
      }
      return 0;   //letra invalida
}

// verifica se a palavra atual possui a letra jogada
void procuraLetra(int atual, char *letra, int len, int *vidas, int *encontradas){
      int achou = 0;
      for(int i = 0; i < len; i++){
          if(palavras[atual][i] == *letra){
              achou = 1;
              encontradas[i] = 1;
          }
      }
      if(!achou){
          printf("A palavra não tem nenhuma letra '%c'.\n", *letra);
          (*vidas)--;
          printf("Você agora possui %d vidas.\n\n", *vidas);
      } else {
          printf("\nPALAVRA:");
          for(int i = 0; i < len; i++){
              if(encontradas[i]){
                  printf(" %c", palavras[atual][i]);
              } else {
                  printf(" _");
              }
          }
          printf("\n\n");
      }
}

// verifica se o cliente acertou todas as letras da palavra atual
int completouPalavra(int *encontradas, int len){
      for(int i = 0; i < len; i++){
          if(!encontradas[i]){
            return 0;
          }
      }
      return 1;
}

// cliente joga uma partida simples
void partidaSimples(int atual, int connfd, int total_vidas, int *pontos){
      int i, len = strlen(palavras[atual]), usadas[26], vidas = total_vidas, encontradas[len];
      char alfabeto[26] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','X','W','Y','Z'};
      char buf[MAXDATASIZE];

      initVetor(usadas, 26);
      initVetor(encontradas, len);

      printf("Partida simples:\n");
      printf("A partida de jogo da forca começou!\n");
      printf("-----\n");
      printf("Você possui %d vidas.\n", vidas);
      printf("A palavra possui %d caracteres\n", len);

      printf("\nPALAVRA:");
      for(i = 0; i < len-1; i++){
          printf(" _");
      }
      printf(" _\n\n");

      while (1) {
          printf("Digite uma letra ou a palavra:\n\n");
          recv(connfd, buf, sizeof(buf), 0);
          buf[strlen(buf)-1] = '\0';

          // se cliente digitou uma letra
          if(strlen(buf) == 1 || strlen(buf) == 0){
              i = procuraAlfabeto(alfabeto, buf, usadas);

              if(i == 1){
                  procuraLetra(atual, buf, len, &vidas, encontradas);
                  if((vidas) == 0){
                      gameover(len, total_vidas, atual);
                      return;
                  }
                  if(completouPalavra(encontradas, len)){
                      (*pontos)++;
                      printf("Você adivinhou a palavra '%s'! Parabéns!\n", palavras[atual]);
                      return;
                  }
              } else if(i == 2){
                  printf("A letra %s já foi utilizada\n\n", buf);
              } else{
                  printf("Letra inválida\n\n");

              }
          } else { // se cliente digitou uma palavra
              if(strcmp(buf, "-h") == 0){
                  printArquivo("README.txt", 1);
              } else if(strcmp(buf, "exit") == 0){
                  printf("Voce desconectou antes do final da partida!\n");
                  printf("A palavra correta era %s, você perdeu!\n\n", palavras[atual]);
                  close(connfd); /* done with this client */
                  exit(0);
              } else {
                  if(strcmp(palavras[atual], buf) == 0){
                      printf("Você adivinhou a palavra!\n\n");
                      (*pontos)++;
                      return;
                  } else {
                      printf("A palavra correta era %s, você perdeu!\n\n", palavras[atual]);
                      return;
                  }
              }
          }
      }
}

// cliente escolhe ser o carrasco
void serCarrasco(){
      printf("   /\\     /\\\n  {  `---'  }  \n  {  O   O  }  \n~~|~   V   ~|~~ \n   \\  \\|/  /\n    `-----'__\n    /     \\  `^\\_\n   {       }\\ |\\_\\_   W\n   |  \\_/  |/ /  \\_\\_( )\n    \\__/  /(_E     \\__/\n      (  /\n       MM\n");
      printf("Nao seja carrasco com o gatinho fofo =^.^=\n");
}

// cliente joga uma partida multiplayer
void multiplayer(int atual, int connfd, int pontos){
      printf("Aguarde... 10s\n");
      for(int i = 1; i <= 10; i++){
          printf("%d...\n", i);
          sleep(1);
      }
      printf("Esta partida será disputada por %d jodadores.\n\n", contador);

      if(contador == 1){
          printf("Voce esta sozinho... Iniciando partida simples.\n");
          partidaSimples(atual, connfd, VIDAS_SP, &pontos);
      } else {
          partidaSimples(atual, connfd, VIDAS_MP, &pontos);
      }
}

int main (int argc, char **argv) {
      int    listenfd, connfd, numPalavras = 0, atual, pontos, numPartidas;
      struct sockaddr_in servaddr;
      char   buf[MAXDATASIZE];
      pid_t pid;
      int port;

      numPalavras = printArquivo("dicionario.txt", 0);

      //remove o '\n' do final das palavras
      for(int i = 0; i < numPalavras; i++){
          int len = strlen(palavras[i]);
          palavras[i][len -1] = '\0';
      }

      int palavrasUsadas[numPalavras];

      initVetor(palavrasUsadas, numPalavras);

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

      //Associa socket criado a porta local do sistema operacional
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
              close(listenfd); /* child closes listening socket */

              while (1) {
                    menu();

                    recv(connfd, buf, sizeof(buf), 0);

                    pontos = 0;
                    numPartidas = 0;

                    if(strcmp(buf, "1\n") == 0){  //inicia partida simpĺes
                        strcpy(buf, "SIM\n");
                        while(strcmp(buf, "SIM\n") == 0){
                            atual = escolhePalavra(numPalavras, palavrasUsadas);
                            if(atual == -1){
                                printf("Você já jogou todas as palavras. Reiniciando...\n\n");
                                initVetor(palavrasUsadas, numPalavras);
                                atual = escolhePalavra(numPalavras, palavrasUsadas);
                            }
                            partidaSimples(atual, connfd, VIDAS_SP, &pontos);
                            numPartidas++;
                            printf("Voce venceu %d de %d partidas ate agora.\n", pontos, numPartidas);
                            printf("Se desejar jogar outra partida, digite SIM.\n\n");
                            recv(connfd, buf, sizeof(buf), 0);

                            if(strcmp(buf, "exit\n") == 0){
                                close(connfd); /* done with this client */
                                exit(0);
                            }
                        }
                    } else if(strcmp(buf, "2\n") == 0){  //cliente vira o carrasco
                        serCarrasco();
                    } else if(strcmp(buf, "3\n") == 0){  //inicia partida multiplayer
                        strcpy(buf, "SIM\n");
                        while(strcmp(buf, "SIM\n") == 0){
                            atual = escolhePalavra(numPalavras, palavrasUsadas);
                            if(atual == -1){
                                printf("Você já jogou todas as palavras. Reiniciando...\n\n");
                                initVetor(palavrasUsadas, numPalavras);
                                atual = escolhePalavra(numPalavras, palavrasUsadas);
                            }
                            contador++;
                            multiplayer(atual, connfd, pontos);
                            printf("Se desejar jogar outra partida, digite SIM.\n\n");
                            recv(connfd, buf, sizeof(buf), 0);

                            if(strcmp(buf, "exit\n") == 0){
                                close(connfd); /* done with this client */
                                exit(0);
                            }
                        }
                    } else if(strcmp(buf, "-h\n") == 0){  //mosta o README
                        printArquivo("README.txt", 1);
                    } else if(strcmp(buf, "exit\n") == 0){  //cliente sai do jogo
                        close(connfd); /* done with this client */
                        exit(0);
                    } else {
                        menu();
                    }
              }

              close(connfd); /* done with this client */
              exit(0);
          }
          close(connfd);
      }
   return(0);
}
