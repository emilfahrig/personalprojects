#ifndef HASHTABLE_H
#define HASHTABLE_H
#include "QString"
#include "user.h"

class HashTable {
    private:
        User* playlist;
        int hashSize;
        int numUsers;
        bool calcLoadFactor(int numUsers, int hashSize);
        bool isPrime(int hashSize);
        void deletePlaylist();
        int hashFunction(int key);

    public:
        HashTable();
        ~HashTable();
        void addToPlaylist(int key, int encryption_input, User* obj);
        int getValue(int keyy);
        void rehashTable();
        bool checkAuthenticity(int encryption1, int encryption2);
        QString getFirstName(int key);
        QString getLastName(int key);
        QString getUsername(int key);
        QString getAddress(int key);
        QString getCity(int key);
        int getZip(int key);
        quint64 getPhone(int key);
        QString getEmail(int key);
        void changePassword(int key_input, QString email_input, int new_encryption);
        bool checkAuthenticityChangePassword(int key_input, QString email_input);
        //description: this function adds a node to the hash table storing in
        //it the given object and key at the appropriate index
        //preconditions: a Movie object has been created in main
        //postconditions: a node has been added to the hash table that stores
        //the passed in data
        //bool watchMovie(long key);
        //description: this function deletes the node in the hash table that
        //stores the passed in key
        //preconditions: a node must exist in the hash table that stores the
        //passed in key
        //postconditions: the node that stores the passed in key is deleted
        //from the linked list
        //void displayMovie(long key);
        //description: displays the information of the movie with the passed
        //in key
        //preconditions: a movie with the passed in key must exist in the hash
        //table for anything to be printed to the console
        //postoncitions: no member variables will be altered after this
        //function runs
        //void displayAll();
        //description: this function prints the title and id stored in each
        //node in the hash table
        //preconditions: nodes must have been added to the hash table for this
        //function to print
        //postoncitions: no member variables will be altered after this
        //function runs
        //void displayGenre(string genre);
        //description: this function prints the title and id stored in each
        //node in the hash table that stores a Movie object with the passed
        //in genre
        //preconditions: nodes must have been added to the hash table that
        //store Movie objects which store the passed in genre
        //postoncitions: no member variables will be altered after this
        //function runs
};

#endif // ARRAY_H
