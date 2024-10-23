## 10/21/24

Make it exit the while loop when it found the right password
Set optind to 1 before while loop starts

How getopt works - if a non-option is found...



No need for the success flag to break out of the while loop when correct password is found


Started exploring how Hydra performs basic auth
- network programming: https://www.geeksforgeeks.org/socket-programming-cc/
- 

## 10/22/24

Added an if chain to check command line validity
Saved all error msgs to a string and print to stderr after all command line related errors are detected
Moved all things ssh to thing-ssh.c

Why I decided to open the usr and pass files right away - no need to duplicate error checking code.