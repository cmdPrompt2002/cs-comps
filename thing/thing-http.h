#ifndef _THINGHTTP
#define _THINGHTTP

extern char *service;
extern char *destination;
extern int port;
extern char *usr;
extern char *usrFilename;
extern char *pass;
extern char *passFilename;
extern FILE *usrFile;
extern FILE *passFile;

int http_main();
int http_attempt(char *usr, char *pass, char* service);
void http_output();
char* to_base64(char *str);


#endif