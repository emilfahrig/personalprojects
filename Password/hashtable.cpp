#include "hashtable.h"
#include "user.h"
#include <QDebug>

HashTable::HashTable()
{
    numUsers = 0;
    hashSize = 11;
    playlist = new User[hashSize];
}

HashTable::~HashTable()
{
    deletePlaylist();
}

void HashTable::deletePlaylist()
{
    delete[] playlist;
    playlist = nullptr;
}

int HashTable::hashFunction(int key)
{
    return key % hashSize;
}

void HashTable::addToPlaylist(int key, int encryption_input, User* obj)
{
    numUsers++;
    playlist[hashFunction(key)].appendNode(obj, key, encryption_input);

    bool needRehash;
    needRehash = calcLoadFactor(numUsers, hashSize);

    //check if the hash table needs to be rehashed
    if  (needRehash)  {
        rehashTable();
    }
}

void HashTable::rehashTable()
{
    int oldTableSize = hashSize;  //stores old table size
    int newTableSize = hashSize * 2;  //stores new table size

    //increment newTableSIze until it is a prime number as they are optimal
    //being the size of a hash table
    while  (!isPrime(newTableSize))  {
        newTableSize++;
    }

    //update tableSize so that hash function is also updated
    hashSize = newTableSize;

    //create new hash table with correct size
    User* newPlaylist = nullptr;
    newPlaylist = new User[hashSize];

    //copy values from old hash table to new hash table
    for  (int i = 0; i < oldTableSize; i++)  {
        playlist[i].rehashNodes(newPlaylist, hashSize);
    }

    //delete the contents in the old hash table and set it to nullptr so it
    //can be updated to the newley created hash table
    deletePlaylist();
    playlist = newPlaylist;
}

bool HashTable::isPrime(int hashSize)
{
    for  (int i = 2; i < hashSize; i++)  {
        if  ((hashSize % i) == 0)  {
            return false;
        }
    }

    return true;
}

bool HashTable::checkAuthenticity(int encryption1, int encryption2)
{
    return playlist[hashFunction(encryption1)].checkAlreadyCreated(encryption1, encryption2);
}

bool HashTable::calcLoadFactor(int numUsers, int hashSize)
{
    //calculate load factor with numMovies and tableSize as a double as we do
    //not want to round load factor
    double loadFactor = (double)numUsers / (double)hashSize;

    if  (loadFactor > 0.7)  {
        return true;
    }

    return false;
}

void HashTable::changePassword(int key_input, QString password_input, int new_encryption)
{
    playlist[hashFunction(key_input)].changePassword_LL(key_input, password_input, new_encryption);
}

bool HashTable::checkAuthenticityChangePassword(int key_input, QString email_input)
{
    return playlist[hashFunction(key_input)].checkAuthenticityChangePassword_LL(key_input, email_input);
}

QString HashTable::getFirstName(int key)
{
    return playlist[hashFunction(key)].getFirstName_LL(key);
}

QString HashTable::getLastName(int key)
{
    return playlist[hashFunction(key)].getLastName_LL(key);
}

QString HashTable::getUsername(int key)
{
    return playlist[hashFunction(key)].getUsername_LL(key);
}

QString HashTable::getAddress(int key)
{
    return playlist[hashFunction(key)].getAddress_LL(key);
}

QString HashTable::getCity(int key)
{
    return playlist[hashFunction(key)].getCity_LL(key);
}

int HashTable::getZip(int key)
{
    return playlist[hashFunction(key)].getZip_LL(key);
}

quint64 HashTable::getPhone(int key)
{
    return playlist[hashFunction(key)].getPhone_LL(key);
}

QString HashTable::getEmail(int key)
{
    return playlist[hashFunction(key)].getEmail_LL(key);
}

