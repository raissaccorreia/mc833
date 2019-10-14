#include "toolkit.h"

/*criacao do socket e checagem de sua possibilidade*/
int Socket(int domain, int type, int protocol) {
   int sockfd;

   if ((sockfd = socket(domain, type, protocol)) < 0) {
      perror("socket error");
      exit(1);
   }

   return sockfd;
}

/*aplicando os valores da criacao do socket na conexao*/
int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
   int result;

   if ((result = bind(sockfd, addr, addrlen)) == -1) {
      perror("bind");
      exit(1);
   }

   return result;
}

/*servidor passa estar aberto a mensagens do cliente*/
int Listen(int sockfd, int backlog) {
   int result;

   if ((result = listen(sockfd, backlog)) == -1) {
      perror("listen");
      exit(1);
   }

   return result;
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
   int connfd;

   if ((connfd = accept(sockfd, addr, addrlen)) == -1) {
      perror("accept");
      exit(1);
   }

   return connfd;
}

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
   int result;

   if ((result = connect(sockfd, addr, addrlen)) < 0) {
      perror("connect error");
      exit(1);
   }

   return result;
}

/*funcao para o encerramento da conexao por parte do servidor*/
int Close(int fd) {
   int result;

   if ((result = close(fd)) < 0) {
      perror("connect error");
   }

   return result;
}

/*criacao do socket*/
int Getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
   int result = getsockname(sockfd, addr, addrlen);

   if (result < 0) {
      perror("getsockname() failed");
   }

   return result;
}

/*atribuicao da estrutura ip e porta ao socket pelo servidor*/
struct sockaddr_in ServerSockaddrIn(int family, unsigned long ip, unsigned short port) {
   struct sockaddr_in addr;

   bzero(&addr, sizeof(addr));
   addr.sin_family      = family;
   addr.sin_addr.s_addr = htonl(ip);
   addr.sin_port        = htons(port);

   return addr;
}

/*atribuicao da estrutura ip e porta ao socket pelo cliente*/
struct sockaddr_in ClientSockaddrIn(int family, const char *ip, unsigned short port) {
   struct sockaddr_in addr;

   bzero(&addr, sizeof(addr));
   addr.sin_family      = family;
   addr.sin_port        = htons(port);

   if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
      perror("inet_pton error");
      exit(1);
   }

   return addr;
}
