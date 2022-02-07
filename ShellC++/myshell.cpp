#include "token.cpp"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <signal.h>
#include <sys/wait.h>
#include <cstring>
#include <sstream>
using namespace std;

int main()
{
    // save the stdin and stdout file descriptor to be restored before 
    // program termination
    int tmpin = dup(0);
    int tmpout = dup(1);

    if (tmpin == -1)
    {
        printf("Failed to duplicate file descriptor!\n");
        exit(1);
    }

    if (tmpout == -1)
    {
        printf("Failed to duplicate file descriptor!\n");
        exit(1);
    }

    // save the current state of input and output destinations
    int fdin, fdout;

    int return_status;
    int childPID;
    int terminatedPID;
    int numCommands = 0;

    string userInput;  
    string inputCommand;

    // char array stores the tokens of each command
    char* tokenAsArgs[MAXARGS];

    TokenQueue tokenized = TokenQueue();

    printf("myshell$");
    getline(cin, userInput);

    stringstream inputStream(userInput); 

    // store all relevant information into tokenized array
    while (getline(inputStream, inputCommand, '|'))
    {
        tokenized.pushToken(inputCommand);
        numCommands++;
    }

    // set input file descriptor to stdin since first command reads from stdin
    fdin = dup(tmpin);

    if (fdin == -1)
    {
        printf("Failed to duplicate file descriptor!\n");
        exit(1);
    }

    for (int i = 0; i < numCommands; i++)
    {
        tokenized.popToken(tokenAsArgs);

        if (dup2(fdin, 0) == -1)
        {
            printf("Failed to replace file descriptor!\n");
            exit(1);
        }

        close(fdin);

        // if last command is running set output file descriptor to stdout 
        // since last command should write to stdout
        if (i == (numCommands - 1))
        {
            fdout = dup(tmpout);

            if (fdout == -1)
            {
                printf("Failed to duplicate file descriptor!\n");
                exit(1);
            }
        }

        // else create a pipe to send output of parent process to child process
        else 
        {
            int fdpipe[2];

            if (pipe(fdpipe) == -1)
            {
                printf("Pipe failed!\n");
                exit(1);
            }

            fdout = fdpipe[1];
            fdin = fdpipe[0];
        }

        if (dup2(fdout, 1) == -1)
        {
            printf("Failed to replace file descriptor!\n");
            exit(1);
        }

        close(fdout);

        childPID = fork();
        if (childPID < 0)
        {
            printf("Fork failed!\n");
            exit(1);
        }

        // execute linux command with execvp only in child process
        if (childPID == 0)
        {
            if (execvp(tokenAsArgs[0], tokenAsArgs) < 0)
            {
                printf("Process replacement failed!");
                exit(1);
            }
            
            perror("Command not sucesesfully executed!\n");
            exit(1);
        }
    }

    if (dup2(tmpin, 0) == -1)
    {
        printf("Failed to replace file descriptor!\n");
        exit(1);
    }

    if (dup2(tmpout, 1) == -1)
    {
        printf("Failed to replace file descriptor!\n");
        exit(1);
    }

    close(tmpin);
    close(tmpout);

    for (int i = 0; i < numCommands; i++)
    {
        // wait for child process to finish - get that's childs PID and exit 
        // status
        terminatedPID = wait(&return_status);
        
        // make sure child process returns normally without error
        if (return_status != 0)
        {
            perror("Child irregularly exited!\n");
            exit(1);
        }
        
        printf("Process %d exits with %d\n", terminatedPID, return_status);
    }
    
    exit(0);
}
