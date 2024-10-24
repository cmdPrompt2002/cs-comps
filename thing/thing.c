#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libssh/libssh.h>
#include "thing-http.h"
#include "thing-ssh.h"

//Example command: ./thing ssh bandit.labs.overthewire.org -u bandit0 -P passwords.txt -s 2220

// int sshAttempt(char* destination, char* username, char* password, ssh_session my_ssh);
// void sshOutput(int attemptStatus, ssh_session my_ssh);

//Global variables
char *service;
char *destination;
int port = -1;
char *usr = NULL;
char *usrFilename = NULL;
char *pass = NULL;
char *passFilename = NULL;
FILE *usrFile;
FILE *passFile;
int verbose = 0; //Three levels? 0,1,2
int delay = 0;

//Candidate names: passwordSquirt, sprinkler, octopus, passqrt, squirt, passquirt, brrr, sqwerty, 
/*Command line parsing*/
int main(int argc, char *argv[]) {
    
    //Flag for printing error messages
    int err = 0;
    char *errMsg = malloc(sizeof(char)*500);
    strcpy(errMsg, "Invalid usage:\n");
    
    usr = malloc(256*sizeof(char));
    pass = malloc(256*sizeof(char));
    
    /*Expected input:
        name service destination [-s PORT] [-u USERNAMES] [-p PASSWORD] {more stuff later}
        bandit.labs.overthewire.org
    */
    
    //Command line parsing
    int opt;
    printf("optind: %i, arg: %s\n", optind, argv[optind]);
    while (optind < argc) {
        if ((opt = getopt(argc, argv, ":s:u:p:U:P:v:d:")) != -1) {
            printf("optind: %i, arg: %s\n", optind, argv[optind]);
            switch (opt) {
                case 's':
                    port = atoi(optarg);
                    break;
                case 'u':
                    usr = optarg;
                    break;
                case 'U':
                    usrFilename = optarg;
                    usrFile = fopen(usrFilename, "r");
                    if (usrFile == NULL) {
                        sprintf(errMsg + strlen(errMsg), "    File not in directory: %s\n",usrFilename);
                        err = 1;
                    }
                    break;
                case 'p':
                    pass = optarg;
                    break;
                case 'P':
                    passFilename = optarg;
                    passFile = fopen(passFilename, "r");
                    if (passFile == NULL) {
                        sprintf(errMsg + strlen(errMsg), "    File not in directory: %s\n",passFilename);
                        err = 1;
                    }
                    break;
                case 'v':
                    verbose = 1;
                    break;
                case 'd':
                    delay = 1;
                    break;
                case ':':
                    sprintf(errMsg,"    Option -%c requires an argument\n", optopt);
                    err = 1;
                    break;
                case '?':
                    sprintf(errMsg + strlen(errMsg),"    Unknown option -%c\n", optopt);
                    err = 1;
                    break;
                default:
                    printf("default\n");
                    break;
            }
        
        } else {
            for (; optind < argc; optind++) {
                if (!strncmp(argv[optind], "ssh", 4)) {
                    service = "ssh";
                } else if (!strncmp(argv[optind], "http-get", 8)) {
                    service = "http-get";
                } else {
                    destination = argv[optind];         
                }
            }
            // if (!strncmp(argv[optind], "ssh", 4)) {
            //         service = "ssh";
            //     } else if (!strncmp(argv[optind], "http-get", 8)) {
            //         service = "http-get";
            //     } else {
            //         destination = argv[optind];         
            //     }
            // optind++;

        }
    }

    //Check for valid command line input
    if (service == NULL) {
        sprintf(errMsg + strlen(errMsg), "    Service unidentified. Available services: ssh, http-get\n");
        err = 1;
    } if (destination == NULL) {
        sprintf(errMsg + strlen(errMsg), "    Destination is missing\n");
        err = 1;
    } if (strlen(pass) == 0 && passFilename == NULL) {
        sprintf(errMsg + strlen(errMsg), "    Password or password file is missing\n");
        err = 1;
    } if (strlen(usr) == 0 && usrFilename == NULL) {
        sprintf(errMsg + strlen(errMsg), "    Username or username file is missing\n");
        err = 1;
    } if (port == -1) {
        sprintf(errMsg + strlen(errMsg), "    Destination port is missing\n");
        err = 1; 
    } if (err == 1) {
        sprintf(errMsg + strlen(errMsg), "type './thing --h' for correct usage\n");
        fprintf(stderr, "%s", errMsg);
        exit(1);
    }
    free(errMsg);

    // while (optind < argc) {
    //     int save = optind;
    //     opt = getopt(argc, argv, ":s:u:U:p:P:");
        
    //     printf("optind: %i, option: %s\n", optind, argv[optind]);
        
    //     switch (opt) {
    //         case 's':
    //             port = atoi(optarg);
    //             break;
    //         case 'u':
    //             usr = optarg;
    //             break;
    //         case 'U':
    //             usrFilename = optarg;
    //             break;
    //         case 'p':
    //             pass = optarg;
    //             break;
    //         case 'P':
    //             passFilename = optarg;
    //             break;
    //         case ':':
    //             printf("Option -%c requires an argument\n", optopt);
    //             exit(1);
    //             break;
    //         case '?':
    //             printf("Unknown option -%c", optopt);
    //             exit(1);
    //             break;
    //         case -1:
    //             //A non-option argument: must be either destination or service
    //             if (!strncmp(argv[optind], "ssh", 4)) {
    //                 service = "ssh";
    //             } else {
    //                 destination = argv[optind];
    //             }
    //             optind++;
    //             break;
    //         default:
    //             exit(1);
    //             break;

    //     }
    // }

    // printf("Passed command line checking\n");

    printf("\n---IT'S SPRAYIN TIME---\n\n");
    printf("Destination:%s\nPort:%i\nService:%s\n\n", destination,port,service);

    /*===Services===*/
    if (!strcmp(service, "ssh")) {ssh_main();}
    else if (!strcmp(service, "http-get")) {printf("http!!\n");}
    
    return 0;
}

