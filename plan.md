# A short description of the project. I'm thinking 1-4 sentences, but whatever works for you.

A rudimentary password spraying tool that supports Basic authentication in http-get and SSH logins, with features such as multithreading, verbose output, redirect output to file, and accepts input wordlists (trite, should remove?). Code written in C.

A short list of learning goals--what do you want to end up understanding that you don't understand yet?

How SSH logins work and what factors must be present in order for it to work. Related—how to use C's SSH package.
Same for the http-get basic authentication.

How to set up a server for testing our tool

How to do multithreading (Im not sure what this is)

What is a build system.

# A description of your project's architecture. This could be just a diagram of your expected code organization, but it could also include things like the setup of a target server, the data you're going to need for testing, a build system (e.g., Makefile or something similar), etc.

## Expected code organization
- hydra.c - parsing the command line arguments

- http-get.c - handling the authentication process in http-get requests. Basic authentication is a guarantee, but we might add more features
- http-get.h

- ssh.c - handling the SSH login process
- ssh.h

## Target server setup

## Data for testing
- 

# A testing and benchmarking plan.
- Using the same protocol and input wordlists, compare the average speed of our program to that of Hydra's.
- Find out how to measure how much memory the program consumes


# A more detailed schedule of development than you wrote last week. In addition to a list of development steps and proposed deadlines, you should feel free to annotate items that feel uncertain at this moment.


# A list of things one or more of you are worried about. This could be stuff you don't understand yet, things you think might take too long, etc.

# A brief explanation of how you're going to communicate and when you're going to meet to work together.

# A brief explanation of how you're going to ensure that all team members contribute substantially to the end product. (It is shockingly easy for one team member to ride the wave of the rest of the team's work, and end up with no contributions to show for themselves, and even less understanding of the project as a whole. Let's make sure this doesn't happen.)