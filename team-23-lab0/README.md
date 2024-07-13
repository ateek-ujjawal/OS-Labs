# CS 5600 Homework Zero

The purpose of this assignment is to get you ready for your first real assignment by:
- setting up the class virtual machine
- using your Github project repository
- using the editor, compiler, and debugger

[Installing your virtual machine](installing.md)

## Set up Git authentication

There are two options for authenticating to the Github server: SSH keys and authorization tokens.
We describe how to do this with authorization tokens, but you are free to use either method.

First, get an auth token:
- Go to [github.khoury.northeastern.edu](https://github.khoury.northeastern.edu)
- click your avatar at the top right, and select "Settings" (next to bottom item)
- on the left select "Developer Settings" (very bottom item)
- click on "Personal Access Tokens"

Finally click "Generate a new token", for scope click "repo - full control of private repositories" scope, and set it to not have an expiration date.
Make sure that you copy and save this token - you can't get it back after you leave this screen, you'll have to delete it and generate a new one.
When Git prompts you for username and password, you'll give it your Khoury LDAP username, and the auth token you just created as the password.

For information on actually using Git, there should be a file named "git-instructions.pdf" available under “Resources → General Resources” in Piazza.

## Actually do the assigment

Step 1: Check out your copy of the Homework 0 Git repository

```
git clone https://github.khoury.northeastern.edu/cs5600-f23/team-N-lab0
```
where `N` is your team number. (e.g. team-1-lab0, team-17-lab0)

Step 2: Use a text editor to edit the file “EDIT-ME.txt” in the repository you've checked out, answering the questions you find there. Commit your changes to the file  and push your changes so that the graders can see them; e.g.:
```
git commit EDIT-ME.txt -m 'answered the questions'
git push
```
(Note that if you don't specify a commit message with -m, git will start an editor and ask you to write a message. When you're working on bigger programs, commit messages will become important.)

Step 3: Create a C program named `homework.c` in this directory which counts from 0 to 9 using a loop, printing each number. Add the file to the repository, commit it, and push your changes.
```
git add homework.c
git commit homework.c -m 'new file, per directions'
git push
```

Step 4. Compile the file with debugging (-g) and run it under the gdb debugger. Begin running with the start command (this will stop at main, allowing you to step through) and then step through using the next command until it has incremented the loop variable several times. Finally execute the following three commands in the debugger: "info locals", "where", and "list". Take a screenshot, add it to the repository, commit it, and push; e.g.:
```
git add screenshot.png
git commit -m 'a screenshot' screenshot.png
git push
```

You can take a screenshot of the VM screen in Windows or MacOS; alternately you can install `gnome-screenshot` - if you run that in another terminal, it will write a screenshot in your `Pictures` directory.

## Rules

As always, each team should do the work on their own.
The following rules will affect your grade on the project:

- Each team should do the work on their own
- Each team member should contribute - i.e. the repository should contain commits from both partners
- Please format your code reasonably
