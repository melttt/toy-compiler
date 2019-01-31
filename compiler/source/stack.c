#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stack.h"

void InitStack ( Stack * pStack )
{
    InitLinkedList ( & pStack->ElmntList );
}
void FreeStack ( Stack * pStack )
{
    FreeLinkedList ( & pStack->ElmntList );
}

int IsStackEmpty ( Stack * pStack )
{
    if ( pStack->ElmntList.iNodeCount > 0 )
        return 0;
    else
        return 1;
}
void Push ( Stack * pStack, void * pData )
{
    AddNode ( & pStack->ElmntList, pData );
}

void Pop ( Stack * pStack )
{
    DelNode ( & pStack->ElmntList, pStack->ElmntList.pTail );
}
void * Peek ( Stack * pStack )
{
    return pStack->ElmntList.pTail->pData;
}

