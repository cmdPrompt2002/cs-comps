#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
//#include <fcntl.h>
#include <netdb.h>
#include <math.h>
#include <errno.h>
#include <regex.h>
#include <time.h>

//#include <poll.h> //For async I/O (WIP)

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "sprinkler-http.h"

//Service-specific parameters
//cs338.jeffondich.com/fdf/login::(F|S)=$'STRING'

//http-get example ./sprinkler -u cs338 -p password -s 80 cs338.jeffondich.com/basicauth/ http-get 
//http-get example ./sprinkler -U usernames.txt -P passwords.txt -s 80 cs338.jeffondich.com/basicauth/ http-get
//http-post example ./sprinkler -u bob@example.com -p bob -s 80 cs338.jeffondich.com/fdf/login http-post
//http-post example ./sprinkler -U usernames.txt -P passwords.txt -s 80 cs338.jeffondich.com/fdf/login http-post
// en.wikipedia.org/wiki/Special:UserLogin
// ./sprinkler -U usernames.txt -P passwords.txt -s 443 -S authenticationtest.com/HTTPAuth/ http-get
// ./sprinkler -U usernames.txt -P passwords.txt -s 443 -S authenticationtest.com/HTTPAuth/::F=$'Location:[^\r\n]*loginFail/\r\n' http-get
// ./sprinkler -U usernames.txt -P passwords.txt -s 443 -S authenticationtest.com/complexAuth/::F=$'Location:[^\r\n]*loginFail/\r\n' http-post

int http_main(char *usr, char *pass, FILE *usrFile, FILE *passFile, int tls, regex_t *checkStr, int responseCheck, char *inputParam);
int sprinkler_connect(struct addrinfo *servinfo);
SSL *sprinkler_tls_connect(SSL_CTX *ctx);

int http_get_init();
int http_post_init(int tls, char *inputParam);

int http_attempt(char *usr, char *pass, int tls, regex_t *checkStr, int responseCheck);
int http_get_attempt(char *usr, char*pass, int tls, regex_t *checkStr, int responseCheck);
int http_post_attempt(char *usr, char *pass, int tls, regex_t *checkStr, int responseCheck);

void sprinkler_send();
void sprinkler_tls_send(SSL *ssl);
void sprinkler_recv(char *fullResponse, int fullResponseSize, int receiveAll);
void sprinkler_tls_recv(char *fullResponse, int fullResponseSize,int receiveAll);

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
extern int verbose;
extern float delay;

//Server response type
#define HTTP_AUTH_SUCCESS 0
#define HTTP_AUTH_FAILURE 1
// #define HTTP_CONNECTION_CLOSED 2
#define MAX_RESPONSE_SIZE 1000

//HTTP request and response vars
char *passBuffer; //for http-get basicauth only
char *requestBuffer;
//char responseBuffer[MAX_RESPONSE_SIZE];
char *fullResponse;
int fullResponseSize;

//HTTP POST only
char *passPrefix;
char *usrPrefix;
char *body;
int reqContentLength = 0;
int headersLength;
regex_t *successCond = NULL;
regex_t *failCond = NULL;

//TLS 
SSL_CTX *ctx = NULL;
SSL *ssl = NULL;

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

