CFLAGS = -Wall

LIB_CODE = toolkit.c
LIB_HEADER = toolkit.h

all: cliente servidor

cliente: cliente.c $(LIB_CODE) $(LIB_HEADER)
	gcc $(CFLAGS) cliente.c $(LIB_CODE) -o cliente

servidor: servidor.c $(LIB_CODE) $(LIB_HEADER)
	gcc $(CFLAGS) servidor.c $(LIB_CODE) -o servidor