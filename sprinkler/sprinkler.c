#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libssh/libssh.h>
#include <pthread.h>
#include "sprinkler-http.h"
#include "sprinkler-ssh.h"

//Example command: ./thing -U usernames.txt -P passwords.txt -s 22 ssh 192.168.64.4

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
int verbose = 0; //-v or -V
float delay = 0; //-d
int threads = 0;
int tls = 0; //-S
regex_t *checkStr = NULL; //-r
int responseCheck = -1;
char *inputParam = NULL; //-i


//For printing out help message when user typed 'sprinkler -h [service | NULL]'
void print_help_msg(char *service);

/*Command line parsing*/
int main(int argc, char *argv[]) {
    //Flag for printing error messages
    int err = 0;
    char *errMsg = malloc(sizeof(char)*500);
    strcpy(errMsg, "Invalid usage:\n");

    /*Expected input:

    NAME
        sprinkler - a very fast password sprayer that works for ssh, http-get (basic auth), and http-post

    SYNOPSIS
        sprinkler [-u USERNAMES | -U FILE] [-p PASSWORD | -P FILE] [-s PORT] SERVICE TARGET
        sprinkler [-h SERVICE | -h]
    
    DESCRIPTION
        sprinkler is a fast password sprayer that supports the following services:
            ssh, http-get, http-post

        This tool allows researchers and pen-testers to see how easy it would be to gain unauthorized access to a remote server.

    MANDATORY OPTIONS
        TARGET      The server (and directory, if applicable) to attack, can be an IPv4 address or domain name
        SERVICE     The target's login service. Type 'sprinkler -h services' to list the protocols available
        
        -s PORT     
            the target server's port
        -u USERNAME
            supply a login username, or -U FILE to supply a username file
        -p PASSWORD
            supply a password, or -P FILE to supply a password file

    EXTRA OPTIONS (type 'w' to see if applicable to service)
        -S 
            connect via SSL
        -d DELAY
            set a delay time (in seconds) between each login attempt
        -v / -V
            Verbose mode. -V prints out all login attempts.
        -r REGEX
            Supply a regex (POSIX extended regex) to determine a login success (S=) or failure (F=) if found in target's response
            Examples:
                -r 'S=welcome to'
                -r 'F=/login/[^\r\n]*\r\n\r\n'
        -i PARAMETERS
            Supply parameter names and values with the following syntax:
                'param1=value1&param2=value2&...'
            If a parameter corresponds to the username, set value to ^USER^
            If a parameter corresponds to the password, set value to ^PASS^
            Example:
                -i 'email=^USER^&passwd=^PASS^&session=12345'
    */
    
    //Command line parsing
    int opt;
    // printf("optind: %i, arg: %s\n", optind, argv[optind]);
    while (optind < argc) {
        if ((opt = getopt(argc, argv, ":h:s:d:u:p:U:P:vVtSi:r:")) != -1) {
            // printf("optind: %i, arg: %s\n", optind, argv[optind]);
            switch (opt) {
                case 'h':
                    service = optarg;
                    print_help_msg(service);
                    exit(0);
                    break;
                case 's':
                    port = atoi(optarg);
                    if (port <= 0) {
                        sprintf(errMsg + strlen(errMsg), "    Invalid port number: %s\n",optarg);
                        err = 1;
                    }
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
                case 'V':
                    verbose = 2;
                    break;
                case 'd':
                    delay = strtod(optarg, NULL);
                    if (delay <= 0) {
                        sprintf(errMsg + strlen(errMsg), "    Invalid delay number: %s\n",optarg);
                        err = 1;
                    }
                    break;
                case 't':
                    threads = 1;
                    break;
                case 'S':
                    tls = 1;
                    break;
                case 'r':
                    checkStr = (regex_t *) malloc(sizeof(regex_t));
                    if (!strncmp(optarg,"S=",2))
                        responseCheck = 1;
                    else if (!strncmp(optarg,"F=",2))
                        responseCheck = 0;
                    else {
                        sprintf(errMsg + strlen(errMsg), "    Invalid usage for option -r\n");
                        err = 1;
                    }
                    
                    if (regcomp(checkStr, optarg + 2, REG_EXTENDED) == -1) {
                        sprintf(errMsg + strlen(errMsg), "    Fails to compile regex for option -r\n");
                        err = 1;
                    }
                    break;
                case 'i':
                    inputParam = malloc(strlen(optarg) + 1);
                    strcpy(inputParam, optarg);
                    break;
                case ':':
                    if (optopt == 'h') {
                        print_help_msg(NULL);
                        exit(0);
                    }
                    sprintf(errMsg + strlen(errMsg),"    Option -%c requires an argument\n", optopt);
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
            int args = 1;
            for (; optind < argc; optind++) {
                if (args > 2) {
                    sprintf(errMsg + strlen(errMsg),"    too many arguments\n");
                    err = 1;
                    break;
                }
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
                args++;
            }

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
        sprintf(errMsg + strlen(errMsg), "type './sprinkler -h' for correct usage\n");
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

    

    printf("\n---IT'S SPRAYIN TIME---\n\n");
    printf("Destination:%s\nPort:%i\nService:%s\n\n", destination,port,service);

    /*===Services===*/
    if (!strcmp(service, "ssh")) {ssh_main(usr,pass,usrFile,passFile);}
    else if (!strncmp(service, "http",4)) {http_main(usr,pass,usrFile,passFile,tls, checkStr, responseCheck, inputParam);}
    return 0;
}

void print_help_msg(char *service) {
    if (service == NULL) {
        //Standard man page

    } else if (!strncmp(service, "ssh",3)) {
        printf("This service has no further options\n");
    } else if (!strncmp(service, "http-get",8)) {

    } else if (!strncmp(service, "http-post",9)) {

    } else {
        printf("Service unknown or not supported: %s\n", service);
        exit(1);
    }
}