int http_main(char *usr, char *pass, FILE *usrFile, FILE *passFile, int tls, regex_t *checkStr, int responseCheck, char* inputParam) {
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

    //Dir to send logins to
    dir = strstr(destination, "/");
    
    //Set the Host header and finalize the dir
    if (dir == NULL) {
        dir = "/";
        host = destination;
    } else {
        int dirIdx = dir - destination;
        strncpy(host,destination,dirIdx+1);
        host[dirIdx] = '\0';
    }

    //Get success/failure condition string. (Ideally, should be in sprinkler-http file or need to be able to process form variables too)
    

    //Get server information into servinfo
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;     // Accepts both ipv4 and ipv6 protocols
    hints.ai_socktype = SOCK_STREAM; // Use TCP, not UDP

    if ((status = getaddrinfo(host, charPort, &hints, &servinfo)) != 0) {
        fprintf(stderr, "[ERROR] getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    //Connect to server
    sprinkler_connect(servinfo);

    //If using tls, then initialize necessary openssl variables
    if (tls) {
        ctx = SSL_CTX_new(TLS_client_method());
        if (ctx == NULL) {
            fprintf(stderr,"Failed to create the SSL_CTX\n");
            exit(1);
        }

        SSL_CTX_set_options(ctx, SSL_OP_ALL);

        if (!SSL_CTX_set_default_verify_paths(ctx)) {
            fprintf(stderr,"Failed to set the default trusted certificate store\n");
            exit(1);
        }
        
        //Don't care about server's certificate, so SSL_VERIFY_NONE
        SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
        ssl = sprinkler_tls_connect(ctx); 
    }


    

    //Initialize required vars for each service
    if (!strcmp(type,"GET")) {
        http_get_init();
    } else if (!strcmp(type,"POST")) {
        http_post_init(tls, inputParam);
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
                
                sleep(delay);
                attemptStatus = http_attempt(usr,pass,tls, checkStr, responseCheck);
                
                
                if (attemptStatus == HTTP_AUTH_SUCCESS) 
                    break;
                 
            }
            rewind(passFile);
        }
        fclose(usrFile);
        fclose(passFile);
    } else if (usrFilename != NULL) {
        while (fgets(usr, 256, usrFile) != NULL) {
           
            if (pass[strlen(pass)-1] == '\n') {pass[strlen(pass)-1] = '\0';}
            attemptStatus = http_attempt(usr,pass,tls,checkStr, responseCheck);
            
        }
    } else if (passFilename != NULL) {
        while (fgets(pass, 256, passFile) != NULL) {
            if (pass[strlen(pass)-1] == '\n') {pass[strlen(pass)-1] = '\0';}
            attemptStatus = http_attempt(usr,pass,tls,checkStr, responseCheck);
            if (attemptStatus == HTTP_AUTH_SUCCESS) {break;} 
            
        }
    } else {
        attemptStatus = http_attempt(usr,pass,tls,checkStr, responseCheck);
    }


    freeaddrinfo(servinfo);
    free(fullResponse);
    free(requestBuffer);
    
    if (!tls) {
        close(sock);
    } else {
        //Close with openssl function
    }

    return 0;
}

