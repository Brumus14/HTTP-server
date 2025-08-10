# HTTP-server
A Linux HTTP server programmed from scratch in C with no external libraries.

Currently the server can host static websites without a backend and I may update the project to add backend support and some other features listed in todo.txt.

The project is programmed purely in C and supports Linux, no external libraries have been used and only require C standard libraries and Linux headers.

To use the HTTP server to host a static website first clone the respository and compile using the Makefile. Then copy the server executable to the website directory and run it. The server will bind to port 8000 by default which can be changed inside server.c.

Here is a demo of the server hosting my personal website:

https://github.com/user-attachments/assets/6ad6d0a3-b3b9-4004-851b-a1f9d00dbf08
