# <code>Sprinkler v1.0</code>

[**What's Sprinkler?**](#whats-sprinkler)

[**Installation**](#installation)\
-->[Installing required libraries](#installing-required-third-party-libraries)\
-->[Installing Sprinkler](#installing-sprinkler)

[**Setting up a target server**](#setting-up-a-target-server)

[**How to use**](#how-to-use)\
-->[Printing help pages](#printing-help-pages)\
-->[Basic syntax for password spraying](#basic-syntax-for-password-spraying)\
-->[Extra options](#extra-options)\
-->[Examples](#examples)


## <code>What's Sprinkler?</code>

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

## <code>Setting up a target server</code>

If you'd like to set up your own target server to test Sprinkler's performance, here's the instructions!

First, download an Ubuntu virtual machine from https://ubuntu.com/download/server#manual-install, which will be our ssh target as well as hosting an Apache2 server we can target with http-get basic authentication and http-post authentication.

After installing and running the Ubuntu VM, install requirements with the following commands:

    sudo apt install ssh

    sudo apt install apache2

    sudo apt install lib apache2-mod-php

Then, configure ssh's settings by editing the file /etc/ssh/sshd_config to say:

    PasswordAuthentication yes

Start SSH with:

    sudo systemctl start ssh

SSH is now running and a valid login target.

Go into /etc/apache2/apache2.conf and at the top add the following code:

    <Directory "/var/www/html">
    AuthType Basic
    AuthName "Restricted Content"
    AuthUserFile /etc/apache2/.htpasswd
    Require valid-user
    </Directory>

Set up a password by running the following, replacing the username as you see fit

    sudo htpasswd /etc/apache2/.htpasswd username

You will be prompted for a password, chooses one to to go with that username. You will need the two to login to your server.

Start Apache2 with:

    sudo systemctl start apache2

Now when you open up the website you will be prompted to login via basic auth.

To setup http-post authentication, reopen /etc/apache2/apache2.conf and comment out your addition to the top.

Create a .php file in /var/www/html/ with the following code:

    <html>
	    <body>
		    <form method=”POST”>
			    <input type=”text” name=”name”>
			    <input type=”psw” name=”psw”
			    <input type=”submit”>
		    </form>
	    </body>
    </html>
    <?php
	    $name=$_POST[‘name’];
	    $psw=$_POST[‘psw’];
	    if ($name == “ubuntu” and $psw == “ubuntu”) {
		    ob_clean();
    		header(‘Location: https://zapatopi.net/treeoctopus/’, true, 302);
	    	die();
	    }
	    else {
	    	echo “Sorry, Invalid login.”;
	    }
    ?>

Restart your Apache2 service like so:

    sudo systemctl restart apache2

Now when you open up the .php page, you will have a form to fill out to login that is a valid target for Sprinkler's http-post method.

To attack these targets, you will need the ip address of your ubuntu virtual machine, which can be found by executing:

    ip a

As well as the ports. Typically ssh will be on port 22 and the websites on port 80.


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

    -S                  (For http-get, http-post) Connect via SSL/TLS 
    -r REGEX            (For http-get, http-post) Supply a regex (POSIX extended regex) to determine a login success (S=) or failure (F=) if found in target's response.

                        Examples:
                            -r 'S=welcome to'
                            -r 'F=/login/[^\r\n]*\r\n\r\n'

    -i PARAMETERS       (For http-post) Supply parameter names and values that go in the request body, using the following syntax:
                            'param1=value1&param2=value2&...'
                        If a parameter corresponds to the username, set value to ^USER^
                        If a parameter corresponds to the password, set value to ^PASS^

                        If this option is not set, then sprinkler proceeds though the following steps:
                            1. Send a GET request to the server to get the login form
                            2. Parse the login form to get all input parameter names (and values if applicable)
                            3. Simultaneously, check if the parameter name suggests a username, or password, or something else:
                                - if the "name" field contains the regex "usr|user|email|.*name" then that parameter name corresponds to the username
                                - if the "name" field contains the regex "pass|pw|psw|ps" then that parameter name corresponds to the password
                                - otherwise, save the "name" and "value" field and add it to the login request
                        
                        Examples:
                            -i 'email=^USER^&passwd=^PASS^&session=12345'
                            -i 'username=^USER^&pass=^PASS^&theme=&session='

 To find out if a service support these options, and how to use them, type this command:

    sprinkler -h SERVICE

## <code>**Examples**</code>

To an SSH server

    sprinkler -U user5.txt -P pass5.txt -s 22 127.0.0.1 ssh

To a HTTP basic auth server

    sprinkler -U usernames.txt -P pass.txt -s 80 127.0.0.1/myDir http-get

To a HTTPS server that uses form-based authentication

    sprinkler -U user.txt -P pass.txt -s 443 -S 127.0.0.1 http-post

Same, but we already know form variable names

    sprinkler -U user.txt -P pass.txt -s 443 -S -i 'email=^USER^&passwd=^PASS^&region=whatever' 127.0.0.1 http-post



