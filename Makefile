all: server client

server:
	gcc -o server server.c

client: 
	gcc -o client client.c

remake: clean all

clean:
	rm client server ServerLog.txt ClientLog.txt