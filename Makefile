all: server client

server: src/server/main.c src/server/server.c src/server/request.c src/server/helper.c src/server/target.c src/server/response.c
	@gcc src/server/main.c src/server/server.c src/server/request.c src/server/helper.c src/server/target.c src/server/response.c -o server

client: src/client/main.c
	@gcc src/client/main.c -o client
