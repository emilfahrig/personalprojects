// CPSC 3500: File System
// Implements the file system commands that are available to the shell.

#include <stdio.h>
#include <string.h>
#include <math.h>  
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <netinet/in.h>
using namespace std;

#include "FileSys.h"
#include "network.h"
#include "BasicFileSys.h"
#include "Blocks.h"

// mounts the file system
void FileSys::mount(int sock) {
  bfs.mount();
  num_blocks_used = 2;
  curr_dir = 1; //by default current directory is home directory, in disk block #1
  fs_sock = sock; //use this socket to receive file system operations from the client and send back response messages
}

// unmounts the file system
void FileSys::unmount() {
  bfs.unmount();
  close(fs_sock);
}

// make a directory
void FileSys::mkdir(const char *name)
{
    if (checkInvalidFileNameLength(name))
    {
        sendClientMessage("504 File name is too long");
        return;
    }

    if (checkFullDisk())
    {
        sendClientMessage("505 Disk is full");
        return;
    }

    struct dirblock_t current_dir_struct;
    bfs.read_block(curr_dir, (void*) &current_dir_struct);

    if (checkFullDirectory(current_dir_struct))
    {
        sendClientMessage("506 Directory is full");
        return;
    }

    if (fileExists(current_dir_struct, name))
    {
        sendClientMessage("502 File exists");
        return;
    }

    // get free disk space and initialize new directory
    short free_block_index = bfs.get_free_block();
    struct dirblock_t new_dir;
    new_dir.magic = DIR_MAGIC_NUM;
    new_dir.num_entries = 0;

    for (int i = 0; i < MAX_DIR_ENTRIES; i++) {
        new_dir.dir_entries[i].block_num = 0;
    }

    // write newly created directory block to disk
    bfs.write_block(free_block_index, (void *) &new_dir);


    // find availbale index in file/directory array in current directory to store new directory
    int i = 0;
    while (current_dir_struct.dir_entries[i].block_num != 0)
    {
        i++;
    }

    // change dir_entries array in current directory to reflect newly added directory
    strncpy(current_dir_struct.dir_entries[i].name, name, (MAX_FNAME_SIZE + 1) * sizeof(char));
    current_dir_struct.dir_entries[i].block_num = free_block_index;
    current_dir_struct.num_entries++;
    bfs.write_block(curr_dir, (void*) &current_dir_struct);

    // used a block, so incement num_blocks_used
    num_blocks_used++;

    sendClientMessage("200 OK");
}

// switch to a directory
void FileSys::cd(const char *name)
{
    struct dirblock_t current_dir_struct;
    bfs.read_block(curr_dir, (void*) &current_dir_struct);

    if (!fileExists(current_dir_struct, name))
    {
        sendClientMessage("503 File does not exist");
        return;
    }

    int dir_index = findIndex(name, current_dir_struct);

    struct dirblock_t dir_to_enter;
    bfs.read_block(current_dir_struct.dir_entries[dir_index].block_num, (void*) &dir_to_enter);

    if (!isDirectory(dir_to_enter))
    {
        sendClientMessage("500 File is not a directory");
        return;
    }

    // change current directory number to user inputted directories' respective block number
    curr_dir = current_dir_struct.dir_entries[dir_index].block_num;

    sendClientMessage("200 OK");
}

void FileSys::home() 
{
    // switch to home directory
    curr_dir = 1;
    sendClientMessage("200 OK");
}

// remove a directory
void FileSys::rmdir(const char *name)
{
    // read in current directory struct
    struct dirblock_t current_dir_struct;
    bfs.read_block(curr_dir, (void*) &current_dir_struct);

    if (!fileExists(current_dir_struct, name))
    {
        sendClientMessage("503 File does not exist");
        return;
    }

    // find index of directory to remove in dir_entries
    int dir_index = findIndex(name, current_dir_struct);
    short block_num_remove = current_dir_struct.dir_entries[dir_index].block_num;

    // read in block to remove to check if it is empty, return if not empty
    struct dirblock_t dir_to_remove;
    bfs.read_block(block_num_remove, (void*) &dir_to_remove);

    if (!isDirectory(dir_to_remove))
    {
        sendClientMessage("500 File is not a directory");
        return;
    }

    if (checkEmptyDirectory(dir_to_remove))
    {
        sendClientMessage("507 Directory is not empty");
        return;
    }

    // init information about directory to remove
    dir_to_remove.magic = 0;
    bfs.write_block(block_num_remove, (void*) &dir_to_remove);

    bfs.reclaim_block(block_num_remove);

    // initialize block that will be removed 
    memset(current_dir_struct.dir_entries[dir_index].name, 0, MAX_FNAME_SIZE + 1);
    current_dir_struct.dir_entries[dir_index].block_num = 0;
    current_dir_struct.num_entries--;

    // write altered directory back to disk 
    bfs.write_block(curr_dir, (void*) &current_dir_struct);

    num_blocks_used--;

    sendClientMessage("200 OK");
}

