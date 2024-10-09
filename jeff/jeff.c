#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libssh/libssh.h>

int main(int argc, char *argv[]) {
    
    /*===Services===*/

    //SSH
    if (!strcmp(argv[1], "ssh")) {
        int port = 2220;

        //Configure the options before making an SSH connection
        ssh_session my_ssh = ssh_new();
        ssh_options_set(my_ssh, SSH_OPTIONS_HOST, "bandit.labs.overthewire.org");
        ssh_options_set(my_ssh, SSH_OPTIONS_PORT, &port);
        ssh_options_set(my_ssh, SSH_OPTIONS_USER, "bandit0");
        
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
       
        rc = ssh_userauth_password(my_ssh, NULL, "bandit0");
        if (rc != SSH_AUTH_SUCCESS) {
            fprintf(stderr, "Error authenticating with password: %s\n",
                    ssh_get_error(my_ssh));
            ssh_disconnect(my_ssh);
            ssh_free(my_ssh);
            exit(-1);
        } else {
            printf("Authentication successful. Username:%s, Password:%s\n", "bandit0", "bandit0");
            ssh_disconnect(my_ssh);
            ssh_free(my_ssh);
        }


        
    }
    
    return 0;
}

