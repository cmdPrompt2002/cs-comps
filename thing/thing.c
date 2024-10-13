#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libssh/libssh.h>

//Candidate names: passwordSquirt, sprinker, octopus, 
/*Command line parsing*/
int main(int argc, char *argv[]) {
    int c;
    char *service;
    int port;
    char *usr;
    char *usrFilename;
    char *pass;
    char *passFilename;
    FILE *usrFile;
    FILE *passFile;

    /*Expected input:
        name service destination [-s PORT] [-u USERNAMES] [-p PASSWORD] {more stuff later}
        bandit.labs.overthewire.org
    */
    

    /*Command line parsing*/

    //Service and destination

    //Options parsing
    while ((c = getopt(argc, argv, ":s:u:U:p:P:")) != -1) {
        printf("%d" , c);
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
        }
    }

    /*===Services===*/

    //SSH
    if (!strcmp(argv[8], "ssh")) {
        char *destination = argv[7];
        // char *username;
        // char *password;

        //Configure the options before making an SSH connection
        fprintf(stdout, "%d",port);
        ssh_session my_ssh = ssh_new();
        ssh_options_set(my_ssh, SSH_OPTIONS_HOST, destination);
        ssh_options_set(my_ssh, SSH_OPTIONS_PORT, &port);


    // if (!strcmp(argv[4], "-u")) {
    //     *username = argv[5]
    // } else if (!strcmp(argv[4], "-U")) {
    //     //Open file, put usernames into a char array
    //     FILE *fptr;

    // // Open a file in read mode
    //     fptr = fopen(argv[5], "r");
    //     username = 
    // }
        ssh_options_set(my_ssh, SSH_OPTIONS_USER, usr);
        
        //Connect to SSH server
        int rc = ssh_connect(my_ssh);
        if (rc != SSH_OK) {
            fprintf(stderr, "Error connecting to host: %s\n",
                ssh_get_error(my_ssh));
            ssh_free(my_ssh);
            exit(-1);
        }

        //Verify the server's identity (not sure if this is necessary)
        // For the source code of verify_knownhost(), check previous example
        // if (verify_knownhost(my_ssh) < 0) {
        //     ssh_disconnect(my_ssh);
        //     ssh_free(my_ssh);
        //     exit(-1);
        // }

        //Spray passwords
       
        rc = ssh_userauth_password(my_ssh, NULL, pass);
        if (rc != SSH_AUTH_SUCCESS) {
            fprintf(stderr, "Error authenticating with password: %s\n",
                ssh_get_error(my_ssh));
            ssh_disconnect(my_ssh);
            ssh_free(my_ssh);
            exit(-1);
        } else {
            printf("Authentication successful. Username:%s, Password:%s\n", usr, pass);
            ssh_disconnect(my_ssh);
            ssh_free(my_ssh);
        }


        
    }
    
    return 0;
}

