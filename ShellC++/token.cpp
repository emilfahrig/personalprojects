#include <iostream>
#include "token.h"
using namespace std;

TokenQueue::TokenQueue()
{
    head = nullptr;
    back = nullptr;
}

TokenQueue::~TokenQueue()
{
    // return if no nodes exits to delete
    if (head == nullptr)
    {
        return;
    }

    Node* trav = head;
    Node* temp = nullptr;

    // traverse the list and delete all nodes
    while (trav != nullptr)
    {
        temp = trav->next;
        delete trav;
        trav = temp;
    }

    head = nullptr;
}

void TokenQueue::pushToken(string valueToPush)
{
    Node* newNode = new Node(valueToPush);

    // if no nodes in list, set head to newNode
    if (head == nullptr)
    {
        head = back = newNode;
    }

    // otherwise append newNode to linked list via back pointer
    else
    {
        back->next = newNode;
        back = newNode;
    }
}

void TokenQueue::popToken(char* args[])
{
    // if list is empty, return
    if (head == nullptr)
    {
        return;
    }

    Node* temp = head->next;

    // copy data stored in argsAsToken to return argument args
    for (int i = 0; i < MAXARGS; i++)
    {
        args[i] = head->argsAsToken[i];
    }

    delete head;
    head = temp;
}
