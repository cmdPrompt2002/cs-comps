#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libssh/libssh.h>
#include "thing-http.h"
#include "thing-ssh.h"
#include <pthread.h>


//Example command: ./thing -U usernames.txt -P passwords.txt -s 2220 ssh bandit.labs.overthewire.org

//Global variables
char *service;
char *destination;
int port = -1;
char usr[256];
char pass[256];
FILE *usrFile;
FILE *passFile;
char *usrFilename = NULL;
char *passFilename = NULL;

//Extra options
int verbose = 0; //Three levels? 0,1,2
int delay = 0;
int threads = 0;
int tls = 0;

//Candidate names: Decided on Sprinkler
/*Command line parsing*/
int main(int argc, char *argv[]) {
    //Flag for printing error messages
    int err = 0;
    char *errMsg = malloc(sizeof(char)*500);
    strcpy(errMsg, "Invalid usage:\n");

    /*Expected input:
        sprinkler [-u USERNAMES | -U FILE] [-p PASSWORD | -P FILE] [-s PORT] service target

        Mandatory options
            target      a target to attack, can be an IPv4 address or DNS name
            service     the service to attack. Type 'sprinkler -h services' to list the protocols available
            
            -s PORT     
                if the service is on a different default port, define it here
            -u USERNAME
                supply a login username, or -U FILE to supply a username list
            -p PASSWORD
                supply a password, or -P FILE to supply a password list

        Extra options
            -S connect via SSL
            -d DELAY
                set a delay time between each login attempt
            -b NUMBYTES
                Set the size of the buffer holding the server's response per login attempt. 
                Buffer should be larger than server's response.
            -v / -V
                Verbose Prints out all 
            
    */
    
    //Command line parsing
    int opt;
    printf("optind: %i, arg: %s\n", optind, argv[optind]);
    while (optind < argc) {
        if ((opt = getopt(argc, argv, ":s:u:p:U:P:v:d:t:S")) != -1) {
            printf("optind: %i, arg: %s\n", optind, argv[optind]);
            switch (opt) {
                case 's':
                    port = atoi(optarg);
                    break;
                case 'u':
                    strncpy(usr,optarg,256);
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
                    strncpy(pass,optarg,256);
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
                case 't':
                    threads = 1;
                    break;
                case 'S':
                    tls = 1;
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
                if (!strncmp(argv[optind], "ssh", 3)) {
                    service = "ssh";
                } else if (!strncmp(argv[optind], "http-get", 8)) {
                    service = malloc(sizeof(char)*9);
                    strcpy(service, "http-get");
                } else if (!strncmp(argv[optind], "http-post", 9)) {
                    service = malloc(sizeof(char)*10);
                    strcpy(service, "http-post");
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

    //Get module specific options
    int responseCheck = -1; //1 = failCond, 0 = successCond
    regex_t *checkStr = NULL;
    char *startParam = strstr(destination,"::"); 
    if (startParam != NULL) {
        char *endParam = strstr(startParam+2,"::");
        startParam[0] = '\0'; //to separate the directory from user input options
        char *param = malloc(sizeof(char)*20);
        int optionLength;
        int optSize = 20;

        while (startParam != NULL) {
            startParam += 4; //points to first idx of option value
            if (endParam == NULL) { optionLength = strlen(startParam) + 1; }  
            else { optionLength = endParam - startParam; }

            if (optionLength > optSize) {
                    param = (char *) realloc(param,optionLength); //Null terminator included
                    optSize = optionLength;
                }

            if (!strncmp(startParam -2, "S=", 2)) {

                if (checkStr != NULL) {
                    fprintf(stderr, "[ERROR] Can only accept either success condtion S failure condition F or neither\n");
                    exit(1);
                } 
                
                checkStr = (regex_t *) malloc(sizeof(regex_t));
                strncpy(param,startParam,optionLength);
                printf("successCond: %s\n",param);
                if (regcomp(checkStr, param, REG_EXTENDED) == -1) {
                    fprintf(stderr, "Fails to compile successCond regex\n");
                    exit(1);
                }
                responseCheck = 1;
            } else if (!strncmp(startParam -2, "F=", 2)) {

                if (checkStr != NULL) {
                    fprintf(stderr, "[ERROR] Can only accept either success condtion S failure condition F or neither\n");
                    exit(1);
                } 

                checkStr = (regex_t *) malloc(sizeof(regex_t));
                strncpy(param,startParam,optionLength);
                printf("failCond: %s\n",param);
                if (regcomp(checkStr, param, REG_EXTENDED) == -1) {
                    fprintf(stderr, "Fails to compile failCond regex\n");
                    exit(1);
                }

                responseCheck = 0;

            } else {
                *(startParam -1) = '\0' ;
                fprintf(stderr, "Unrecognized parameter: %s\nType 'sprinkler -U [service]' for correct usage\n",startParam-2);
                exit(1);
            }

            startParam = strstr(startParam, "::");
            if (startParam != NULL) {
                endParam = strstr(startParam+2, "::");
            }

        }
        free(param);
    }

    printf("\n---IT'S SPRAYIN TIME---\n\n");
    printf("Destination:%s\nPort:%i\nService:%s\n\n", destination,port,service);

    /*===Services===*/
    if (!strcmp(service, "ssh")) {ssh_main(usr,pass,usrFile,passFile);}
    else if (!strncmp(service, "http",4)) {http_main(usr,pass,usrFile,passFile,tls, checkStr, responseCheck);}
    return 0;
}