#ifndef USER_H
#define USER_H
#include <QMainWindow>

class User {
    private:
        QString username;
        QString password;
        QString firstName;
        QString lastName;
        QString address;
        QString city;
        int zip;
        quint64 phone;
        QString email;

        struct UserNode  {
            User* nodeUser;
            int username_key;
            int encryption;
            UserNode* next;
        };

        UserNode* head;  //used to store the head of each list pointed to by
                          //ech index in the hash table
        bool isEmpty();
        //description: this private utility function checks if the list is
        //empty
        //preconditions: a list has properly been created
        //postconditions: this will have no effect on any member variables in
        //the class
        void deleteUser();
        //desciption: this function deletes all nodes stored in a linked list
        //(private utility used in the destructor)
        //preconditions: a linked list must have already been created for this
        //function to be necessary
        //postconditions: all nodes in the linked list will have been deleted
        //after this function runs

    public:
        User();
        //description: the default constructor with is used when initializing
        //arrays because it stores a head pointer to a linked list of Movies
        //in each index of the array
        //preconditions: a Movie object has been created
        //postconditions: a head pointer to a list of Movies is created at each
        //index after this function runs
        User(QString username, QString password_input, QString firstName_input, QString lastName_input, QString address_input, QString city_input, int zip_input, quint64 phone_input, QString email_input);
        //description: the parametrized constructor used to create Movie object
        //in main to be stored in the hash table
        //preconditions: a Movie object is created with the correct parameters
        //postconditions: a Movie object has been created which stores the
        //given parameters in it
        ~User();
        //description: destructor which deallocated any dynamically stored
        //memory (nodes in the linked list and the head pointer)
        //preconditions: memory has been dynamically allocated for this
        //function to deallocate
        //postconditions: all dynamically allocated memory will be deallocated
        //after this funcion runs
        void appendNode(User* obj, int key_input, int encryption_input);
        //description: appends a node to the end of a linked list
        //preconditions: a linked list has properly been created the adress
        //of a Movie object has been passed into this function
        //postconditions: a node has been created storing the Movie object
        //passed into this function and it's key and has been appended to the
        //end of this linked list this function is called upon
        void appendNode(UserNode* newMovieNode);
        //description: an overloaded function that appends an already created
        //to the end of a linked list (this is used when copying nodes as
        //nodes to be copied have already been created)
        //preconditions: a linked list has already been created and nodes have
        //been stored into said linked list to be copied to another linked list
        //postconditions: the node of existing linked list that has been passed
        //into this function is appended to the linked list this function is
        //called upon
        //void display();
        //description: this function displays the movie title and id to the
        //console of every node stored in the linked list
        //preconditions: nodes must have been stores in a linked list for
        //this function to be able to print data to the console
        //postconditions: this will have no effect on any member variables in
        //the class
        //bool searchDelete(long key);
        //description: this function searches for a node which stores the given
        //key and deletes said node from the linked list
        //preconditions: nodes must have been stores in a linked list and a node
        //must exist in the list that stores the passed in key for this function
        //to delete a node from the linked list
        //postconditions: assuming there exist a node in the linked list that
        //stores the passed in key, that node will have been deleted from the
        //linked list
        //void displayMovie(long key);
        //description: this function displays information (title, genre, content
        //rating, tagline, director, year and id) to the console stored in the
        //node that holds the passed in key
        //preconditions: nodes must have been stores in a linked list and a node
        //must exist in the list that stores the passed in key for this function
        //to display the contents of a movie
        //postconditions: this will have no effect on any member variables in
        //the class
        //bool printGenre(string inputGenre);
        //description: this function print all movie titles and  id's of the
        //nodes which store the passed in genre
        //preconditions: nodes have been stored in the linked list that hold
        //the passed in genre, or else this funciton won't print anything
        //postconditions: this will have no effect on any member variables in
        //the class
        void rehashNodes(User* newPlaylist, int tableSize);
        //description: this function copies all nodes stored in the existing
        //hash table into the newly created hash table based on the new hash
        //function
        //preconditions: nodes have been stored in the hash table to be copied
        //to the new hash table and the load factor of 0.7 has been exceeded
        //postcontions: the newly created hash table will store all the nodes
        //of the old hash table in the correct indicies
        bool checkAlreadyCreated(int encryption1, int encryption2);
        //int getVal();
        QString getFirstName_LL(int key_input);
        QString getLastName_LL(int key_input);
        QString getUsername_LL(int key_input);
        QString getAddress_LL(int key_input);
        QString getCity_LL(int key_input);
        int getZip_LL(int key_input);
        quint64 getPhone_LL(int key_input);
        QString getEmail_LL(int key_input);
        void changePassword_LL(int key, QString encryption, int new_encryption_key);
        bool checkAuthenticityChangePassword_LL(int key, QString email);
};

#endif
