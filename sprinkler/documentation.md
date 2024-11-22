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

TODO: base64 function - malloc once, keep track of size, if size of new one exceed allocated memory, then realloc. NO. USE STACK MEM INSTEAD. DONE.

## 11/04/24

http-get-attempt uses stack frame for encodedPass instead of mallocing
Fixed the recv function and http-post-init
Attempted to change global vars to local vars, and heap mem to stack mem (pass and usr) to improve performance.

Fixing recv error
- Requestbuffer NOT the issue. Printed them out. Looks fine every time.
- Connect NOT the issue
- Send NOT the issue (although the strlen(requestBuffer) doesn't return what's expected)
- Recv looks fine (maybe the issue is )

TODO: replace requestBuffer with a struct that contains char* and strlength of the array, so don't have to use strlen wither every sprintf call

Why http-post slower than get?
- regexec slower than strstr?
- More calls to global variables?


POSTER: DON'T USE URLS FOR REFERENCE. QR BETTER

Questions
- Is there any point in making it compatible with ssl? Do some websites accept TLS only?
- Possible options: TLS, restore session, focus on multithreading, other HTTP authentication protocols (proxy, ntlm)

Answers
- TLS is worth it. Some websites require https

Find where struct ssl_ctx_st is declared/defined and include that file before typs.h

Checkout gprofiler to see where my program slows

Look into libssl library initialization. Why is this a thing/ why do it at all?

ALL VARIABLES RELATED TO SENDING/RECEIVING
- requestBuffer, requestBufferLength, headersLength, responseBuffer, fullResponseBuffer, fullResponseLength

ALL VARIABLES RELATED TO REQUEST INFO
- host, dir, port

Learned about regex negative lookahead

1st PRIORITY: test whether the new recv function works, particularly the stuff about getting content legnth and line 730. 
It works

2nd PRIORITY: finish the tls_recv function and test it.
Finished it

3rd PRIORITY: test whether the recv_2 function is faster than setting the Connection: Close header and reconnect every time.
Faster than OG sprinkler_recv and closing the connection everyting

TODO: Finish the if clause in tls_recv

TIL: sending a plain HTTP request to SSL-only server results in a 400 bad request! OpenSSL is absolutely necessary

TODO: make it print out special characters in successCond/failCond

Oral exam: 

1st 5 min
- 2 min: elevator pitch
- the rest: my contributions
- can use slides to show parts of code
- Why is this important?
- How we set up test servers
- Don't need to fully understand my partner's work, but should understand rough outline

Recv
Option1: Different while loops for receiveAll = 1 or 0
Option2: If RecieveAll, then get recv once to get contentLength, and recv until all of contentlength bytes are received.

Both - get contentLength variable
ReceiveAll = 1
- realloc array if necessary
- call recv until bytesToRecv <=

ReceiveAll = 0
- no need to realloc array
- Set bytesToRead variable: if bytesToRead is less than bufSize, then do nothing. Else, set bytesToRead to bufSize - 1.

Dilemma: how to realloc array
1. Access global variable fullResponse and fullResponseSize
2. Return an array of pointers: index 0 = pointer to new array, index 1: pointer to new array size
3. Input a pointer to a struct that has pointer to array and array size. When realloc is needed, just access the struct elements. Should be preserved when function ends.


Function start

response = realloc....

return (response,responseSize)
Function ends

fullResponse = recv(....)[0]
fullResponseSize = [1]

## 11/16/24

Recv - accounted for chunked encoding - call recv (and realloc if necessary) until endBody marker (\r\n0\r\n) is found
Cases to test, each case also test for sufficiently large and small buf array
- ReceiveAll = 1 and chunked
    - Small array: worked!!
    - Large array: worked!!
- ReceiveAll = 1 and not chunked
    - Small array: worked!!
    - Large array: worked!!
- ReceiveAll = 0 and chunked
    - Small array: worked!!
    - large array: worked!!
- ReceiveAll = 0 and not chunked
    - Small array: worked!!
    - Large array: worked!!

TODO: 


Sam added delay time
Changed thing to sprinkler for all files
Changed Makefile accordingly
Added man page

- process in http-file. What's the caveat of this? 
- add -i option to input parameter names and values then add -r option to input regex 


NEXT STEPS
1. Use a vector struct for all input arrays
2. Make an options struct that stores the additional options??
3. Use valgrind to check for memory leaks
4. Use gprof to see how execution speed can be improved
5. The -i option: we want it to work for http-get too
6. 