// list the contents of current directory
void FileSys::ls()
{
    string temp_message_body;

    struct dirblock_t current_dir_struct;
    bfs.read_block(curr_dir, (void*) &current_dir_struct);

    for (int i = 0; i < MAX_DIR_ENTRIES; i++)
    {
        if (current_dir_struct.dir_entries[i].block_num != 0)
        {
            // read in block and display it's file name or directory name
            struct dirblock_t file_or_dir;
            bfs.read_block(current_dir_struct.dir_entries[i].block_num, (void*) &file_or_dir);
            if (isDirectory(file_or_dir))
            {
                temp_message_body += current_dir_struct.dir_entries[i].name;
                temp_message_body += "/\n";
            }

            else if (file_or_dir.magic == INODE_MAGIC_NUM)
            {
                temp_message_body += current_dir_struct.dir_entries[i].name;
                temp_message_body += "\n";
            }
        }
    }

    sendClientMessage("200 OK", to_string(strlen(temp_message_body.c_str())), temp_message_body);
}

// create an empty data file
void FileSys::create(const char *name)
{
    if (checkInvalidFileNameLength(name))
    {
        sendClientMessage("504 File name is too long");
        return;
    }

    // read current directory block
    struct dirblock_t current_dir_struct;
    bfs.read_block(curr_dir, (void*) &current_dir_struct);

    if (checkFullDirectory(current_dir_struct))
    {
        sendClientMessage("506 Directory is full");
        return;
    }

    if (fileExists(current_dir_struct, name))
    {
        sendClientMessage("502 File exists");
        return;
    }

    if (checkFullDisk())
    {
        sendClientMessage("505 Disk is full");
        return;
    }

    short free_block_index = bfs.get_free_block();

    // initliaize file block
    struct inode_t new_file;
    new_file.magic = INODE_MAGIC_NUM;
    new_file.size = 0;

    for (int i = 0; i < MAX_DATA_BLOCKS; i++)
    {
        new_file.blocks[i] = 0;
    }

    bfs.write_block(free_block_index, (void*) &new_file);

    // find availbale index in file/directory array in current directory to store new directory
    int i = 0;
    while (current_dir_struct.dir_entries[i].block_num != 0)
    {
        i++;
    }

    // add new file to directory
    strncpy(current_dir_struct.dir_entries[i].name, name, (MAX_FNAME_SIZE + 1) * sizeof(char));
    current_dir_struct.dir_entries[i].block_num = free_block_index;
    current_dir_struct.num_entries++;
    bfs.write_block(curr_dir, (void*) &current_dir_struct);

    num_blocks_used++;

    sendClientMessage("200 OK");
}

