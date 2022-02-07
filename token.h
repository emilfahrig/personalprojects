#ifndef TOKENQUEUE_H
#define TOKENQUEUE_H

#include <iostream>
#include <string.h>
#include <sstream>
using namespace std;

const int MAXARGS = 20;

class TokenQueue
{
    public:
        TokenQueue();
        ~TokenQueue();

        struct Node {
            char* argsAsToken[MAXARGS];
            Node* next = nullptr;

            Node (string userString)
            {
                stringstream tokenStream(userString);

                // string to temporarily store string command arguments
                // to 
                string tokenItems;

                int numTokens = 0;
                while (getline(tokenStream, tokenItems, ' '))
                {
                    // if string is not empty, store char* into argsAsToken
                    if (tokenItems != "")
                    {
                        argsAsToken[numTokens++] = strdup(tokenItems.c_str());
                    }
                }

                // must set last index to NULL to indicate the end of the char*
                // array
                argsAsToken[numTokens] = NULL;
            }
        };

        Node* head;
        Node* back;

        void pushToken(string valueToPush);
        void popToken(char* args[]);
};

#endif
