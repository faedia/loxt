typedef struct LinkedListNode LinkedListNode;

typedef struct LinkedList
{
    LinkedListNode* head;
    LinkedListNode* tail;
} LinkedList;

LinkedList llnew();
void llfree(LinkedList* ll);
void llappend(LinkedList* ll, char* item);
void llprepend(LinkedList* ll, char* item);
void llinsert(LinkedListNode* before, char* item);
void llremove(LinkedListNode* pos);
char* llelement(LinkedListNode* pos);
LinkedListNode* llfind(LinkedList* ll, const char* item);