// append data to a data file
void FileSys::append(const char *name, const char *data)
{
    struct dirblock_t current_dir_struct;
    bfs.read_block(curr_dir, (void*) &current_dir_struct);

    if (!fileExists(current_dir_struct, name))
    {
        sendClientMessage("503 File does not exist");
        return;
    }

    // find index of directory to remove in dir_entries
    int dir_index = findIndex(name, current_dir_struct);

    short write_inode_block_number = current_dir_struct.dir_entries[dir_index].block_num;

    struct inode_t write_inode;
    bfs.read_block(write_inode_block_number, (void*) &write_inode);

    int num_bytes_to_write = strlen(data);
    if (checkExceedsMaxFileSize(write_inode, num_bytes_to_write))
    {
        sendClientMessage("508 Append exceeds maximum file size");
        return;
    }

    if (!isInode(write_inode))
    {
        sendClientMessage("501 File is a directory");
        return;
    }

    int num_bytes_written = 0;
    int num_current_blocks = getNumberBlocks(write_inode);

    struct datablock_t current_datablock;

    // read data block only if there is already bytes in the datablock
    if (write_inode.size != 0) 
    {
        bfs.read_block(write_inode.blocks[num_current_blocks - 1], (void*) &current_datablock);
    }

    while (num_bytes_written < num_bytes_to_write)
    {
        // write datablock to disk if it is full of 128 bytes
        if (((write_inode.size % BLOCK_SIZE) == 0) && (write_inode.size != 0))
        {
            if (write_inode.blocks[num_current_blocks - 1] != 0)
            {
                bfs.write_block(write_inode.blocks[num_current_blocks - 1], (void*) &current_datablock);
            }

            if (checkFullDisk())
            {
                sendClientMessage("505 Disk is full");
                return;
            }

            short free_block_index = bfs.get_free_block();
            bfs.write_block(free_block_index, (void*) &current_datablock);

            num_blocks_used++;

            write_inode.blocks[num_current_blocks++] = free_block_index;
            bfs.write_block(write_inode_block_number, (void*) &write_inode);
        }

        // adding characters char by char into the datablocks
        current_datablock.data[(write_inode.size++) % BLOCK_SIZE] = data[num_bytes_written++];
    }

    // add datablock to disk if it was the inode's first datablock
    if (num_bytes_to_write == write_inode.size)
    {
        if (checkFullDisk())
        {
            sendClientMessage("505 Disk is full");
            return;
        }

        short free_block_index = bfs.get_free_block();
        bfs.write_block(free_block_index, (void*) &current_datablock);

        num_blocks_used++;

        write_inode.blocks[num_current_blocks] = free_block_index;
        bfs.write_block(write_inode.blocks[num_current_blocks], (void*) &current_datablock);
    }

    // otherwise write the modified datablock back to the disk
    else
    {
        bfs.write_block(write_inode.blocks[num_current_blocks - 1], (void*) &current_datablock);
    }

    // since we changed the contents of the inode block, write back to disk
    bfs.write_block(write_inode_block_number, (void*) &write_inode);
    sendClientMessage("200 OK");
}

// display the contents of a data file
void FileSys::cat(const char *name)
{
    // string to hold the message body to senf back to the client
    string temp_message_body;

    struct dirblock_t current_dir_struct;
    bfs.read_block(curr_dir, (void*) &current_dir_struct);

    if (!fileExists(current_dir_struct, name))
    {
        sendClientMessage("503 File does not exist");
        return;
    }

    // find index of directory to remove in dir_entries
    int dir_index = findIndex(name, current_dir_struct);

    struct inode_t read_inode;
    bfs.read_block(current_dir_struct.dir_entries[dir_index].block_num, (void*) &read_inode);

    if (!isInode(read_inode))
    {
        sendClientMessage("501 File is a directory");
        return;
    }

    int num_current_blocks = getNumberBlocks(read_inode);

    struct datablock_t file_data;
    int num_bytes_read = read_inode.size;

    // loop through all current datablocks of the inode and store all contents one by one
    for (int i = 0; i < num_current_blocks; i++)
    {
        bfs.read_block(read_inode.blocks[i], (void*) &file_data);
        int num_bytes_in_file = (num_bytes_read >= BLOCK_SIZE) ? BLOCK_SIZE : num_bytes_read;
        
        for (int byte = 0; byte < num_bytes_in_file; byte++)
        {
            temp_message_body += file_data.data[byte];
        }

        num_bytes_read -= num_bytes_in_file;
    }

    sendClientMessage("200 OK", to_string(strlen(temp_message_body.c_str())), temp_message_body);
}

