#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "FileSys.h"
#include "network.h"
using namespace std;

const int MAX_NUM_TOKEN = 3;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cout << "Usage: ./nfsserver port#\n";
        return -1;
    }

    int port = atoi(argv[1]);

    // create the socket and accept the client connection
    int sock_fd = socket(AF_INET, SOCK_STREAM, AF_UNSPEC);
    if (sock_fd == -1)
    {
        printf("Error. Could not create socket.");
        exit(1);
    }

    struct sockaddr_in hints;

    hints.sin_family = AF_INET;
    hints.sin_port = htons(10186);
    hints.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock_fd, (sockaddr*)&hints, sizeof(hints)) == -1)
    {
        printf("Error. Bind failed.");
        exit(1);
    }

    if (listen(sock_fd, 1) == -1)
    {
        printf("Error. Listen failed.");
        exit(1);
    }


    sockaddr_in client;
    socklen_t client_size = sizeof(client);

    int sock = accept(sock_fd, (sockaddr*)&client, &client_size);
    if (sock == -1)
    {
        printf("Error. Could not establish a communication socket with clinet.");
        exit(1);
    }
    
    // since we have connected with a clinet we can close listening socket since we only have one clinet 
    if (close(sock_fd) == -1)
    {
        printf("Error. Could not close listening socket.");
        exit(1);
    }

    //mount the file system
    FileSys fs;
    fs.mount(sock); //assume that sock is the new socket created 
                    //for a TCP connection between the client and the server.   
 
    //loop: get the command from the client and invoke the file
    //system operation which returns the results or error messages back to the clinet
    //until the client closes the TCP connection.
    MsgHeader command;

    while (true)
    {
        // receive the message length from clinet
        command.val = htonl(1234);
        int* int_buffer = (int*)&command;
        int bytes_read_int = 0;
        while (bytes_read_int < sizeof(command)) {
            int x = read(sock, (void*)int_buffer, sizeof(command) - bytes_read_int);
            if (x == -1 || x == 0) {
                perror("Read error.");
                close(sock);
                exit(1); 
            }
            int_buffer += x;
            bytes_read_int += x;
        }

        // allocate buffer to store message length sent from client
        int num_buyes_to_read = command.length;
        char* received_payload = new char[num_buyes_to_read];
        char* current_char = received_payload;

        int bytes_read_char = 0;
        while (bytes_read_char < num_buyes_to_read) {
            int x = read(sock, (void*)current_char, num_buyes_to_read - bytes_read_char);
            if (x == -1 || x == 0) {
                perror("Read error.");
                close(sock);
                exit(1); 
            }
            current_char += x;
            bytes_read_char += x;
        }

        // strip command of \r\n
        stringstream command(received_payload);
        string token_no_whitespace;

        getline(command, token_no_whitespace, '\r');

        // now get all relevant tokens from clinet package
        stringstream command_stream(token_no_whitespace);
        string token;

        // array to store tokens
        string tokenArray[MAX_NUM_TOKEN] = {""};

        for(int i = 0; getline(command_stream, token, ' '); i++)
        {
            tokenArray[i] = token;
        }

        // call appropriate file system funtion based of user command 
        if (tokenArray[0] == "mkdir")
        {
            fs.mkdir(tokenArray[1].c_str());
        }
        else if (tokenArray[0] == "cd")
        {
            fs.cd(tokenArray[1].c_str());
        }

        else if (tokenArray[0] == "home")
        {
            fs.home();
        }

        else if (tokenArray[0] == "rmdir")
        {
            fs.rmdir(tokenArray[1].c_str());
        }

        else if (tokenArray[0] == "ls")
        {
            fs.ls();
        }

        else if (tokenArray[0] == "create")
        {
            fs.create(tokenArray[1].c_str());
        }

        else if (tokenArray[0] == "append")
        {
            fs.append(tokenArray[1].c_str(), tokenArray[2].c_str());
        }

        else if (tokenArray[0] == "cat")
        {
            fs.cat(tokenArray[1].c_str());
        }

        else if (tokenArray[0] == "head")
        {
            fs.head(tokenArray[1].c_str(), stoi(tokenArray[2]));
        }

        else if (tokenArray[0] == "rm")
        {
            fs.rm(tokenArray[1].c_str());
        }

        else if (tokenArray[0] == "stat")
        {
            fs.stat(tokenArray[1].c_str());
        }

        delete[] received_payload;
    }

    //close the listening socket
    if (close(sock_fd) == -1)
    {
        printf("Error. Could not close socket.");
        exit(1);
    }

    //unmout the file system
    fs.unmount();

    return 0;
}
