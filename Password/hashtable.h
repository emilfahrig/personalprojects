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
};

#endif // ARRAY_H