// display the first N bytes of the file
void FileSys::head(const char *name, unsigned int n)
{
    string temp_message_body;

    struct dirblock_t current_dir_struct;
    bfs.read_block(curr_dir, (void*) &current_dir_struct);

    if (!fileExists(current_dir_struct, name))
    {
        sendClientMessage("503 File does not exist");
        return;
    }

    // find index of directory to remove in dir_entries
    int dir_index = findIndex(name, current_dir_struct);

    struct inode_t read_inode;
    bfs.read_block(current_dir_struct.dir_entries[dir_index].block_num, (void*) &read_inode);

    if (!isInode(read_inode))
    {
        sendClientMessage("501 File is a directory");
        return;
    }

    int num_current_blocks = getNumberBlocks(read_inode);

    struct datablock_t file_data;
    int num_bytes_read = (n > read_inode.size) ? read_inode.size : n;
    for (int i = 0; i < num_current_blocks; i++)
    {
        bfs.read_block(read_inode.blocks[i], (void*) &file_data);
        int num_bytes_in_file = (num_bytes_read >= BLOCK_SIZE) ? 128 : num_bytes_read;
        
        for (int i = 0; i < num_bytes_in_file; i++)
        {
            temp_message_body += file_data.data[i];
        }

        num_bytes_read -= BLOCK_SIZE;
    }

    sendClientMessage("200 OK", to_string(strlen(temp_message_body.c_str())), temp_message_body);
}

// delete a data file
void FileSys::rm(const char *name)
{
    struct dirblock_t current_dir_struct;
    bfs.read_block(curr_dir, (void*) &current_dir_struct);

    if (!fileExists(current_dir_struct, name))
    {
        sendClientMessage("503 File does not exist");
        return;
    }

    // find index of directory to remove in dir_entries
    int dir_index = findIndex(name, current_dir_struct);

    struct inode_t read_inode;
    bfs.read_block(current_dir_struct.dir_entries[dir_index].block_num, (void*) &read_inode);

    if (!isInode(read_inode))
    {
        sendClientMessage("501 File is a directory");
        return;
    }

    int num_current_blocks = getNumberBlocks(read_inode);

    // initialize all datablock contents back to original state and write back to disk to be used again
    for (int i = 0; i < num_current_blocks; i++)
    {
        struct datablock_t file_data;
        bfs.read_block(read_inode.blocks[i], (void*) &file_data);
        memset(file_data.data, 0, BLOCK_SIZE);
        bfs.write_block(read_inode.blocks[i], (void*) &file_data);
        bfs.reclaim_block(read_inode.blocks[i]);

        num_blocks_used--;
    }

    // initialize inode values and write back to disk
    read_inode.magic = 0;
    read_inode.size = 0;
    memset((void*)read_inode.blocks, 0, MAX_DATA_BLOCKS * sizeof(short));
    bfs.write_block(current_dir_struct.dir_entries[dir_index].block_num, (void*) &read_inode);

    bfs.reclaim_block(current_dir_struct.dir_entries[dir_index].block_num);

    num_blocks_used--;

    // initialize block that will be removed 
    memset(current_dir_struct.dir_entries[dir_index].name, 0, MAX_FNAME_SIZE + 1);
    current_dir_struct.dir_entries[dir_index].block_num = 0;
    current_dir_struct.num_entries--;

    // write changes back to current directory structure
    bfs.write_block(curr_dir, (void*) &current_dir_struct);

    sendClientMessage("200 OK");
}

// display stats about file or directory
void FileSys::stat(const char *name)
{
    string temp_message_body;

    struct dirblock_t current_dir_struct;
    bfs.read_block(curr_dir, (void*) &current_dir_struct);

    if (!fileExists(current_dir_struct, name))
    {
        sendClientMessage("503 File does not exist");
        return;
    }

    int dir_index = findIndex(name, current_dir_struct);

    struct inode_t file_or_dir;
    bfs.read_block(current_dir_struct.dir_entries[dir_index].block_num, (void*) &file_or_dir);

    // append to temp_message_body to create a string that can be sent back to client
    if (file_or_dir.magic == INODE_MAGIC_NUM)
    {
        temp_message_body += "Inode block: ";
        temp_message_body += to_string(current_dir_struct.dir_entries[dir_index].block_num);
        temp_message_body += "\n";
        temp_message_body +=  "Bytes in file: ";
        temp_message_body += to_string(file_or_dir.size);
        temp_message_body += "\n";
        temp_message_body += "Number of Blocks: ";
        temp_message_body += to_string(getNumberBlocks(file_or_dir));
        temp_message_body += "\n";
        temp_message_body += "First Block: ";
        temp_message_body += to_string(file_or_dir.blocks[0]);
        temp_message_body += "\n";
    }

    else
    {
        temp_message_body += "Directory name: ";
        temp_message_body += current_dir_struct.dir_entries[dir_index].name;
        temp_message_body += "/\n";
        temp_message_body += "Directory block: ";
        temp_message_body += to_string(current_dir_struct.dir_entries[dir_index].block_num);
        temp_message_body += "\n";
    }

    sendClientMessage("200 OK", to_string(strlen(temp_message_body.c_str())), temp_message_body);
}

