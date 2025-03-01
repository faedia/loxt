#include "ll.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct LinkedListNode
{
    LinkedListNode *previous;
    LinkedListNode *next;
    char *data;
};

LinkedList llnew()
{
    LinkedListNode *head = (LinkedListNode *)malloc(sizeof(LinkedListNode));
    LinkedListNode *tail = (LinkedListNode *)malloc(sizeof(LinkedListNode));

    head->previous = NULL;
    head->next = tail;
    head->data = NULL;

    tail->previous = head;
    tail->next = NULL;
    tail->data = NULL;
    LinkedList ll = {head, tail};
    return ll;
}

void llfree(LinkedList *ll)
{
    LinkedListNode *current = ll->head;
    while (current)
    {
        LinkedListNode *next = current->next;
        free(current);
        current = next;
    }
    ll->head = NULL;
    ll->tail = NULL;
}

void llappend(LinkedList *ll, char *item)
{
    llinsert(ll->tail, item);
}

void llprepend(LinkedList *ll, char *item)
{
    llinsert(ll->head->next, item);
}

void llinsert(LinkedListNode *before, char *item)
{
    LinkedListNode *new_node = (LinkedListNode *)malloc(sizeof(LinkedListNode));
    new_node->previous = before->previous;
    new_node->next = before;
    new_node->data = item;

    before->previous->next = new_node;
    before->previous = new_node;
}

void llremove(LinkedListNode *pos)
{
    LinkedListNode *prev = pos->previous;
    LinkedListNode *next = pos->next;

    prev->next = next;
    next->previous = prev;
    free(pos);
}

char *llelement(LinkedListNode *pos)
{
    return pos->data;
}

LinkedListNode *llfind(LinkedList *ll, const char *item)
{
    LinkedListNode *current = ll->head->next;
    while (current)
    {
        if (current->data && strcmp(current->data, item) == 0)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int main()
{
    LinkedList ll = llnew();
    for (int i = 0; i < 10000000; i++)
    {
        llprepend(&ll, strdup("Hello, World!"));
        LinkedListNode *node = llfind(&ll, "Hello, World!");
        printf("%s\n", llelement(node));
    }

    llfree(&ll);
    return 0;
}