#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libssh/libssh.h>

//Example command: ./thing ssh bandit.labs.overthewire.org -u bandit0 -P passwords.txt -s 2220

void sshAttempt(char* destination, char* username, char* password, ssh_session my_ssh);
void sshOutput(int attemptStatus, ssh_session my_ssh);

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
    pass = malloc(256*sizeof(char));
    
    

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
                usr = optarg;
                // strncpy(usr, optarg, 256);
                // usr[255] = '\0';
                break;
            case 'U':
                usrFilename = optarg;
                break;
            case 'p':
                pass = optarg;
                // strncpy(pass, optarg, 256);
                // pass[255] = '\0';
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
                    service = "ssh";
                } else {
                    destination = argv[optind];
                }
                optind++;
                break;

        }
    }

    //Error checking


    printf("Passed command line checking\n");
    /*===Services===*/
    int success = 0;
    if (!strcmp(service, "ssh")) {
        //Create a new ssh_session handler
        

        if (usrFilename != NULL && passFilename != NULL) {
            usrFile = fopen(usrFilename, "r");
            passFile = fopen(passFilename, "r");
            while (fgets(usr, 256, usrFile) != NULL) {
                while (fgets(pass, 256, passFile) != NULL) {
                    ssh_session my_ssh = ssh_new();
                    sshAttempt(destination, usr, pass, my_ssh);   
                }
                rewind(passFile);
            }
            fclose(usrFile);
            fclose(passFile);
        } else if (usrFilename != NULL) {
            usrFile = fopen(usrFilename, "r");
            while (fgets(usr, 256, usrFile) != NULL) {
                ssh_session my_ssh = ssh_new();
                sshAttempt(destination, usr, pass, my_ssh);   
            }
            fclose(usrFile);
        } else if (passFilename != NULL) {
            printf("A\n");
            passFile = fopen(passFilename, "r");
            printf("B\n");
            while (fgets(pass, 256, passFile) != NULL) {
                printf("C\n");
                ssh_session my_ssh = ssh_new();
                sshAttempt(destination, usr, pass, my_ssh);   
            }
            fclose(passFile);
        } else {
            ssh_session my_ssh = ssh_new();
            sshAttempt(destination, usr, pass, my_ssh);   
        }

        // ssh_free(my_ssh);
        
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

    printf("Sucessful connection\n");
    
    printf("User:%s, Pass:%s\n", usr, pass);
    sshOutput(ssh_userauth_password(my_ssh, NULL, pass), my_ssh);
    return;
}

void sshOutput(int attemptStatus, ssh_session my_ssh) {
    if(attemptStatus == SSH_AUTH_SUCCESS) {
        printf("Authentication successful. || Destination:%s || Username:%s || Password:%s\n", destination, usr, pass);
        ssh_disconnect(my_ssh);
    } else if(attemptStatus == SSH_AUTH_DENIED) {
        printf("Authentication failure. Password:%sis not valid for user: %s\n", pass, usr);
        ssh_disconnect(my_ssh);
    } else {
        fprintf(stderr, "Error authenticating with password: %s\n",
        ssh_get_error(my_ssh));
        ssh_disconnect(my_ssh);
    }
    ssh_free(my_ssh); //Do we really wanna free and allocate new ssh_session every time?
    return;
}