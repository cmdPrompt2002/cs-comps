#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libssh/libssh.h>
#include "funcbind.h"

int sshAttempt(char* destination, char* username, char* password, ssh_session myssh);

//Global variables
char *service;
char *destination;
int port;
char *usr = NULL;
char *usrFilename = NULL;
char *pass = NULL;
char *passFilename = NULL;
FILE *usrFile;
FILE *passFile;

//Candidate names: passwordSquirt, sprinkler, octopus, passqrt, squirt, passquirt, 
/*Command line parsing*/
int main(int argc, char *argv[]) {
    int opt;
    usr = malloc(256*sizeof(char));
    usr[0] = '\0';
    pass = malloc(256*sizeof(char));
    pass[0] = '\0';
    

    /*Expected input:
        name service destination [-s PORT] [-u USERNAMES] [-p PASSWORD] {more stuff later}
        bandit.labs.overthewire.org
    */
    

    /*Command line parsing*/
    //printf("optind: %i, arg: %s\n", optind, argv[optind]);
    while (optind < argc) {
        opt = getopt(argc, argv, ":s:u:U:p:P:");
        // printf("optind: %i, arg: %s\n", optind, argv[optind]);
        switch (opt) {
            case 's':
                port = atoi(optarg);
                break;
            case 'u':
                strncpy(usr, optarg, 256);
                usr[255] = '\0';
                break;
            case 'U':
                usrFilename = optarg;
                usrFile = fopen(usrFilename, "r");
                break;
            case 'p':
                strncpy(pass, optarg, 256);
                pass[255] = '\0';
                break;
            case 'P':
                passFilename = optarg;
                break;
            case ':':
                printf("Option -%c requires an argument\n", optopt);
                exit(1);
                break;
            case '?':
                printf("Unknown option -%c", optopt);
                exit(1);
                break;
            case -1:
                //either destination or service

                if (!strncmp(argv[optind], "ssh", 4)) {
                    service = "ssh_main";
                } else {
                    destination = argv[optind];
                }
                optind++;
                break;

        }
    }

    /*===Services===*/
    //SSH
    if (!strcmp(service, "ssh")) {
        
    
        if (usrFilename != NULL && passFilename != NULL) {
            while (fgets(usr, 256, usrFile) != NULL) 
        } else if (usrFilename != NULL) {

        } else if (passFilename != NULL) {

        } else {

        }

        if (usrFilename != NULL) {
            usrFile = fopen(usrFilename, "r"); 
            fgets(usr, 256, passFile);
            usr[255] = '\0';
        }

        while (usr != NULL) {

            if (passFileName != NULL) {
                passFile = fopen(passFilename, "r"); 
                fgets(pass, 256, passFile);
            }

            while (pass != NULL) {
                //Call sshattempt

                if (passFileName != NULL) {
                    passFile = fopen(passFilename, "r"); 
                    fgets(pass, 256, passFile);
                } else {
                    pass = NULL;
                }
            }

            if (usrFilename != NULL) {
                fgets(usr 256, passFile);
            } else {
                usr = NULL;
            }
            fclose(passFile);
        }
        fclose(usrFile)
        

        
        ssh_session my_ssh = ssh_new();
        sshAttempt(destination, usr, pass, my_ssh);   
    }
    
    return 0;
}

void sshAttempt(char* destination, char* username, char* password, ssh_session my_ssh) {
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

    sshOutput(ssh_userauth_password(my_ssh, NULL, pass));
    return;
}

void sshOutput(int attemptStattus) {
    if(attemptStatus == SSH_AUTH_SUCCESS) {
        printf("Authentication successful. || Destination:%s || Username:%s || Password:%s\n", destination, usr, pass);
        ssh_disconnect(my_ssh);
    } else if(attemptStatus == SSH_AUTH_DENIED) {
        printf("Authentication failure. Password:%s is not valid for user: %s\n", pass, usr);
        ssh_disconnect(my_ssh);
    } else {
        fprintf(stderr, "Error authenticating with password: %s\n",
        ssh_get_error(my_ssh));
        ssh_disconnect(my_ssh);
    }
    ssh_free(my_ssh);
    return;
}