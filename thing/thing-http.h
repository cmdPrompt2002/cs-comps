#ifndef _THINGHTTP
#define _THINGHTTP

#include <regex.h>

extern char *service;
extern char *destination;
extern int port;
extern char *usrFilename;
extern char *passFilename;


int http_main(char *usr, char *pass, FILE *usrFile, FILE *passFile);
int connect_to_serv(struct addrinfo *servinfo);

int http_get_init();
int http_post_init();

int http_attempt(char *usr, char *pass);
int http_get_attempt(char *usr, char *pass);
int http_post_attempt(char *usr, char *pass, regex_t *successCond, regex_t *failCond);

void sprinkler_send();
char *sprinkler_recv(int attempt, char *fullResponse, int fullResponseSize);

char *to_base64(const unsigned char *data, size_t input_length, char *encoded_data);


#endif