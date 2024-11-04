#ifndef _THINGSSH
#define _THINGSSH
#include <libssh/libssh.h>

extern char *service;
extern char *destination;
extern int port;
extern char *usr;
extern char *usrFilename;
extern char *pass;
extern char *passFilename;
extern FILE *usrFile;
extern FILE *passFile;
extern int threads;

void ssh_main();
int sshAttempt(char* destination, char* username, char* password, ssh_session my_ssh);
void sshOutput(int attemptStatus, ssh_session my_ssh);


#endif