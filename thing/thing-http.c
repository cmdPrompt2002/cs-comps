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
#include <regex.h>

#include "thing-http.h"

//Service-specific parameters
//cs338.jeffondich.com/fdf/login::(F|S)=$'STRING'

//http-get example ./thing -u cs338 -p password -s 80 cs338.jeffondich.com/basicauth/ http-get 
//http-get example ./thing -U usernames.txt -P passwords.txt -s 80 cs338.jeffondich.com/basicauth/ http-get
//http-post example ./thing -u bob@example.com -p bob -s 80 cs338.jeffondich.com/fdf/login http-post
//http-post example ./thing -U usernames.txt -P passwords.txt -s 80 cs338.jeffondich.com/fdf/login http-post

int http_main();
int connect_to_serv(struct addrinfo *servinfo);

int http_get_init();
int http_post_init();

int http_attempt(char *usr, char *pass);
int http_get_attempt(char *usr, char *pass);
int http_post_attempt(char *usr, char *pass, regex_t *successCond, regex_t *failCond);

void sprinkler_send();
char *sprinkler_recv(int attempt, char *fullResponse, int fullResponseSize);

char *to_base64(const unsigned char *data, size_t input_length, char *encoded_data);

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

//HTTP request and response vars
char *passBuffer; //for http-get basicauth only
char *requestBuffer;
char responseBuffer[MAX_RESPONSE_SIZE];
char *fullResponse;
int fullResponseSize;

