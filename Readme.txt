The file myshell.cpp is a simple implementation of a shell written in C/C++.
The shell is capable of executing one command or multiple commands using
multiprocessing. When running multiple commands in a pipeline, the shell uses
pipes to replace the output file descriptor of the parent process and the input
file descriptor of the child process so the output of each command can be sent 
through the pipeline. This is repeated for all process execpt the last child 
process. The reason for this is that the last child process should write to 
stdout so the user can see the output of the commands. The process ID of each
process in the pipeline is printed to stdout as well as it's exit status - 
meaning if 4 commands are given, the process ID and exit status for each 
process running a command will be given. All potential system call errors
are handled by printing an error message and returning an exit status of 1. 
Tokenization is preformed by first splitting the user input by commands using
getline with the delimeter '|'. Next, each command line string is passed as an
arguemnt to the TokenQueue which further processes the string by seperating it 
with the " " delimeter to obtain all command arguments. There arguments are 
stored as a char* array in a linked list. Since we have no knowledge about how
many command line arguments are given, a linked list makes sure that no extra 
unused memory is allocated on the stack. Furthermore, a queue implementation is
used as we can append all char* to the linked list and pop each char* from the
linked list to obtain all tokenized commands in the order the user gave them. 

There are currently no known bugs or issues with the myshell.cpp, token.cpp or
token.h file. 