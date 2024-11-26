# A short description of the project. I'm thinking 1-4 sentences, but whatever works for you.

A rudimentary password spraying tool that supports Basic authentication in http-get and SSH logins, with features such as multithreading, verbose output, redirect output to file, how long it takes, when the login is successful, and accepts input wordlists (trite, should remove?). Code written in C.

# A short list of learning goals--what do you want to end up understanding that you don't understand yet?

How SSH logins work and what factors must be present in order for it to work. Related—how to use C's SSH package.
Same for the http-get basic authentication.

How to interact with a terminal from a script in C. 

How to set up a server for testing our tool.

How to input passwords into prompts from a list.

Multithreading - what it is and how to do it.

Learn more about build system - how to convert our source code files into a standalone program that's executable from terminal. We will use a makefile for this.

# A description of your project's architecture. This could be just a diagram of your expected code organization, but it could also include things like the setup of a target server, the data you're going to need for testing, a build system (e.g., Makefile or something similar), etc.

## Expected code organization
- hydra.c - parsing the command line arguments
- hydra.h

- http-get.c - handling the authentication process in http-get requests. Basic authentication is a guarantee, but we might add more features
- http-get.h

- ssh.c - handling the SSH login process
- ssh.h

-something for the build system, perhaps makefile is relevant?


## Target server setup
- Ubuntu virtual machine to host different sites from.
- Different sites with different methods of requesting authentification.

## Data for testing
- Wordlists: use Kali's rockyou.txt, and other files in /usr/share/wordlists directory.
- Time required for running through the wordlist (should be logged while our program runs).
- Log of successful and failed attempts.

# A testing and benchmarking plan.
- Using the same target server from the same client machine, attack target.
- Using the same protocol and input wordlists, compare the average speed of our program to that of Hydra's.
- Find out how to measure how much memory the program consumes.


# A more detailed schedule of development than you wrote last week. In addition to a list of development steps and proposed deadlines, you should feel free to annotate items that feel uncertain at this moment.

Set up SSH-based authentication website on openSSH, which runs on our downloaded Ubuntu server. Includes about 5 users whose username and password are common, and also another 5 users with edge case usernames and passwords. (By next Monday!)

Edge case passwords (if viable):
    -, "", >, |, <, /

Prioritize on getting the minimum viable product done first. This includes the following:
    - Able to attempt to ssh login to the server with a username from a file.
    - Able to respond to login prompts with a password from a file.
    - Able to close connection and record whether the attempt was successful or a failure.
    - Able to reattempt with another username and password combination.

To achive our goal, we will follow the following schedule:
1. set up a target server on an Ubuntu VM using openSSH, which requires ssh authentication.
2. Write a basic executable capable of attempting to login given a username to the target server.
3. Let it attempt to login with a password from a list.
4. Make it print out a log of whether the attempts (only one for now) were succesful then automatically close the connection.
5. Let it try to login to different usernames from a list with password from the list.

Next steps after this will depend on the time remaining, but we will split up and individually work towards fleshing out the program and working towards stretch goals. This will involve.
1. Configure output to be more readable.
2. Check command-line arguments for validity and give a helpful error message if invalid.
3. Introduce a delay between login attempts.
4. Make it work with other protocols, such as basic auth.
5. Improve it's runtime through various ways. 
6. Introduce multithreading to speed up its progression through the lists. 
7. Restore previous sessions that ended early due to crashes and user termination. 

# A list of things one or more of you are worried about. This could be stuff you don't understand yet, things you think might take too long, etc.

How to make a build system, how we will be configuring the server, how to parse the server's response/request for authentication.

# A brief explanation of how you're going to communicate and when you're going to meet to work together.

Communicate via primarily Discord but we also have slack if there's a problem with Discord.

We'll meet frequently early on to make the foundation program together. Afterwards, we will check in with each other to review our progress on chosen tasks, and learn about what the other person is doing and how they're doing it.

# A brief explanation of how you're going to ensure that all team members contribute substantially to the end product. (It is shockingly easy for one team member to ride the wave of the rest of the team's work, and end up with no contributions to show for themselves, and even less understanding of the project as a whole. Let's make sure this doesn't happen.)

Focus on working together to make a working foundation for our product. After that, split up to add more features and expand use cases. 

We start by working together, so we should be able to make sure that both of us make sure we both feel we are contributing. When we split up for adding features, we will do periodic check ins on the other team member to learn about what they're doing but also make sure they are contributing.