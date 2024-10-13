#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libssh/libssh.h>

int sshAttempt(char* destination, char* username, char* password, ssh_session myssh);

//Global variables
char *service;
char *destination;
int port;
char *usr;
char *usrFilename;
char *pass;
char *passFilename;
FILE *usrFile;
FILE *passFile;

//Candidate names: passwordSquirt, sprinkler, octopus, passqrt, squirt, passquirt, 
/*Command line parsing*/
int main(int argc, char *argv[]) {
    int c;
    

    /*Expected input:
        name service destination [-s PORT] [-u USERNAMES] [-p PASSWORD] {more stuff later}
        bandit.labs.overthewire.org
    */
    

    /*Command line parsing*/
    //printf("optind: %i, arg: %s\n", optind, argv[optind]);
    while (optind < argc) {
        c = getopt(argc, argv, ":s:u:U:p:P:");
        // printf("optind: %i, arg: %s\n", optind, argv[optind]);
        switch (c) {
            case 's':
                port = atoi(optarg);
                break;
            case 'u':
                usr = optarg;
                break;
            case 'U':
                usrFilename = optarg;
                usrFile = fopen(usrFilename, "r");
                break;
            case 'p':
                pass = optarg;
                break;
            case 'P':
                passFilename = optarg;
                passFile = fopen(usrFilename, "r");
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
                    service = "ssh";
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
        ssh_session my_ssh = ssh_new();
        int attemptStatus = sshAttempt(destination, usr, pass, my_ssh);
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
    }
    
    return 0;
}

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

    //Spray passwords
    return ssh_userauth_password(my_ssh, NULL, pass);
    
}