int sprinkler_connect(struct addrinfo *servinfo) {

    //Connect to the destination (don't care if ipv4 or ipv6)
    struct addrinfo *p;
    for (p = servinfo; p != NULL; p = p->ai_next) {

        if ((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }

        //Set the socket to non-blocking mode
        // if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0) {
        //     fprintf(stderr,"fcntl failed");
        //     exit(1);
        // }
        
        if (connect(sock, p->ai_addr, p->ai_addrlen) == -1 && errno != EINPROGRESS) {
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

SSL *sprinkler_tls_connect(SSL_CTX *ctx) {

    int err;
    
    SSL *ssl = SSL_new(ctx);
    if (ssl == NULL) {
        fprintf(stderr,"Failed to create the SSL object\n");
        SSL_clear(ssl);
        exit(1);
    }

    SSL_set_bio(ssl, NULL, NULL);

    /*
    * Tell the server during the handshake which hostname we are attempting
    * to connect to in case the server supports multiple hosts. SNI = server name indication
    */
    if (!SSL_set_tlsext_host_name(ssl, host)) {
        fprintf(stderr,"Failed to set the SNI hostname\n");
        exit(1);
    }

    if (!SSL_set_fd(ssl,sock)) {
        fprintf(stderr,"SSL_set_fd failed.\n");
        exit(1);
    }

    /* Do the TLS handshake with the server */
    if (SSL_connect(ssl) < 1) {
        err = ERR_get_error();
        fprintf(stderr, "Failed to connect to server: %s\n", ERR_error_string(err, NULL));
        exit(1);
    }

    return ssl;
}

int http_attempt(char *usr, char *pass, int tls, regex_t *checkStr, int responseCheck) {

    //Finalize the request buffer for each service
    if (!strcmp(type, "GET")) {
        return http_get_attempt(usr, pass, tls, checkStr, responseCheck);
    } else if (!strcmp(type,"POST")) {
        return http_post_attempt(usr,pass, tls, checkStr, responseCheck);
    } else {
        fprintf(stderr, "Service %s unrecognized\n",service);
        exit(1);
    }
    return 0;
}

int http_get_init() {
    passBuffer = malloc(sizeof(char)*512); //stores "username:password" in unencoded plaintext
    requestBuffer = malloc(sizeof(char)*700);
    fullResponseSize = 450;
    fullResponse = malloc(sizeof(char)*fullResponseSize);
    return 0;
}

//Assumes basic auth
int http_get_attempt(char *usr, char*pass, int tls, regex_t *checkStr, int responseCheck) {
    
    //Concat usr and pass for base64 encoding
    sprintf(passBuffer,"%s:%s",usr,pass);
    
    //To base64
    size_t input_length = strlen(passBuffer);
    size_t output_length = 4 * ((input_length + 2) / 3);
    char encodedPass[output_length + 1]; //Stores the base64 encoding of "username:password"
    to_base64((const unsigned char *)passBuffer, input_length, encodedPass) ;

   //Finalize the request buffer
    sprintf(requestBuffer,
              "%s %.250s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/5.0 "
                "(Sprinkler)\r\nConnection: "
                "keep-alive\r\nAuthorization: Basic %.*s\r\n\r\n",
              type, dir, host, (int)output_length, encodedPass);

    if (tls) {
        sprinkler_tls_send(ssl);
        sprinkler_tls_recv(fullResponse, fullResponseSize, 0);
        // printf("%s\n",fullResponse);
    } else {
        sprinkler_send();
        sprinkler_recv(fullResponse, fullResponseSize, 0);
        //printf("fullResponseSize: %d\n",fullResponseSize);
        //printf("%s\n=======================\n",fullResponse);
    }
    
    int ret;
    if (responseCheck == 1) {
        if ((ret = regexec(checkStr,fullResponse, 0,NULL, 0)) == 0) {
            printf("\033[0;32mAuthentication successful:\033[0m || Destination:%s || Username:%s || Password:%s\n", destination, usr, pass);
            return HTTP_AUTH_SUCCESS;
        } else if (ret == REG_NOMATCH) {
            if (verbose == 1) {
                printf("\033[0;31mAuthentication failure:\033[0m Username:%s, Password:%s\n", usr, pass);
            }
            return HTTP_AUTH_FAILURE;
        } else {
            fprintf(stderr, "Regexec of successCond failed\n");
            exit(1);
        }

    } else if (responseCheck == 0) {
        if ((ret = regexec(checkStr,fullResponse, 0,NULL, 0)) == 0) {
            if (verbose == 1) {
                printf("\033[0;31mAuthentication failure:\033[0m Username:%s, Password:%s\n", usr, pass);
            }
            return HTTP_AUTH_FAILURE;
        } else if (ret == REG_NOMATCH) {
            printf("\033[0;32mAuthentication successful:\033[0m || Destination:%s || Username:%s || Password:%s\n", destination, usr, pass);
            return HTTP_AUTH_SUCCESS;
        } else {
            fprintf(stderr, "Regexec of failCond failed\n");
            exit(1);
        }
    } else {
        if (strstr(fullResponse,"HTTP/1.1 2") != NULL) {
            printf("\033[0;32mAuthentication successful:\033[0m || Destination:%s || Username:%s || Password:%s\n", destination, usr, pass);
            return HTTP_AUTH_SUCCESS;
        } else {
            if (verbose == 1)
                printf("\033[0;31mAuthentication failure:\033[0m Username:%s, Password:%s\n", usr, pass);
            return HTTP_AUTH_FAILURE;
        }
    }
    
}

int http_post_init(int tls, char *inputParam) {
    //Initialize the global vars
    requestBuffer = malloc(sizeof(char)*1000);
    body = malloc(sizeof(char)*500);
    body[0] = '\0';

    //email=^USER^&passwd=^PASS^&status=lol&...
    if (inputParam != NULL) { //Only works if the server uses application/x-www-form-urlencoded
        
        char *cur = inputParam;
        
        char *sep = NULL;
        char *and = NULL;
        int len = 0;
        
        while (cur != NULL) {
            sep = strstr(cur, "=");
            if (sep == NULL || sep - cur <= 1) {
                fprintf(stderr,"invalid usage for option -i: %s\n", inputParam);
                exit(1);
            }
            and = strstr(sep, "&");
            if (and != NULL && and - sep <= 0) {
                fprintf(stderr, "invalid usage for option -i: %s\n", inputParam);
                exit(1);
            }
            
            if (!strncmp(sep+1,"^USER^",6)) {
                if (usrPrefix != NULL) {
                    fprintf(stderr, "Invalid usage for option -i: %s\n", inputParam);
                    exit(1);
                }
                usrPrefix = malloc(sizeof(char)*(sep-cur+2)); 
                //strncpy(usrPrefix, cur, sep - cur);
                sprintf(usrPrefix, "%.*s=", (int)(sep-cur) , cur);
                // printf("usrPrefix: %s\n", usrPrefix);
            } else if (!strncmp(sep+1, "^PASS^",6)) {
                if (passPrefix != NULL) {
                    fprintf(stderr, "Invalid usage for option -i: %s\n", inputParam);
                    exit(1);
                }
                passPrefix = malloc(sizeof(char)*(sep-cur+2));
                //strncpy(passPrefix, cur, len);
                sprintf(passPrefix, "%.*s=", (int)(sep-cur) , cur);
                // printf("passPrefix: %s\n", passPrefix);
            } else {
                len = (and == NULL) ? strlen(cur) : and - cur;
                sprintf(body + strlen(body), "%.*s&", len, cur);
                printf("Body so far: %s\n", body);
            }

            cur = (and == NULL) ? NULL : and + 1;

        }

        sprintf(requestBuffer,
                "POST %.250s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/5.0 "
                    "(Thing)\r\nConnection: "
                    "keep-alive\r\nAccept: text/html\r\n"
                    "Content-Type: application/x-www-form-urlencoded\r\n"
                    "Origin: http://%s\r\n"
                    "Referer: http://%s\r\n",
                dir, host,host,destination);
        

    } else {
       
        sprintf(requestBuffer,
              "GET %.250s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/5.0 "
                "(Thing)\r\nConnection: "
                "close\r\nAccept: text/html\r\n\r\n",
               dir, host);
        /*GET SERVER'S HTML*/ //The html contains the variable names required for authentication

        //Allocate buffer for html
        int authDetailsSize = 5000;
        char *authDetails = malloc(sizeof(char)*authDetailsSize);

        //Send the HTTP request
        

        if (tls) {
            sprinkler_tls_send(ssl);
            sprinkler_tls_recv(authDetails, authDetailsSize, 0);
        } else {
            sprinkler_send();
            sprinkler_recv(authDetails, authDetailsSize, 1);
        }
        
        //authDetails = sprinkler_recv(0,authDetails,authDetailsSize);
        

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
                sprintf(body + strlen(body), "%s=%s&",name,value);
                //Added the & at the end of each auth var to distinguish between different vars
                

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
    }

    reqContentLength = strlen(body) + strlen(usrPrefix) + strlen(passPrefix) + 1;
    headersLength = strlen(requestBuffer);
    fullResponseSize = 1500;
    fullResponse = malloc(sizeof(char)*fullResponseSize);
  
    return 0;
}

int http_post_attempt(char *usr, char *pass, int tls, regex_t *checkStr, int responseCheck) {
    //Finalize the request buffer
    
    sprintf(requestBuffer + headersLength, "Content-Length: %lu\r\n\r\n%s%s%s&%s%s", 
            reqContentLength + strlen(usr) + strlen(pass), body, usrPrefix, usr, passPrefix, pass);

    
    if (tls) {
        sprinkler_tls_send(ssl);
        sprinkler_tls_recv(fullResponse, fullResponseSize, 0);
    } else {
        sprinkler_send();
        sprinkler_recv(fullResponse,fullResponseSize, 1);
        //printf("%s\n=======================\n",fullResponse);
        
    }

    //Check whether response contains successCond or failCond. 
    //If neither conds are supplied by the user, 
    //then success if server returns a 3XX status code with a location header that's different from the directory in the request
    int ret;
    if (responseCheck == 1) {
        
        if ((ret = regexec(checkStr,fullResponse, 0,NULL, 0)) == 0) {
            printf("\033[0;32mAuthentication successful:\033[0m || Destination:%s || Username:%s || Password:%s\n", destination, usr, pass);
            return HTTP_AUTH_SUCCESS;
        } else if (ret == REG_NOMATCH) {
            if (verbose == 1) {
                printf("\033[0;31mAuthentication failure:\033[0m Username:%s, Password:%s\n", usr, pass);
            }
            return HTTP_AUTH_FAILURE;
        } else {
            fprintf(stderr, "regexec of successCond failed\n");
            exit(1);
        }

    } else if (responseCheck == 0) {

        if ((ret = regexec(checkStr,fullResponse, 0,NULL, 0)) == 0) {
            if (verbose == 1) {
                printf("\033[0;31mAuthentication failure:\033[0m Username:%s, Password:%s\n", usr, pass);
            }
            return HTTP_AUTH_FAILURE;
        } else if (ret == REG_NOMATCH) {
            printf("\033[0;32mAuthentication successful:\033[0m || Destination:%s || Username:%s || Password:%s\n", destination, usr, pass);
            return HTTP_AUTH_SUCCESS;
        } else {
            fprintf(stderr, "regexec of successCond failed\n");
            exit(1);
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
                    if (verbose == 1) {
                        printf("\033[0;31mAuthentication failure:\033[0m Username:%s, Password:%s\n", usr, pass);
                    }
                    
                    return HTTP_AUTH_FAILURE;
                }

                //If location header value is different from directory in the request line, then success by default
                fullResponse[match[1].rm_eo] = '\n';
                printf("\033[0;32mAuthentication successful:\033[0m || Destination:%s || Username:%s || Password:%s\n", destination, usr, pass);
                return HTTP_AUTH_SUCCESS;

            }
        } else {
            if (verbose == 1) {
                printf("\033[0;31mAuthentication failure:\033[0m Username:%s, Password:%s\n", usr, pass);
            }
            
            return HTTP_AUTH_FAILURE;
        }
    }
    return HTTP_AUTH_FAILURE;
}

void sprinkler_send() {
    //Send the HTTP request
    int requestLength = strlen(requestBuffer);
    bytes_sent = send(sock, requestBuffer, requestLength, 0);
    
    if (bytes_sent == -1) {
        fprintf(stderr, "Failed to send the HTTP request\n");
        exit(1);
    } else if (bytes_sent != requestLength) {
        fprintf(stderr,"Request buffer too large to send in one go\nPrompt needs to make better code\n");
        exit(1);
    }
    
}

void sprinkler_tls_send(SSL *ssl) {
    size_t written;
    size_t requestLength = strlen(requestBuffer);

    //Send the HTTP request
    if (!SSL_write_ex(ssl, requestBuffer, requestLength, &written)) {
        fprintf(stderr, "Failed to send HTTP request using TLS\n");
        exit(1);
    } else if (written != requestLength) {
        fprintf(stderr,"Request buffer too large to send in one go\nPrompt needs to make better code\n");
        exit(1);
    }
    
}

/* void sprinkler_recv: 
DESCRIPTION
    Read the server's response into buf by calling C's recv function.
    Compatible with both chunked and non-chunked encoding.
    Can specify whether all of server's response should be read into buf.
INPUTS
    * char *buf: pointer to the buffer that stores bytes from the server
    * int bufSize: total number of bytes allocated for buf
    * int receiveAll:
            1: Read everything from the server's response into buf.
               If the server's response > bufSize, then buf is realloced automatically
            0: Only read up to bufSize bytes at most, and discard the rest into a trash array
OUTPUT
    None
*/
void sprinkler_recv(char *buf, int bufSize, int receiveAll) {
    
    /* Bytes received from a single call to recv */
    int bytesReceived = 0;

    /* Cumulative no. of bytes received */
    int totalBytesReceived = 0;

    /* Bytes to read into buf. Calculation of this value depends on receiveAll and whether the target's response is chunked*/
    int bytesToRead = bufSize - 1; 

    /* How many bytes from server's response to discard into the trash array (large buffer for stuff we don't care about)*/
    int bytesToTrash = 0;

    /* How many time has recv returns an error value */
    int numFails = 0;

    /* If server uses non-chunked encoding, this will store the value of the Content-Length header */
    int contentLength = -1;

    /* Stores the pointer to the end of headers section (marked by \r\n\r\n) */
    char *endHeader = NULL;

    /* This variable points to the end of response body. Used iff server uses chunked encoding */
    char *endBody = NULL;

    /* If server's response uses chunked encoding, then no Content-Length header. Need to call recv until endBody marker is found */
    int chunked = 0;

    //Get the Content-Length header value
    while (endHeader == NULL && bytesToRead > 0) {

        while ((bytesReceived = recv(sock, buf + totalBytesReceived, bytesToRead, 0)) <= 0) {
            close(sock);
            if (numFails > 3) {
                fprintf(stderr,"Reattempts failed: %s\n",strerror(errno));
                // printf("Server's last response: %s\n", buf);
                // printf("Current request: %s\n", requestBuffer);
                // printf("%s\n",strerror(errno));
                exit(1);
            }

            if (bytesReceived == -1 && verbose == 1) 
                fprintf(stderr,"[VERBOSE] sprinkler_recv: recv failed. Reattempting...\n");
            else if (bytesReceived == 0 && verbose == 1)
                fprintf(stderr,"[VERBOSE] sprinkler_recv: server closed connection. Reconnecting...\n");

            sprinkler_connect(servinfo);
            sprinkler_send();
            numFails++;
        }

        buf[bytesReceived] = '\0';
        bytesToRead -= bytesReceived;
        totalBytesReceived += bytesReceived;
        endHeader = strstr(buf, "\r\n\r\n");

    }
   
    if (endHeader == NULL) {
        fprintf(stderr, "Headers length larger than bufSize. Prompt needs better code.\n");
        //printf("%s\n",buf);
        exit(1);
    }

    //Compile the regex and find the Content-Length value 
    regex_t regex;
    regmatch_t match[2];
    
    char *contentLengthString = "Content-Length: ([^\r\n]*)\r\n"; 
    if (regcomp(&regex, contentLengthString ,REG_EXTENDED) != 0) {
        fprintf(stderr, "Could not compile contentLength regex\n");
        exit(1);
    }
    
    int regexResult = regexec(&regex, buf, 2, match, 0);
    int numHeaderBytes =  endHeader + 3 - buf + 1; //Correct. This is how many bytes goes into the header (\r\n\r\n included)
    int numContentBytesRead = -1; 
    if (regexResult == 0) { //Found Content-Length header
        buf[match[1].rm_eo] = '\0';
        contentLength = atoi(buf + match[1].rm_so); //Correct
        buf[match[1].rm_eo] = '\r';

        //Find out how much of the body is read, and adjust bytesToRead accordingly
        
        numContentBytesRead = totalBytesReceived - numHeaderBytes; //Correct
        bytesToRead = contentLength - numContentBytesRead; //Correct if all of response is already in buf. Also correct if only some of response is in buf

        //Realloc buf if bytesToRead is larger than its size. If clause correct. Realloc correct.
        if (receiveAll == 1 && bytesToRead >= bufSize - totalBytesReceived) {
            if (verbose == 1) {
                printf("[VERBOSE] sprinkler_recv: target's response larger than allocated buffer. Reallocing...\n");
            }
            bufSize = (totalBytesReceived + contentLength + 1)*1.25;
            fullResponse = (char *) realloc(fullResponse, (size_t) bufSize);
            buf = fullResponse;
            fullResponseSize = bufSize;
        }

        //In case we don't need all of server's response, adjust these variables. looks correct
        else if (receiveAll == 0 && bytesToRead >= bufSize - totalBytesReceived) {
            bytesToRead = bufSize - totalBytesReceived - 1;
            bytesToTrash = contentLength - numContentBytesRead - bytesToRead;
        }

    } else if (regexResult == REG_NOMATCH) { 
        //Chunked encoding might be used. Check if that's true
        char *ptr = strstr(buf,"Transfer-Encoding: chunked\r\n");
        if (ptr != NULL && ptr - buf < numHeaderBytes) {
            
            bytesToRead = bufSize - totalBytesReceived - 1;
            chunked = 1;
            endBody = strstr(buf,"\r\n0\r\n");
        } else {
            fprintf(stderr,"Server's response is unsupported. Missing Content-Length or chunked transfer encoding\n");
            exit(1);
        }

        //If buf is full but we haven't found the marker for end of body, realloc if necessary
        if (bytesToRead <= 0 && endBody == NULL && receiveAll) {
            if (verbose == 1) {
                printf("[VERBOSE] sprinkler_recv: target's response larger than allocated buffer. Reallocing...\n");
            }
            bufSize *=2;
            fullResponse = (char *) realloc(fullResponse, (size_t) bufSize);
            buf = fullResponse;
            fullResponseSize = bufSize;
            bytesToRead = bufSize - totalBytesReceived -1;
        }
        
    } else {
        fprintf(stderr,"Regexec of contentLengthString failed\n");
        exit(1);
    }
    
    
    //Receive the rest of the response
    numFails = 0;
    //printf("BytesToRead: %d\nEndBody: %s\n",bytesToRead,endBody);
    while (bytesToRead > 0 && endBody == NULL) {
        //printf("Receiving the rest of response\n");
        while ((bytesReceived = recv(sock, buf + totalBytesReceived, bytesToRead, 0)) <= 0) {
            
            close(sock);
            if (numFails > 3) {
                fprintf(stderr,"Reattempts failed.\n");
                exit(1);
            }
            
            if (bytesReceived == -1 && verbose == 1) 
                fprintf(stderr,"[VERBOSE] sprinkler_recv: recv failed. Reattempting...\n");
            else if (bytesReceived == 0 && verbose == 1)
                fprintf(stderr,"[VERBOSE] sprinkler_recv: server closed connection. Reconnecting...\n");

            sprinkler_connect(servinfo);
            sprinkler_send();
            numFails++;
        }
        totalBytesReceived += bytesReceived;
        buf[totalBytesReceived] = '\0';
        bytesToRead -= bytesReceived;

        if (chunked) {
            printf("Here!\n");
            endBody = strstr(buf,"\r\n0\r\n"); //Should switch to indexing to improve speed!!!

            //If buf is full but we haven't found the marker for end of body, realloc if necessary
            if (bytesToRead <= 0 && endBody == NULL && receiveAll) {
                if (verbose == 1) {
                    printf("[VERBOSE] sprinkler_recv: target's response larger than allocated buffer. Reallocing...\n");
                }
                bufSize *=2;
                fullResponse = (char *) realloc(fullResponse, (size_t) bufSize);
                buf = fullResponse;
                fullResponseSize = bufSize;
                bytesToRead = bufSize - totalBytesReceived -1;
            }
        }
        
    }
    
    //Read bytes we don't care into the trash array. CORRECT!
    if (bytesToTrash != 0 || (chunked == 1 && endBody == NULL) ) {
        //printf("Enters trash\nendBody: %s\nBytesToTrash: %d\n",endBody,bytesToTrash);
        //We need to account for the case where some part of the end marker is splitted into the buf array.
        numFails = 0;
        char trash[MAX_RESPONSE_SIZE];
        strncpy(trash,buf+bufSize-6,5); //Copies the last 5 bytes in buf into the trash array, in case those constitute the end marker

        while ((chunked == 0 && bytesToTrash > 0) || (chunked == 1 && endBody == NULL)) {
            while ((bytesReceived = recv(sock, trash+5, MAX_RESPONSE_SIZE-5, 0)) <= 0) {
                
                close(sock);
                if (numFails > 3) {
                    fprintf(stderr,"Reattempts failed.\n");
                    exit(1);
                }
                
                if (bytesReceived == -1 && verbose == 1) 
                    fprintf(stderr,"[VERBOSE] sprinkler_recv: recv failed. Reattempting...\n");
                else if (bytesReceived == 0 && verbose == 1)
                    fprintf(stderr,"[VERBOSE] sprinkler_recv: server closed connection. Reconnecting...\n");

                sprinkler_connect(servinfo);
                sprinkler_send();
                numFails++;
            }
            //totalBytesReceived += bytesReceived; //No need
            
            if (chunked) {
                endBody = strstr(trash, "\r\n0\r\n");
                strncpy(trash,trash+MAX_RESPONSE_SIZE-5,5); //-5 bc no null byte at the end
            } else {
                bytesToTrash -= bytesReceived;
            }
            // trash[MAX_RESPONSE_SIZE-1] = '\0';
            // printf("TRASH: %s\n\n",trash);
        }
    }
}

void sprinkler_tls_recv(char *fullResponse, int fullResponseSize, int receiveAll) {
    size_t bytesReceived;
    int totalBytesReceived = 0;
    int retValue = 0;
    int err = 0;
    int numFails = 0;

    if (receiveAll) {

        //ugh
    }

    while ((retValue = SSL_read_ex(ssl, fullResponse, fullResponseSize - 1, &bytesReceived)) <= 0) {

        if (numFails > 3) {
            fprintf(stderr, "sprinkler_tls_recv failed\n");
            exit(1);
        }

        if (verbose == 1) {
            err = SSL_get_error(ssl,retValue);
            if (err == SSL_ERROR_ZERO_RETURN) { //Peer closed connection
                printf("[VERBOSE] sprinkler_tls_recv: server closed connection. Reconnecting...\n");  
            } else {
                printf("[VERBOSE] sprinkler_tls_recv: %s\n", ERR_error_string(err, NULL));
            }
        }

        SSL_free(ssl); close(sock);
        sprinkler_connect(servinfo);
        ssl = sprinkler_tls_connect(ctx);
        sprinkler_tls_send(ssl);
        numFails++;
    }

    fullResponse[bytesReceived] = '\0';
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

