#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Network file headers
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include "thing-http.h"

int http_main();
int http_attempt(char *usr, char *pass, char* service);
void http_output();
char* to_base64(char *str);

//HTTP Global vars
int status;
int sock;
const struct sockaddr address;
struct addrinfo hints;
struct addrinfo *servinfo;
int bytes_sent;
char *host;

char meth;
char passBuffer[200];
char requestBuffer[500];
char responseBuffer[2000];



//Web: http://cs338.jeffondich.com/basicauth/
int http_main() {
    if (usrFilename != NULL && passFilename != NULL) {
        return 0;
    } else if (usrFilename != NULL) {
        return 0;
    } else if (passFilename != NULL) {
        return 0;
    } else {
        http_attempt(usr,pass,service);
    }
    return 0;
}

int http_attempt(char *usr, char *pass, char* service) {

    //Prepare the socket
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // Use TCP, not UDP

    if ((status = getaddrinfo(destination, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    if ((sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
        fprintf(stderr, "socket failed\n");
        exit(1);
    }

    // setsockopt(sock,SOL_SOCKET,SO_REUSEADDR, 1, 1);
    // setsockopt(sock,SOL_SOCKET,SO_REUSEADDR, 1, 1);

    if (bind(sock, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
        fprintf(stderr,"bind failed\n");
        exit(1);
    }

    //Connect to the destination
    if (connect(sock, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        fprintf(stderr, "Connection failure\n");
        exit(1);
    }

    //Prepare HTTP request

    //GET line and Host header
    char *type = "GET";

    host = strstr(destination, "/");
    if (!strcmp(host,"")) {
        host = destination;
    }
    printf("Host: %s\n",host);

    //Basic auth
    sprintf(passBuffer,"%s:%s",usr,pass);
    to_base64(passBuffer);

    sprintf(requestBuffer,
              "%s %.250s HTTP/1.1\r\nHost: %s\r\nConnection: "
                "close\r\nAuthorization: Basic %s\r\nUser-Agent: Mozilla/5.0 "
                "(Thing)\r\n",
              type, destination + idx, host, passBuffer);

    //Send the HTTP request
    bytes_sent = send(sock, requestBuffer, strlen(requestBuffer), 0);
    if (bytes_sent != strlen(requestBuffer)) {
        perror("Request buffer too large to send in one go\nPrompt needs to make better code\n");
        exit(1);
    }
    //Recover the server's response
    if (recv(socket, responseBuffer, sizeof(responseBuffer), 0) == -1) {
        perror("recv failed\n");
    } else {
        printf("%s",responseBuffer);
    }

    close(sock);
    return 0;
}

void http_output() {

}

char* to_base64(char *str) {

}