int FileSys::getNumberBlocks(struct inode_t current_inode)
{
    return ((current_inode.size == 0) ? 0 : 
        ((current_inode.size % 128 == 0) ? (current_inode.size / BLOCK_SIZE) : 
        ((current_inode.size / BLOCK_SIZE) + 1)));
}

int FileSys::findIndex(const char* name, struct dirblock_t current_dir_struct)
{
    int i = 0;
    while ((strncmp(current_dir_struct.dir_entries[i].name, name, MAX_FNAME_SIZE + 1) != 0) && (i < MAX_DIR_ENTRIES))
    {
        i++;
    }

    return i;
}

bool FileSys::isDirectory(struct dirblock_t current_dir_struct)
{
    return (current_dir_struct.magic == DIR_MAGIC_NUM) ? true : false;
}

bool FileSys::isInode(struct inode_t current_inode)
{
    return (current_inode.magic == INODE_MAGIC_NUM) ? true : false;
}

bool FileSys::fileExists(struct dirblock_t current_dir_struct, const char* name)
{
    for (int i = 0; i < MAX_DIR_ENTRIES; i++)
    {
        if ((current_dir_struct.dir_entries[i].block_num != 0) && (strncmp(current_dir_struct.dir_entries[i].name, name, MAX_FNAME_SIZE + 1) == 0))
        {   
            return true;
        }
    }

    return false;
}

bool FileSys::checkInvalidFileNameLength(const char* name)
{
    return (strlen(name) > MAX_FNAME_SIZE) ? true : false;
}

bool FileSys::checkFullDirectory(struct dirblock_t current_dir_struct)
{
    return (current_dir_struct.num_entries == MAX_DIR_ENTRIES) ? true : false;
}

bool FileSys::checkEmptyDirectory(struct dirblock_t current_dir_struct)
{
    return (current_dir_struct.num_entries != 0) ? true : false;
}

bool FileSys::checkFullDisk()
{
    return (num_blocks_used == NUM_BLOCKS) ? true : false;
}

bool FileSys::checkExceedsMaxFileSize(struct inode_t current_inode, int num_bytes_to_add)
{
    return ((current_inode.size + num_bytes_to_add) > MAX_DATA_BLOCKS * BLOCK_SIZE) ? true : false;
}

void FileSys::sendClientMessage(string code_message, string message_length, string message_body)
{
    string command_to_send;

    // if length is empty, there is no body and length is 0
    if (message_length == "")
    {
        command_to_send = code_message + "\r\n" + "Length: 0\r\n\r\n";
    }

    // otherwise, append body message and length to command to send to client
    else 
    {
        command_to_send = code_message + "\r\n" + "Length: " + message_length + "\r\n\r\n" + message_body;
    }

    int command_length = strlen(command_to_send.c_str());

    // send client length of message so they know size of package to be received
    send_cmdlength_to_client(command_length);

    // allocate a dynamic char array with extra space for null termination character
    char* p = new char[command_length + 1];
    char* currentP = p;
    strcpy(p, command_to_send.c_str());
    int bytes_sent = 0;

    int num_bytes_to_send = command_length;

    while (bytes_sent < num_bytes_to_send) {
        int x = write(fs_sock, (void*)currentP, num_bytes_to_send - bytes_sent);
        if (x == -1 || x == 0) {
            perror("Write error.");
            close(fs_sock);
            exit(1); 
        }
        currentP += x;   //adjust the buffer
        bytes_sent += x;  //record the bytes sent
    }

    delete[] p;
}

void FileSys::send_cmdlength_to_client(int command_length)
{
    MsgHeader msg;

    msg.length = command_length;
    msg.val = htonl(1234);
    int bytes_sent = 0;
    int* p = (int*)&msg;
    while (bytes_sent < sizeof(msg)) {
        int x = write(fs_sock, (void*)p, sizeof(msg) - bytes_sent);
        if (x == -1 || x == 0) {
            perror("Write error.");
            close(fs_sock);
            exit(1); 
        }
        p += x;   //adjust the buffer
        bytes_sent += x;  //record the bytes sent
    }
}

// HELPER FUNCTIONS (optional)
