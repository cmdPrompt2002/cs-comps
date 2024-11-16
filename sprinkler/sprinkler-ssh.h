#ifndef _SPRINKLERSSH
#define _SPRINKLERSSH
#include <libssh/libssh.h>

extern char *service;
extern char *destination;
extern int port;

extern char *usrFilename;
extern char *passFilename;

extern int threads;

void ssh_main(char *usr, char *pass, FILE *usrFile, FILE *passFile);
int sshAttempt(char* destination, char* username, char* password, ssh_session my_ssh);
void sshOutput(int attemptStatus, ssh_session my_ssh, char *usr, char *pass);


#endif