/*
Input:
    - destination: IP address or hostname to attack
    - username and password for SSH login
    - my_ssh handler
Output: int attemptStatus. Tells whether the auth is sucessful.
*/
// int sshAttempt(char* destination, char* username, char* password, ssh_session my_ssh) {
//     //Configure the options before making an SSH connection
//     ssh_options_set(my_ssh, SSH_OPTIONS_HOST, destination);
//     ssh_options_set(my_ssh, SSH_OPTIONS_PORT, &port);
//     ssh_options_set(my_ssh, SSH_OPTIONS_USER, usr);
    
//     //Connect to SSH server
//     int rc = ssh_connect(my_ssh);
//     if (rc != SSH_OK) {
//         fprintf(stderr, "Error connecting to host: %s\n",
//             ssh_get_error(my_ssh));
//         ssh_free(my_ssh);
//         exit(-1);
//     }

//     //printf("Successful connection\n");
//     //printf("User:%s, Pass:%s\n", usr, pass);
//     int attemptStatus = ssh_userauth_password(my_ssh, NULL, pass);
//     sshOutput(attemptStatus, my_ssh);
//     return attemptStatus;
// }

// void sshOutput(int attemptStatus, ssh_session my_ssh) {
//     if (attemptStatus == SSH_AUTH_SUCCESS) {
//         printf("\033[0;32mAuthentication successful:\033[0m || Destination:%s || Username:%s || Password:%s\n", destination, usr, pass);
//         ssh_disconnect(my_ssh);
//     } else if(attemptStatus == SSH_AUTH_DENIED) {
//         printf("\033[0;31mAuthentication failure:\033[0m Username:%s, Password:%s\n", usr, pass);
//         ssh_disconnect(my_ssh);
//     } else {
//         fprintf(stderr, "Error authenticating with password: %s\n",
//         ssh_get_error(my_ssh));
//         ssh_disconnect(my_ssh);
//     }
    
//     ssh_free(my_ssh); //Do we really wanna free and allocate new ssh_session every time?

//     return;
// }