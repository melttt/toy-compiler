#ifndef _LIST_H
#define _LIST_H


typedef struct _LinkedListNode                  // A linked list node
{
    void * pData;                               // Pointer to the node's data
    struct _LinkedListNode * pNext;                    // Pointer to the next node in the list
}LinkedListNode;


typedef struct _LinkedList                      // A linked list
{
    LinkedListNode* pHead,                     // Pointer to head node
                  * pTail;                     // Pointer to tail nail node
    int iNodeCount;                             // The number of nodes in the list
}LinkedList;


void InitLinkedList ( LinkedList * pList );
void FreeLinkedList ( LinkedList * pList );
int AddNode ( LinkedList * pList, void * pData );
void DelNode ( LinkedList * pList, LinkedListNode * pNode );
int AddString ( LinkedList * pList, char * pstrString );
char* GetStringByIndex ( LinkedList * pList, int iIndex );
#endif
