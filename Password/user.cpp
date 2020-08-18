#include "user.h"
#include "hashtable.h"
#include "passwordstorage.h"
#include "QTextStream"
#include <QDebug>

User::User()
{
    head = nullptr;
}

User::~User()
{
    if  (!head)  {
        deleteUser();
        head = nullptr;
    }
}

User::User(QString username_input, QString password_input, QString firstName_input, QString lastName_input, QString address_input, QString city_input, int zip_input, quint64 phone_input, QString email_input)
{
    username = username_input;
    password = password_input;
    firstName = firstName_input;
    lastName = lastName_input;
    address = address_input;
    city = city_input;
    zip = zip_input;
    phone = phone_input;
    email = email_input;
}

void User::deleteUser()
{
    UserNode* deletePtr = nullptr;
    UserNode* temp = nullptr;

    //if list is empty return, as nothing must be deleted
    if  (isEmpty())  {
        return;
    }

    //else, delete the node at the head pointer and replace with head->next
    //until the list is empty
    else  {
        deletePtr = head;

        while  (deletePtr)  {
            temp = deletePtr->next;
            delete deletePtr;
            deletePtr = temp;
        }
    }

}

bool User::isEmpty()
{
    if  (head == nullptr)  {
        return true;
    }

    else  {
        return false;
    }
}

void User::appendNode(User* obj, int key_input, int encryption_input)
{
    UserNode* appendPtr = nullptr;
    UserNode* newNode = nullptr;
    newNode = new UserNode;
    newNode->username_key = key_input;
    newNode->encryption = encryption_input;
    newNode->nodeUser = obj;
    newNode->next = nullptr;

    //if list is empty, set head to newNode
    if  (isEmpty())  {
        head = newNode;
        return;
    }

    else  {
        appendPtr = head;
        //find last node in linked list
        while  (appendPtr->next)  {
            appendPtr = appendPtr->next;
        }

        //set next pointer of the last node to newNode
        appendPtr->next = newNode;
    }
}

void User::appendNode(UserNode* newMovieNode)
{
    UserNode* appendPtr = nullptr;
    //create new node and set it equal to passed in node
    UserNode* newNode = nullptr;
    newNode = newMovieNode;
    newNode->next = nullptr;

    //if linked list is empty, set head to newNode
    if  (isEmpty())  {
        head = newNode;
        return;
    }

    else  {
        appendPtr = head;

        //find last node in list
        while  (appendPtr->next)  {
            appendPtr = appendPtr->next;
        }

        //set last node's next pointer to newNode
        appendPtr->next = newNode;
    }
}

void User::rehashNodes(User* newPlaylist, int tableSize)
{
    UserNode* copyPtr = nullptr;
    UserNode* previousPtr = nullptr;

    //if list is empty, no nodes to copy to new hash table
    if  (isEmpty())  {
        return;
    }

    else  {
        copyPtr = head;

        //traverse the linked list and find the correct index for each node
        //and copy said node into the linked list at the calculated index of
        //the new hash table
        while  (copyPtr)  {
            previousPtr = copyPtr->next;
            int newIndex = (copyPtr->username_key) % tableSize;
            newPlaylist[newIndex].appendNode(copyPtr);
            copyPtr = previousPtr;
        }
    }
}

bool User::checkAlreadyCreated(int encryption1, int encryption2)
{
    if  (isEmpty())  {
        return false;
    }

    else  {
        if  ((head->username_key == encryption1) && (head->encryption == encryption2))  {
            return true;
        }

        else    {
            UserNode* users = head;

            while  (users)  {
                if  ((users->username_key = encryption1) && (users->encryption == encryption2))  {
                    return true;
                }

                users = users->next;
            }
        }
    }

    return false;
}

QString User::getFirstName_LL(int key_input)
{
    if (isEmpty())  {
        return "";
    }

    else  {
        UserNode* findKey = head;

        while  (findKey)  {
            if  (findKey->username_key == key_input)  {
                return head->nodeUser->firstName;
            }

            findKey = findKey->next;
        }
    }

    return "";
}

QString User::getLastName_LL(int key_input)
{
    if  (isEmpty())  {
        return "";
    }

    else    {
        UserNode* traverse = head;

        while  (traverse)  {
            if  (traverse->username_key == key_input)  {
                return traverse->nodeUser->lastName;
            }

            traverse = traverse->next;
        }
    }

    return "";
}

QString User::getUsername_LL(int key_input)
{
    if  (isEmpty())  {
        return "";
    }

    else    {
        UserNode* traverse = head;

        while  (traverse)  {
            if  (traverse->username_key == key_input)  {
                return traverse->nodeUser->username;
            }

            traverse = traverse->next;
        }
    }

    return "";
}

QString User::getAddress_LL(int key_input)
{
    if  (isEmpty())  {
        return "";
    }

    else    {
        UserNode* traverse = head;

        while  (traverse)  {
            if  (traverse->username_key == key_input)  {
                return traverse->nodeUser->address;
            }

            traverse = traverse->next;
        }
    }

    return "";
}

QString User::getCity_LL(int key_input)
{
    if  (isEmpty())  {
        return "";
    }

    else    {
        UserNode* traverse = head;

        while  (traverse)  {
            if  (traverse->username_key == key_input)  {
                return traverse->nodeUser->city;
            }

            traverse = traverse->next;
        }
    }

    return "";
}

int User::getZip_LL(int key_input)
{
    if  (isEmpty())  {
        return 0;
    }

    else    {
        UserNode* traverse = head;

        while  (traverse)  {
            if  (traverse->username_key == key_input)  {
                return traverse->nodeUser->zip;
            }

            traverse = traverse->next;
        }
    }

    return 0;
}

quint64 User::getPhone_LL(int key_input)
{
    if  (isEmpty())  {
        return 0;
    }

    else    {
        UserNode* traverse = head;

        while  (traverse)  {
            if  (traverse->username_key == key_input)  {
                qDebug() << traverse->nodeUser->phone;
                return traverse->nodeUser->phone;
            }

            traverse = traverse->next;
        }
    }

    return 0;
}

QString User::getEmail_LL(int key_input)
{
    if  (isEmpty())  {
        return "";
    }

    else    {
        UserNode* traverse = head;

        while  (traverse)  {
            if  (traverse->username_key == key_input)  {
                return traverse->nodeUser->email;
            }

            traverse = traverse->next;
        }
    }

    return "";
}

void User::changePassword_LL(int key, QString new_encryption, int new_encryption_key)
{
    if  (isEmpty())  {
        return;
    }

    else  {
        if  (head->username_key == key)  {
            head->nodeUser->password = new_encryption;
            head->encryption = new_encryption_key;
        }

        UserNode* traverse = nullptr;

        while  (traverse)  {
            if  (traverse->username_key == key)  {
                traverse->nodeUser->password = new_encryption;
                traverse->encryption = new_encryption_key;
            }

            traverse = traverse->next;
        }
    }

    return;
}

bool User::checkAuthenticityChangePassword_LL(int key, QString email)
{
    if  (isEmpty())  {
        return false;
    }

    else  {
        if  ((head->username_key == key) && (head->nodeUser->email == email))  {
            return true;
        }

        else    {
            UserNode* users = head;

            while  (users)  {
                if  ((users->username_key = key) && (users->nodeUser->email == email))  {
                    return true;
                }

                users = users->next;
            }
        }
    }

    return false;
}