//HTTP POST only
char *passPrefix;
char *usrPrefix;
char *body;
int contentLength = 0;
int headersLength;
regex_t *successCond = NULL;
regex_t *failCond = NULL;



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
    
    //Set the Host header and finalize the dir
    if (dir == NULL) {
        dir = "/";
        host = destination;
    } else {
        int dirIdx = dir - destination;
        strncpy(host,destination,dirIdx+1);
        host[dirIdx] = '\0';

        char *startParam = strstr(dir,"::"); 
        if (startParam != NULL) {
            char *endParam = strstr(startParam+2,"::");
            startParam[0] = '\0'; //to separate the directory from user input options
            char *param = malloc(sizeof(char)*20);
            int optionLength;
            int optSize = 20;

            while (startParam != NULL) {
                startParam += 4; //points to first idx of option value
                if (endParam == NULL) { optionLength = strlen(startParam) + 1; }  
                else { optionLength = endParam - startParam; }

                if (optionLength > optSize) {
                        param = (char *) realloc(param,optionLength); //Null terminator included
                        optSize = optionLength;
                    }

                if (!strncmp(startParam -2, "S=", 2)) {
                    successCond = (regex_t *) malloc(sizeof(regex_t));
                    strncpy(param,startParam,optionLength);
                    printf("len: %lu, successCond: %s\n", strlen(param),param);
                    if (regcomp(successCond, param, REG_EXTENDED) == -1) {
                        fprintf(stderr, "Fails to compile successCond regex\n");
                        exit(1);
                    }
                } else if (!strncmp(startParam -2, "F=", 2)) {
                    failCond = (regex_t *) malloc(sizeof(regex_t));
                    strncpy(param,startParam,optionLength);
                    printf("len: %lu, failCond: %s\n", strlen(param),param);
                    if (regcomp(failCond, param, REG_EXTENDED) == -1) {
                        fprintf(stderr, "Fails to compile failCond regex\n");
                        exit(1);
                    }
                } else {
                    *(startParam -1) = '\0' ;
                    fprintf(stderr, "Unrecognized parameter: %s\nType 'sprinkler -U [service]' for correct usage\n",startParam-2);
                    exit(1);
                }

                startParam = strstr(startParam, "::");
                if (startParam != NULL) {
                    endParam = strstr(startParam+2, "::");
                }

            }
            free(param);
        }
    
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

    //Initialize required vars for each service
    if (!strcmp(type,"GET")) {
        http_get_init();
    } else if (!strcmp(type,"POST")) {
        http_post_init();
    }

    //Here comes the spraying
    int attemptStatus;
    if (usrFilename != NULL && passFilename != NULL) {
        while (fgets(usr, 256, usrFile) != NULL) {
            if(usr[strlen(usr)-1] == '\n') { //Dont change this to ... != '\0', or the last entry will be shorteded by 1 char.
                usr[strlen(usr)-1] = '\0';
            }                
            while (fgets(pass, 256, passFile) != NULL) {
                
                // Replace new line with null to mark end of password
                if(pass[strlen(pass)-1] == '\n') 
                    pass[strlen(pass)-1] = '\0';
                attemptStatus = http_attempt(usr,pass);
                if (attemptStatus == HTTP_AUTH_SUCCESS) 
                    break;
                 
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
            
        }
    } else if (passFilename != NULL) {
        while (fgets(pass, 256, usrFile) != NULL) {
            if (pass[strlen(pass)-1] == '\n') {pass[strlen(pass)-1] = '\0';}
            attemptStatus = http_attempt(usr,pass);
            if (attemptStatus == HTTP_AUTH_SUCCESS) {break;} 
            
        }
    } else {
        attemptStatus = http_attempt(usr,pass);
    }
    freeaddrinfo(servinfo);
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

    //Finalize the request buffer for each service
    if (!strcmp(type, "GET")) {
        return http_get_attempt(usr,pass);
    } else if (!strcmp(type,"POST")) {
        return http_post_attempt(usr,pass, successCond, failCond);
    } else {
        fprintf(stderr, "Service %s unrecognized\n",service);
        exit(1);
    }
    return 0;
}

int http_get_init() {
    passBuffer = malloc(sizeof(char)*512); //stores "username:password" in unencoded plaintext
    requestBuffer = malloc(sizeof(char)*700);
    fullResponse = malloc(sizeof(char)*100);
    fullResponseSize = 100;
    return 0;
}
//Assumes basic auth
int http_get_attempt(char *usr, char*pass) {
    
    //Concat usr and pass for base64 encoding
    sprintf(passBuffer,"%s:%s",usr,pass);
    // printf("passBuffer: %s\n",passBuffer);
    
    //To base64
    size_t input_length = strlen(passBuffer);
    size_t output_length = 4 * ((input_length + 2) / 3);
    char encodedPass[output_length + 1]; //Stores the base64 encoding of "username:password"
    to_base64((const unsigned char *)passBuffer, input_length, encodedPass) ;

    // printf("%s\n",encodedPass);

    sprintf(requestBuffer,
              "%s %.250s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/5.0 "
                "(Thing)\r\nConnection: "
                "keep-alive\r\nAuthorization: Basic %.*s\r\n\r\n",
              type, dir, host, (int)output_length, encodedPass);

    sprinkler_send();
    sprinkler_recv(0,fullResponse,fullResponseSize);

    if (strstr(fullResponse,"HTTP/1.1 2") != NULL) {
        printf("\033[0;32mAuthentication successful:\033[0m || Destination:%s || Username:%s || Password:%s\n", destination, usr, pass);
        return HTTP_AUTH_SUCCESS;
    } else {
        printf("\033[0;31mAuthentication failure:\033[0m Username:%s, Password:%s\n", usr, pass);
        return HTTP_AUTH_FAILURE;
    }
}

int http_post_init() {
    //Initialize the global vars
    requestBuffer = malloc(sizeof(char)*1000);
    sprintf(requestBuffer,
              "GET %.250s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/5.0 "
                "(Thing)\r\nConnection: "
                "close\r\nAccept: text/html\r\n\r\n",
               dir, host);
    body = malloc(sizeof(char)*300);
    
    /*GET SERVER'S HTML*/ //The html contains the variable names required for authentication

    //Allocate buffer for html
    char *authDetails = malloc(sizeof(char)*10000);

    //Send the HTTP request
    sprinkler_send();

    //Receive the server's response
    authDetails = sprinkler_recv(0,authDetails,10000);
    //printf("%s\n",authDetails);

    /*PARSE SERVER'S HTML*/
    regex_t regex;
    regmatch_t match;
    char *formTag = "<form[^>]*method=\"post\"[^>]*>";
    char *inputTag = "<input[^>]*(name=\"[^\"]*\")[^>]*>"; //|<input[^>]*(value=\"[^\"]*\")[^>]*(name=\"[^\"]*\")[^>]*>  <input[^>]*(name=\"[^\"]*\")[^>]*(value=\"[^\"]*\")[^>]*>|
    char *formEndTag = "</form>";
    
    if (regcomp(&regex, formTag, REG_EXTENDED | REG_ICASE) != 0) {
        fprintf(stderr, "Could not compile formTag regex\n");
        exit(1);
    }

    //Search for the <form...method="post"...> tag 
    int formTag_start; //Start idx of <form...>
    int formTag_end; //End idx of <form ...>
    int formEndTag_start; //Start idx of </form>
    if (regexec(&regex, authDetails, 1, &match, 0) != 0) {
        fprintf(stderr,"Can't find formTag. Change service.\n");
        exit(1);
    } 
    formTag_start = match.rm_so;
    formTag_end = match.rm_eo;
    //Search for the </form> tag that corresponds to the tag above
    if (regcomp(&regex, formEndTag, REG_EXTENDED) != 0) {
        fprintf(stderr, "Could not compile formEndTag regex\n");
        exit(1);
    }
    
    if (regexec(&regex, authDetails + formTag_end, 1, &match, 0) != 0) {
        fprintf(stderr,"Can't find formEndTag. Change service.\n");
        exit(1);
    }
    formEndTag_start = formTag_end + match.rm_so;

    //Now we need to prepare the POST request buffer that sends auth variables
    sprintf(requestBuffer,
              "POST %.250s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/5.0 "
                "(Thing)\r\nConnection: "
                "keep-alive\r\nAccept: text/html\r\n"
                "Content-Type: application/x-www-form-urlencoded\r\n"
                "Origin: http://%s\r\n"
                "Referer: http://%s\r\n",
               dir, host,host,destination);
    
    //Find the input tag in response html (authDetails)
    int start = formTag_end;
    int inputEnd;
    regmatch_t *matches = malloc(sizeof(regmatch_t)*3);
    int valueLength;
    char *name;
    char *value;
    int nameFound, valueFound, usrMatchResult, passMatchResult;
 
    //Find and get each auth variable name and/or values
    while (1) {

        //Prepare the inputTag regex
        if (regcomp(&regex, inputTag, REG_EXTENDED) != 0) {
        fprintf(stderr, "Could not compile inputTag regex\n");
        exit(1);
        } 
        //Search for the inputTag sequence
        if (regexec(&regex, authDetails + start, 3, matches, 0) != 0) {
            break;
        } else if (matches[0].rm_so + start >= formEndTag_start) { 
            //This input tag is after the </form> tag, so it's irrelevant.
            break;
        } 
        nameFound = 0; 
        valueFound = 0;
        inputEnd = matches[0].rm_eo;

        for (int i = 1; i < 3; i++) {
            if (!strncmp(authDetails + start + matches[i].rm_so,"name",4)) {
                nameFound = 1;
                valueLength = (matches[i].rm_eo - matches[i].rm_so) - 7;
                name = malloc(sizeof(char)*(valueLength+1));
                strncpy(name,authDetails + start + matches[i].rm_so + 6,valueLength);
                name[valueLength] = '\0';
                //printf("Val length: %i\n%s\n",valueLength, name);
                

            } else if (!strncmp(authDetails + start + matches[i].rm_so,"value",5)) {
                valueFound = 1;
                valueLength = (matches[i].rm_eo - matches[i].rm_so) - 8;
                value = malloc(sizeof(char)*(valueLength+1));
                strncpy(value,authDetails + start + matches[i].rm_so + 7,valueLength);
                value[valueLength] = '\0';
                //printf("Val length: %i\n%s\n",valueLength, value);
            }
        }

        
        if (nameFound && valueFound) { //If name and value found, then add both to request buffer
            sprintf(body + strlen(body), "%s=%s",name,value);
            //Add the & at the end of each auth var to distinguish between different vars
            sprintf(body + strlen(body), "&");

        } else if (nameFound) { //If only name is found, then check if it could be usr or pass
            //Prepare usr string regex
            char *usrString = "usr|user|email|name";
            if (regcomp(&regex, usrString, REG_EXTENDED | REG_ICASE) != 0) {
                fprintf(stderr, "Could not compile usrString regex\n");
                exit(1);
            }
            usrMatchResult = regexec(&regex, name, 1, &match, 0);
            
            char *passString = "pass|pw";
            if (regcomp(&regex, passString, REG_EXTENDED | REG_ICASE) != 0) {
                fprintf(stderr, "Could not compile passString regex\n");
                exit(1);
            }
            passMatchResult = regexec(&regex, name, 1, &match, 0);

            if (usrMatchResult == 0) {
                usrPrefix = malloc(sizeof(char)*(strlen(name)+1));
                sprintf(usrPrefix,"%s=",name);
            } else if (passMatchResult == 0) {
                passPrefix = malloc(sizeof(char)*(strlen(name)+1));
                sprintf(passPrefix,"%s=",name);
            } else if (usrMatchResult == REG_NOMATCH && passMatchResult == REG_NOMATCH) {
                //sprintf(body + strlen(body), "%s=&",name);
            } else {
                fprintf(stderr, "regexec ran out of memory\n");
            }

            
        }
        start += inputEnd;
 
    }
    free(authDetails);
    contentLength = strlen(body) + strlen(usrPrefix) + strlen(passPrefix) + 1;
    headersLength = strlen(requestBuffer);
    fullResponse = malloc(sizeof(char)*20000);
    fullResponseSize = 20000;
    return 0;
}

int http_post_attempt(char *usr, char *pass, regex_t *successCond, regex_t *failCond) {
    //Finalize the request buffer
    sprintf(requestBuffer + headersLength, "Content-Length: %lu\r\n\r\n%s%s%s&%s%s", 
            contentLength + strlen(usr) + strlen(pass), body, usrPrefix, usr, passPrefix,pass);

    sprinkler_send();
    fullResponse = sprinkler_recv(0,fullResponse,fullResponseSize);

    //Check whether response contains successCond or failCond. 
    //If neither conds are supplied by the user, 
    //then success if server returns a 3XX status code with a location header that's different from the directory in the request
    if (successCond != NULL && failCond != NULL) {
        fprintf(stderr, "Can only accept either success condtion S or failure condition F or neither\n");
        exit(1);

    } else if (successCond != NULL) {
        
        if (regexec(successCond,fullResponse, 0,NULL, 0) == 0) {
            printf("\033[0;32mAuthentication successful:\033[0m || Destination:%s || Username:%s || Password:%s\n", destination, usr, pass);
            return HTTP_AUTH_SUCCESS;
        } else {
            printf("\033[0;31mAuthentication failure:\033[0m Username:%s, Password:%s\n", usr, pass);
            return HTTP_AUTH_FAILURE;
        }

    } else if (failCond != NULL) {

        if (regexec(failCond,fullResponse, 0,NULL, 0) == 0) {
            printf("\033[0;31mAuthentication failure:\033[0m Username:%s, Password:%s\n", usr, pass);
            return HTTP_AUTH_FAILURE;
        } else {
            printf("\033[0;32mAuthentication successful:\033[0m || Destination:%s || Username:%s || Password:%s\n", destination, usr, pass);
            return HTTP_AUTH_SUCCESS;
        }

    } else {
        
        regex_t statusCodeRegex;
        regex_t locationRegex;
        regmatch_t match[2];
        if (regcomp(&statusCodeRegex, "HTTP/1.1 3[^\r\n]*\r\n",REG_EXTENDED) != 0) {
            fprintf(stderr, "Could not compile statusCodeRegex regex\n");
            exit(1);
        }
        if (regexec(&statusCodeRegex,fullResponse, 1,match, 0) == 0) {
            
            if (regcomp(&locationRegex, "Location: ([^\r\n]*)\r\n",REG_EXTENDED) != 0) {
                fprintf(stderr, "Could not compile locationRegex regex\n");
                exit(1);
            }
            if (regexec(&locationRegex,fullResponse, 2,match, 0) == 0) {
                fullResponse[match[1].rm_eo] = '\0';

                //If location header value is same as directory in the request line, then probably wrong usr or pass. 
                if (!strcmp(dir,fullResponse + match[1].rm_so)) {
                    fullResponse[match[1].rm_eo] = '\n';
                    printf("\033[0;31mAuthentication failure:\033[0m Username:%s, Password:%s\n", usr, pass);
                    return HTTP_AUTH_FAILURE;
                }

                //If location header value is different from directory in the request line, then success by default
                fullResponse[match[1].rm_eo] = '\n';
                printf("\033[0;32mAuthentication successful:\033[0m || Destination:%s || Username:%s || Password:%s\n", destination, usr, pass);
                return HTTP_AUTH_SUCCESS;

            }
        } else {
            printf("\033[0;31mAuthentication failure:\033[0m Username:%s, Password:%s\n", usr, pass);
            return HTTP_AUTH_FAILURE;
        }
    }
    return HTTP_AUTH_FAILURE;
}

void sprinkler_send() {
    
    //Send the HTTP request
    bytes_sent = send(sock, requestBuffer, strlen(requestBuffer), 0);
    //printf("%s\n\n\n\n",requestBuffer);
    if (bytes_sent != strlen(requestBuffer)) {
        fprintf(stderr,"Request buffer too large to send in one go\nPrompt needs to make better code\n");
        exit(1);
    }
}

char *sprinkler_recv(int attempt, char *fullResponse, int fullResponseSize) {
    //Receive the server's response
    // printf("Entered recv\n");
    int bytesReceived = recv(sock, responseBuffer, MAX_RESPONSE_SIZE-1, 0);
    if (bytesReceived == -1) {
        
        if (attempt > 3) {
           fprintf(stderr, "recv failed right away\n");
           fprintf(stderr, "%s\n",requestBuffer);
           exit(1);
        }
        fprintf(stderr,"recv failed. Try again...\n");
        // close(sock);
        // connect_to_serv(servinfo);
        sprinkler_send();
        sprinkler_recv(attempt + 1, fullResponse,fullResponseSize);
        
    } else if (bytesReceived == 0) {
        if (attempt > 3) {
           fprintf(stderr, "Fails to reconnect and receive response\n");
           exit(1);
        }
        printf("Connection closed. Try again...\n");
        close(sock);
        connect_to_serv(servinfo);
        sprinkler_send();
        sprinkler_recv(attempt + 1, fullResponse, fullResponseSize);
    } else { //Server's response received. But need to determine if there's more to receive.
        responseBuffer[bytesReceived] = '\0';
        //printf("responseBuffer:\n%s\n",responseBuffer);
        sprintf(fullResponse, "%.*s", fullResponseSize, responseBuffer);

        if (bytesReceived >= fullResponseSize) {
            fullResponse[fullResponseSize - 1] = '\0';
            return fullResponse;
        }
        
        regex_t regex;
        regmatch_t match[2];
        int contentLength;
        
        char *contentLengthString = "Content-Length: ([^\r\n]*)\r\n"; 
        if (regcomp(&regex, contentLengthString ,REG_EXTENDED) != 0) {
            fprintf(stderr, "Could not compile contentLength regex\n");
            exit(1);
        }
        
        int regexResult = regexec(&regex, fullResponse, 2, match, 0);
        if (regexResult == 0) {
            fullResponse[match[1].rm_eo +1] = '\0';
            contentLength = atoi(fullResponse + match[1].rm_so);
            fullResponse[match[1].rm_eo +1] = '\n';
        } else if (regexResult == REG_NOMATCH) {
            contentLength = -1;
        } else {
            fprintf(stderr,"Regexec of contentLengthString failed\n");
            exit(1);
        }
       
        int totalBytesReceived = bytesReceived;
        int bytesToRead;
        while (1) {

            if (totalBytesReceived >= fullResponseSize) {
                fullResponse[fullResponseSize - 1] = '\0';
                break;
            } else if (totalBytesReceived >= contentLength && contentLength != -1) {
                break;
            }

            bytesReceived = recv(sock, responseBuffer, MAX_RESPONSE_SIZE-1, 0);

            if (bytesReceived == -1) {
                fprintf(stderr, "recv failed. Try again...\n");
                int i = 0;
                while (1) {
                    close(sock);
                    connect_to_serv(servinfo);
                    sprinkler_send();
                    bytesReceived = recv(sock, responseBuffer, MAX_RESPONSE_SIZE-1, 0);
                    if (bytesReceived > 0) {
                        break;
                    } else if (bytesReceived == 0) {
                        return fullResponse;
                    } else if (i >= 3) {
                        fprintf(stderr, "Recv failed\n");
                        exit(1);
                    }
                }

            } else if (bytesReceived == 0) {
                //Connection closed - should mean all of server's response is received
                break;
            }

            responseBuffer[bytesReceived] = '\0';

            if (totalBytesReceived + bytesReceived < fullResponseSize) 
                bytesToRead = bytesReceived;
            else 
                bytesToRead = fullResponseSize - totalBytesReceived;
            
            sprintf(fullResponse + totalBytesReceived, "%.*s", bytesToRead, responseBuffer);
            totalBytesReceived += bytesReceived;
        }
    }

    return fullResponse;
}

/*
https://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c
Base64 code from user ryyst
*/
char *to_base64(const unsigned char *data, size_t input_length, char *encoded_data) {
    
    size_t output_length = 4 * ((input_length + 2) / 3);

    
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