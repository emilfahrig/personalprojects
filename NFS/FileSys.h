// CPSC 3500: File System
// Implements the file system commands that are available to the shell.

#ifndef FILESYS_H
#define FILESYS_H

#include "BasicFileSys.h"
#include <string>
using namespace std;

class FileSys {
  
  public:
    // mounts the file system
    void mount(int sock);

    // unmounts the file system
    void unmount();

    // make a directory
    void mkdir(const char *name);

    // switch to a directory
    void cd(const char *name);
    
    // switch to home directory
    void home();
    
    // remove a directory
    void rmdir(const char *name);

    // list the contents of current directory
    void ls();

    // create an empty data file
    void create(const char *name);

    // append data to a data file
    void append(const char *name, const char *data);

    // display the contents of a data file
    void cat(const char *name);

    // display the first N bytes of the file
    void head(const char *name, unsigned int n);

    // delete a data file
    void rm(const char *name);

    // display stats about file or directory
    void stat(const char *name);

  private:
    BasicFileSys bfs;	// basic file system
    short curr_dir;	// current directory

    int fs_sock;  // file server socket
    int num_blocks_used;

    int findIndex(const char* name, struct dirblock_t current_dir_struct);
    bool isDirectory(struct dirblock_t current_dir_struct);
    bool isInode(struct inode_t current_inode);
    bool fileExists(struct dirblock_t current_dir_struct, const char* name);
    bool checkInvalidFileNameLength(const char* name);
    bool checkFullDirectory(struct dirblock_t current_dir_struct);
    bool checkEmptyDirectory(struct dirblock_t current_dir_struct);
    bool checkFullDisk();
    bool checkExceedsMaxFileSize(struct inode_t current_inode, int num_bytes_to_add);
    void sendClientMessage(string code_message, string message_length = "", string message_body = "");
    void send_cmdlength_to_client(int command_length);
    int getNumberBlocks(struct inode_t current_inode);
    // Additional private variables and Helper functions - if desired
};

#endif 
