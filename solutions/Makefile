CFLAGS += -Wall -Wextra -pedantic -O0 -std=gnu17
all: ex3_client ex3_server 


ex3_client: exercice3/echo_client.c common.h
	${CC} -o $@ $^ ${CFLAGS}

ex3_server: exercice3/echo_server.c common.h
	${CC} -o $@ $^ ${CFLAGS}


clean:
	-rm ex[134]_client ex[134]_server
