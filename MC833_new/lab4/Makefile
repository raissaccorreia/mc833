CFLAGS = -Wall

LIB_CODE = toolkit.c
LIB_HEADER = toolkit.h

all: cliente servidor client_unselect

cliente: cliente.c $(LIB_CODE) $(LIB_HEADER)
	gcc $(CFLAGS) cliente.c $(LIB_CODE) -o cliente

servidor: servidor.c $(LIB_CODE) $(LIB_HEADER)
	gcc $(CFLAGS) servidor.c $(LIB_CODE) -o servidor

client_unselect: client_unselect.c $(LIB_CODE) $(LIB_HEADER)
	gcc $(CFLAGS) client_unselect.c $(LIB_CODE) -o client_unselect