// CPSC 3500: Shell
// Implements a basic shell (command line interface) for the file system

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "network.h"
#include <arpa/inet.h>
using namespace std;

#include "Shell.h"

static const string PROMPT_STRING = "NFS> ";	// shell prompt

// Mount the network file system with server name and port number in the format of server:port
void Shell::mountNFS(string fs_loc) {
	  //create the socket cs_sock and connect it to the server and port specified in fs_loc
	  //if all the above operations are completed successfully, set is_mounted to true  

    int socket_fd;
    int temp_cs_sock;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    // get the server name to convert to ip address and the port number
    stringstream tcpip(fs_loc);

    string ipAddress;
    string portNumber;

    getline(tcpip, ipAddress, ':');
    getline(tcpip, portNumber, '\n');

    // get info about the server to connect to
    if ((rv = getaddrinfo(ipAddress.c_str(), portNumber.c_str(), &hints, &servinfo)) != 0)
    {
        printf("Could not connect.");
        exit(1);
    }

    // create a socket and connect to the socket
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            printf("Error.");
            continue;
        }

        if (connect(socket_fd, p->ai_addr, p->ai_addrlen) == -1)
        {
            printf("Connect error.");
            close(socket_fd);
            continue;
        }

        // if we get here, we have connected, set is_mounted to true and set cs_sock
        cs_sock = socket_fd;
        is_mounted = true;
        break;
    }

    freeaddrinfo(servinfo);
} 

// Unmount the network file system if it was mounted
void Shell::unmountNFS() {
	  if (is_mounted)
    {
        if (close(cs_sock) == -1)
        {
            printf("Error. Could not close socket.");
            exit(1);
        }
    }
}

void Shell::send_command_to_server(const char* command)
{
    int command_length = strlen(command);

    // send command length to server so server knows how many bytes to allocate for the incoming packet
    send_cmdlength_to_server(command_length);

    char* p = new char[command_length];
    char* currentP = p;
    strcpy(p, command);
    int bytes_sent = 0;

    int num_bytes_to_send = command_length;

    while (bytes_sent < num_bytes_to_send) {
        int x = write(cs_sock, (void*)currentP, num_bytes_to_send - bytes_sent); 
        if (x == -1 || x == 0) {
            perror("Write error.");
            close(cs_sock);
            exit(1); 
        }
        currentP += x;   //adjust the buffer
        bytes_sent += x;  //record the bytes sent
    }

    delete[] p;
}

void Shell::send_cmdlength_to_server(int command_length)
{
    MsgHeader msg;

    msg.length = command_length;
    msg.val = htonl(1234);
    int bytes_sent = 0;
    int* p = (int*)&msg;
    while (bytes_sent < sizeof(msg)) {
        int x = write(cs_sock, (void*)p, sizeof(msg) - bytes_sent);
        if (x == -1 || x == 0) {
            perror("Write error.");
            close(cs_sock);
            exit(1); 
        }
        p += x;   //adjust the buffer
        bytes_sent += x;  //record the bytes sent
    }
}

char* Shell::receive_command_from_server()
{
    MsgHeader command;

    // receive message header from server so we know how many bytes in the incoming message
    command.val = htonl(1234);
    int* int_buffer = (int*)&command;
    int bytes_read_int = 0;
    while (bytes_read_int < sizeof(command)) {
        int x = read(cs_sock, (void*)int_buffer, sizeof(command) - bytes_read_int);
        if (x == -1 || x == 0) {
            perror("Read error.");
            close(cs_sock);
            exit(1); 
        }
        int_buffer += x;
        bytes_read_int += x;
    }

    // allocate a char array to store the received message from the server 
    int num_buyes_to_read = command.length;
    char* received_payload = new char[num_buyes_to_read + 1];
    char* current_char = received_payload;

    int bytes_read_char = 0;
    while (bytes_read_char < num_buyes_to_read) {
        int x = read(cs_sock, (void*)current_char, num_buyes_to_read - bytes_read_char);
        if (x == -1 || x == 0) {
            perror("Read error.");
            close(cs_sock);
            exit(1); 
        }
        current_char += x;
        bytes_read_char += x;
    }

    // return received packet to be printed to stdout
    received_payload[num_buyes_to_read] = '\0';
    return received_payload;
}

// Remote procedure call on mkdir
void Shell::mkdir_rpc(string dname) {
    // send appropriate command in http format
    string command_to_send = "mkdir " + dname + "\r\n";
    send_command_to_server(command_to_send.c_str());

    // receive server message and print its contents
    char* msg = receive_command_from_server();
    cout << msg;
}

// Remote procedure call on cd
void Shell::cd_rpc(string dname) {
    string command_to_send = "cd " + dname + "\r\n";
    send_command_to_server(command_to_send.c_str());

    char* msg = receive_command_from_server();
    cout << msg;
}

// Remote procedure call on home
void Shell::home_rpc() {
    string command_to_send = "home\r\n";
    send_command_to_server(command_to_send.c_str());

    char* msg = receive_command_from_server();
    cout << msg;
}

// Remote procedure call on rmdir
void Shell::rmdir_rpc(string dname) {
    string command_to_send = "rmdir " + dname + "\r\n";
    send_command_to_server(command_to_send.c_str());

    char* msg = receive_command_from_server();
    cout << msg;
}

// Remote procedure call on ls
void Shell::ls_rpc() {
    string command_to_send = "ls\r\n";
    send_command_to_server(command_to_send.c_str());

    char* msg = receive_command_from_server();
    cout << msg;
}

// Remote procedure call on create
void Shell::create_rpc(string fname) {
    string command_to_send = "create " + fname + "\r\n";
    send_command_to_server(command_to_send.c_str());

    char* msg = receive_command_from_server();
    cout << msg;
}

