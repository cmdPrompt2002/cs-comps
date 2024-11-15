#ifndef _THINGHTTP
#define _THINGHTTP

#include <regex.h>
#include <openssl/ssl.h>

extern char *service;
extern char *destination;
extern int port;
extern char *usrFilename;
extern char *passFilename;

int http_main(char *usr, char *pass, FILE *usrFile, FILE *passFile, int tls, regex_t *checkStr, int responseCheck);
int sprinkler_connect(struct addrinfo *servinfo);
SSL *sprinkler_tls_connect(SSL_CTX *ctx);

int http_get_init();
int http_post_init(int tls);

int http_attempt(char *usr, char *pass, int tls, regex_t *checkStr, int responseCheck);
int http_get_attempt(char *usr, char*pass, int tls, regex_t *checkStr, int responseCheck);
int http_post_attempt(char *usr, char *pass, int tls, regex_t *checkStr, int responseCheck);

void sprinkler_send();
void sprinkler_tls_send(SSL *ssl);
char *sprinkler_recv(int attempt, char *fullResponse, int fullResponseSize);
void sprinkler_recv_2(char *fullResponse, int fullResponseSize, int receiveAll);
void sprinkler_tls_recv(char *fullResponse, int fullResponseSize,int receiveAll);

char *to_base64(const unsigned char *data, size_t input_length, char *encoded_data);


#endif