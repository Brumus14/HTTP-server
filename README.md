# HTTP-server
Currently the server can host static websites without a backend and I may update the project to add backend support and some other features listed in todo.txt.

The project is programmed purely in C and natively supports Linux, no external libraries have been used and only require C standard libraries and Linux headers. It uses the HTTP 1.1 specification.

It can be used cross-platform using Docker.

## Compilation and Usage
### Binary (Linux):
First clone the repository and compile using the `Makefile`. Run the generated `server` binary, you can optionally specifiy the directory to host as a command line argument otherwise it hosts the directory the binary is inside. By default the server binds to 0.0.0.0 on port 8000 which can be changed inside `server.c` and recompiling.

### Docker (cross-platform):
Download the Dockerfile and build it into a Docker image, then use the generated image to run a container with `docker run -p <port>:8000 -v "<directory>:/server/data" <generated-image-name>`,
- `port` is the port to host the server on for the host,
- `directory` is where the files are to be hosted,
- `generated-image-name` is the name you gave the Docker image when you built it.

Once running the server will be hosted on 0.0.0.0 at the port you gave it.

## Note
The server by default hosts the root target (`/`) with `/index.html` so by going to 0.0.0.0:8000 is essentially the same as 0.0.0.0:8000/index.html, this can be modified by changing `TARGET_ROOT_TARGET` inside `target.c`.

## Demo
Here is a demo of the server hosting my personal website:

https://github.com/user-attachments/assets/6ad6d0a3-b3b9-4004-851b-a1f9d00dbf08
