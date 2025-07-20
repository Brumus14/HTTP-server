all: server

server: src/server/main.c src/server/server.c src/server/request.c src/server/helper.c src/server/target.c src/server/response.c src/server/http.c
	gcc src/server/main.c src/server/server.c src/server/request.c src/server/helper.c src/server/target.c src/server/response.c src/server/http.c -Wall -o server
