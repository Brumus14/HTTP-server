FROM alpine:3.22 AS build
RUN apk add --no-cache build-base git
WORKDIR /server
RUN git clone --branch v0.1 --depth 1 https://github.com/Brumus14/HTTP-server
WORKDIR HTTP-server
RUN make all

FROM alpine:3.22
WORKDIR /server
COPY --from=build /server/HTTP-server/server .
RUN chmod +x server
EXPOSE 8000
RUN mkdir /server/data
ENTRYPOINT ["/bin/sh", "-c", "/server/server /server/data"]
