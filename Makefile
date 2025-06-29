all: server client

server: src/server.c
	@gcc src/server.c -o server

client: src/client.c
	@gcc src/client.c -o client
