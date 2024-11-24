# <code>Sprinkler v1.0</code>

[**About**](#about)

[**Installation**](#installation)\
    - [Installing required libraries](#installing-required-third-party-libraries)\
-->[Installing Sprinkler](#installing-sprinkler)

[**How to use**](#how-to-use)\
-->[Printing help pages](#printing-help-pages)\
-->[Basic syntax for password spraying](#basic-syntax-for-password-spraying)\
-->[Extra options](#extra-options)\
-->[Examples](#examples)


## <code>About</code>

Sprinkler is a password sprayer developed by Prompt Eua-anant and Sam Ederington for Carleton College CS Comps project (Fall 2024). Written in C, it utilizes several third-party libraries to spray login credentials to some remote server. The following services are supported by Sprinkler: ssh, http-get, http-post. The available services include ssh, http-get, http-post.

What is password spraying? 
- Shotgunning a bunch of login credentials to a remote server, seeing which ones grant you access


For more information, visit [https://github.com/cmdPrompt2002/cs-comps/tree/main/sprinkler/writeup.pdf]

## <code>Installation</code>

### Installing required third-party libraries

Sprinkler requires serveral third-party libraries. To install them, execute the following command in your terminal according to your machine's OS

Linux:

    sudo apt-get install libssl-dev libssh-dev

MacOS:

1. You must download the libssl and libssh zip files from their websites. DONT install from brew.

    - libssl: https://openssl-library.org/
    - lilbssh: https://www.libssh.org/
2. Once you've downloaded the zip files for each, following the instructions in README and INSTALL to completion.

### Installing Sprinkler

Once you have installed all the required third-party libraries, follow the instructions below.

Linux:

    make
    sudo make install

MacOS:

1. Open the Makefile and set the LIBSSH variable to  '-rpath PATH' where PATH is the absolute path to the build/lib folder containing the libssh .dylib files.

2. Execute the following:

        make
        sudo make install


## <code>How to use</code>

### Printing help pages

To print out standard help page or additional options available for a service, type the following commands:

    sprinkler -h

    sprinkler -h SERVICE


### Basic syntax for password spraying:

    sprinkler -u user1 -p pass1 -s 80 TARGET SERVICE

Required inputs:

Option        | Description 
------------- | ------------- 
-u or -U      | -u for inputting a single username. -U for inputting a file with one username per line. 
-p or -P      | Same as above, but for passwords  
-s            | Port number of the target server

Argument        | Description 
------------- | -------------
TARGET        | The IP address or domain name, and directory (if applicable) to spray login credentials to. 
SERVICE       | The login service used by the TARGET. Currently, Sprinkler supports three services: ssh, http-get, and http-post  

NOTE: all arguments must come AFTER the options


Examples:

    sprinkler -u user1 -P passwords.txt -s 80 mywebsite.com/loginPages/basicAuth http-get

    sprinkler -U user5.txt -P pass5.txt -s 22 127.0.0.1 ssh

    sprinkler -u user -p pass -s 80 127.0.0.1/login http-post

### Extra options

**Global** options:

    -d DELAY            set a delay time in seconds between each login attempt
    -v                  Verbose mode. Prints out status reports eg. if target server closes the connection
    -V                  Extra verbose mode. Prints out all success/failure messages for all login credentials sent to the server

**Service-specific** options:

    -S                  Connect via SSL/TLS
    -r REGEX            Supply a regex to determine login success/failure based on whether the REGEX is found in TARGET's response
    -i PARAMETERS       Supply parameter names and values (if applicable) to send to TARGET

 To find out if a service support these options, and how to use them, type this command:

    sprinkler -h SERVICE

## <code>**Examples**</code>

Basic password spraying to an SSH server

    sprinkler -U user5.txt -P pass5.txt -s 22 127.0.0.1 ssh

Spraying to an HTTP basic auth server with a delay of 1.5 seconds between attempts 

    sprinkler -U usernames.txt -P pass.txt -s 80 127.0.0.1 http-get

Spraying to an HTTPS server that uses form-based authentication

    sprinkler -U user.txt -P pass.txt -s 443 -S 127.0.0.1 http-post

Same as above, but with form parameters specified

    sprinkler -U user.txt -P pass.txt -s 443 -S -i 'email=^USER^&passwd=^PASS^' 127.0.0.1 http-post