// Remote procedure call on append
void Shell::append_rpc(string fname, string data) {
    string command_to_send = "append " + fname + " " + data + "\r\n";
    send_command_to_server(command_to_send.c_str());

    char* msg = receive_command_from_server();
    cout << msg;
}

// Remote procesure call on cat
void Shell::cat_rpc(string fname) {
    string command_to_send = "cat " + fname + "\r\n";
    send_command_to_server(command_to_send.c_str());

    char* msg = receive_command_from_server();
    cout << msg << endl;
}

// Remote procedure call on head
void Shell::head_rpc(string fname, int n) {
    string command_to_send = "head " + fname + " " + to_string(n) + "\r\n";
    send_command_to_server(command_to_send.c_str());

    char* msg = receive_command_from_server();
    cout << msg << endl;;
}

// Remote procedure call on rm
void Shell::rm_rpc(string fname) {
    string command_to_send = "rm " + fname + "\r\n";
    send_command_to_server(command_to_send.c_str());

    char* msg = receive_command_from_server();
    cout << msg;
}

// Remote procedure call on stat
void Shell::stat_rpc(string fname) {
    string command_to_send = "stat " + fname + "\r\n";
    send_command_to_server(command_to_send.c_str());

    char* msg = receive_command_from_server();
    cout << msg;
}

// Executes the shell until the user quits.
void Shell::run()
{
  // make sure that the file system is mounted
  if (!is_mounted)
 	return; 
  
  // continue until the user quits
  bool user_quit = false;
  while (!user_quit) {

    // print prompt and get command line
    string command_str;
    cout << PROMPT_STRING;
    getline(cin, command_str);

    // execute the command
    user_quit = execute_command(command_str);
  }

  // unmount the file system
  unmountNFS();
}

// Execute a script.
void Shell::run_script(char *file_name)
{
  // make sure that the file system is mounted
  if (!is_mounted)
  	return;
  // open script file
  ifstream infile;
  infile.open(file_name);
  if (infile.fail()) {
    cerr << "Could not open script file" << endl;
    return;
  }


  // execute each line in the script
  bool user_quit = false;
  string command_str;
  getline(infile, command_str, '\n');
  while (!infile.eof() && !user_quit) {
    cout << PROMPT_STRING << command_str << endl;
    user_quit = execute_command(command_str);
    getline(infile, command_str);
  }

  // clean up
  unmountNFS();
  infile.close();
}


// Executes the command. Returns true for quit and false otherwise.
bool Shell::execute_command(string command_str)
{
  // parse the command line
  struct Command command = parse_command(command_str);

  // look for the matching command
  if (command.name == "") {
    return false;
  }
  else if (command.name == "mkdir") {
    mkdir_rpc(command.file_name);
  }
  else if (command.name == "cd") {
    cd_rpc(command.file_name);
  }
  else if (command.name == "home") {
    home_rpc();
  }
  else if (command.name == "rmdir") {
    rmdir_rpc(command.file_name);
  }
  else if (command.name == "ls") {
    ls_rpc();
  }
  else if (command.name == "create") {
    create_rpc(command.file_name);
  }
  else if (command.name == "append") {
    append_rpc(command.file_name, command.append_data);
  }
  else if (command.name == "cat") {
    cat_rpc(command.file_name);
  }
  else if (command.name == "head") {
    errno = 0;
    unsigned long n = strtoul(command.append_data.c_str(), NULL, 0);
    if (0 == errno) {
      head_rpc(command.file_name, n);
    } else {
      cerr << "Invalid command line: " << command.append_data;
      cerr << " is not a valid number of bytes" << endl;
      return false;
    }
  }
  else if (command.name == "rm") {
    rm_rpc(command.file_name);
  }
  else if (command.name == "stat") {
    stat_rpc(command.file_name);
  }
  else if (command.name == "quit") {
    return true;
  }

  return false;
}

// Parses a command line into a command struct. Returned name is blank
// for invalid command lines.
Shell::Command Shell::parse_command(string command_str)
{
  // empty command struct returned for errors
  struct Command empty = {"", "", ""};

  // grab each of the tokens (if they exist)
  struct Command command;
  istringstream ss(command_str);
  int num_tokens = 0;
  if (ss >> command.name) {
    num_tokens++;
    if (ss >> command.file_name) {
      num_tokens++;
      if (ss >> command.append_data) {
        num_tokens++;
        string junk;
        if (ss >> junk) {
          num_tokens++;
        }
      }
    }
  }

  // Check for empty command line
  if (num_tokens == 0) {
    return empty;
  }
    
  // Check for invalid command lines
  if (command.name == "ls" ||
      command.name == "home" ||
      command.name == "quit")
  {
    if (num_tokens != 1) {
      cerr << "Invalid command line: " << command.name;
      cerr << " has improper number of arguments" << endl;
      return empty;
    }
  }
  else if (command.name == "mkdir" ||
      command.name == "cd"    ||
      command.name == "rmdir" ||
      command.name == "create"||
      command.name == "cat"   ||
      command.name == "rm"    ||
      command.name == "stat")
  {
    if (num_tokens != 2) {
      cerr << "Invalid command line: " << command.name;
      cerr << " has improper number of arguments" << endl;
      return empty;
    }
  }
  else if (command.name == "append" || command.name == "head")
  {
    if (num_tokens != 3) {
      cerr << "Invalid command line: " << command.name;
      cerr << " has improper number of arguments" << endl;
      return empty;
    }
  }
  else {
    cerr << "Invalid command line: " << command.name;
    cerr << " is not a command" << endl; 
    return empty;
  } 

  return command;
}

