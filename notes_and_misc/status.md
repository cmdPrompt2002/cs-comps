10/13/2024

What's already working:
Given a server name, username, password, port, and ssh as the connection method inputted through command line arguments we can attempt to login to the destination via ssh with the provided information. The order of the command line arguments does not matter, as long as they follow the relevant signal (if there is one). We can then check to make sure it works and exit out for more attempts. We have tested it by logging into both of our respective ubuntu accounts as well as bandit0 from the bandit assignment, and it works succesfully identifies succesfull logins, failed logins, and wrong hosts. Ubuntu has worked well as a target to ssh into, and seems like its going to work well as a target or to host other login protocols. Basic output configuration to make it more readable too.

What we're working on right now:
Reading inputs from files a line at a time in order to understand multiple inputs and try to log in with multiple passwords. Cross compiling our code so we can compile it on our personal machines and get just the executable and run it in kali. Adding more ubuntu users to test cases with weird usernames and passwords as well as more general testing.

Things to know:
Cross compiling is convuluted but interesting. 