#ifndef XSC_STACK
#define XSC_STACK

#include "list.h"

typedef struct _Stack                           // A stack
{
    LinkedList ElmntList;                       // An internal linked list to hold the
}Stack;

void InitStack ( Stack * pStack );
void FreeStack ( Stack * pStack );
int IsStackEmpty ( Stack * pStack );
void Push ( Stack * pStack, void * pData );
void Pop ( Stack * pStack );
void * Peek ( Stack * pStack );
#endif
