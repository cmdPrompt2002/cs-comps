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

TODO - try to reuse the same socket --> Success!

## 10/28/24

Try http-POST: see FDF wireshark 

Poster advices from Jeff
- Sometimes don't need complete sentences. It's easier to read
- 

## 10/29/24

POST websites
- https://en.wikipedia.org/w/index.php?title=Special:UserLogin
- Jeff's fdf

Wikipedia
- Order of input doesnt matter

POST step-by-step
1. Get html of the login page (very hard for Wikipedia, try Jeff's fdf first) DONE
2. Find the <form method = post> tag (use REG_EXTENDED flag to enable special characters ?.*+) DONE
3. Get all the non-password and non-username <input> names and values. Record the content-length!

How: 


4. Repeat 2 and 3
5. Call http_post_attempt, which supplies username and password into the request body. Add the length of password and username to the content-length header

send request 

TODO
- Determine if website uses HTTP 1.1 or 2.0
- NTLM, DIgest based authentication: 
    Overview: https://www.httpwatch.com/httpgallery/authentication/

## 10/30/24
Formtag_end is right!
BUT formendtag_start is wrong :/
Fixed! - the match.rm_so starts from authdetails + formtag_end, not from beginning of authDetails

All index vars track from the beginning of authDetails! Checked!

TODO: malloc name and value at declaration, and realloc if valuelength exceeds current size
TODO: Also grab the placeholder value and search for email or password. Right now it only searches the name, not the placeholder.

TODO: processs the server's set-cookie header, send a GET request with that cookie, and read the html response for "                   Please check your login details and try again"


inits--> http_attempt --> (http_get_attempt OR http_post_attempt) --> sprinkler_send --> process response

TODO: error check for multiple services entered. For example, if user enters '... http-post ssh' then it should exit w/ error msg

DOTHISFIRST: use regex to find content-length, and use this to determine whether the sprinkler_recv function should enter the while loop in the first place. DONE

Poster session pitch idea
- Attention grabbing
- What could it be used for/what problem it's tryna solve
- Most interesting issues that came up???
- Why is this important to them?
-
- Issue: how to quickly check which combo of usr and pass is correct
- Why it's important: make sure that server is not vulnerable to this kind of attack
- How did we proceed: set up web servers, make password spraying tool 

No demo at poster session, so don't have to freeze code.

## 11/02/24

How to parse the extra options attached to the destination
- Can't strtok - the regcomp function accepts a char *pointer*, so need to copy just the option value and assign a new char *
- Need strstr, and strncpy

TODO: base64 function - malloc once, keep track of size, if size of new one exceed allocated memory, then realloc