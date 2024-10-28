## 10/21/24

Make it exit the while loop when it found the right password
Set optind to 1 before while loop starts

How getopt works - if a non-option is found...



No need for the success flag to break out of the while loop when correct password is found

Initialize global var port to -1 for detection of invalid usage. Not 22 by default bc http-get may not use port 22

Started exploring how Hydra performs basic auth
- network programming: https://www.geeksforgeeks.org/socket-programming-cc/
- 

## 10/22/24

Added an if chain to check command line validity
Saved all error msgs to a string and print to stderr after all command line related errors are detected
Moved all things ssh to thing-ssh.c

Why I decided to open the usr and pass files right away - no need to duplicate error checking code.

For some reason, moving the ssh code into thing-ssh.c makes Sam's compiler unable to recognize libssh functions, despite having the include libssh tag in both .c and .h files.

Next steps
- Prompt focuses on HTTP basic auth
- Sam focuses on improving runtime and implementing delay

## 10/23/24

Better guide for network programming in C: https://beej.us/guide/bgnet/html/index-wide.html#getaddrinfoprepare-to-launch

What hydra does:
- Creates a header node containing (1) header string (2) value string and (3) pointer to the next node
- For each header/value pair, create a node and make it link to the previous one
- Headers created: Host,User-agent, Cookie, Content-length, Content-type, Proxy-Authorization

## 10/24/24

socket(), tobase64(), and connect() successfully :D
recv takes too long and returns nothing! Maybe I didn't send the request correctly? Yep syntax issue. Fixed!!
Auth successful!! :D Works for both single pass and user and files!

TODO - try to reuse the same socket 