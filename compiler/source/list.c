#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"



void InitLinkedList ( LinkedList * pList )
{
    // Set both the head and tail pointers to null
    pList->pHead = NULL;
    pList->pTail = NULL;
    // Set the node count to zero, since the list is currently empty
    pList->iNodeCount = 0;
}


void FreeLinkedList ( LinkedList * pList )
{
    if ( ! pList )
        return;
    if ( pList->iNodeCount )
    {
        LinkedListNode * pCurrNode,
                       * pNextNode;
        pCurrNode = pList->pHead;
        while (1)
        {
            pNextNode = pCurrNode->pNext;
            if ( pCurrNode->pData )
                free ( pCurrNode->pData );
            if ( pCurrNode )
                free ( pCurrNode );
            if ( pNextNode )
                pCurrNode = pNextNode;
            else
                break;
        }
    }
}



int AddNode (LinkedList* pList, void* pData )
{
    LinkedListNode * pNewNode = ( LinkedListNode * ) malloc ( sizeof ( LinkedListNode ) );
    pNewNode->pData = pData;
    pNewNode->pNext = NULL;
    if ( ! pList->iNodeCount )
    {
        pList->pHead = pNewNode;
        pList->pTail = pNewNode;
    }
    else
    {
        pList->pTail->pNext = pNewNode;
        pList->pTail = pNewNode;
    }
    ++ pList->iNodeCount;
    return pList->iNodeCount - 1;
}


void DelNode ( LinkedList * pList, LinkedListNode * pNode )
{
    if ( pList->iNodeCount == 0 )
        return;
    if ( pNode == pList->pHead )
    {
        pList->pHead = pNode->pNext;
    }
    else
    {
        LinkedListNode * pTravNode = pList->pHead;
        int iCurrNode;
        for (iCurrNode = 0; iCurrNode < pList->iNodeCount; ++ iCurrNode )
        {
            if ( pTravNode->pNext == pNode )
            {
                if ( pList->pTail == pNode )
                {
                    pTravNode->pNext = NULL;
                    pList->pTail = pTravNode;
                }
                else
                {
                    pTravNode->pNext = pNode->pNext;
                }    
                break;
            }
            pTravNode = pTravNode->pNext;
        }
    }
    -- pList->iNodeCount;
    if ( pNode->pData )
        free ( pNode->pData );
    free ( pNode );

}

int AddString ( LinkedList * pList, char * pstrString )
{
    LinkedListNode * pNode = pList->pHead;
    int iCurrNode;
    for (iCurrNode = 0; iCurrNode < pList->iNodeCount; ++ iCurrNode )
    {
        if ( strcmp ( ( char * ) pNode->pData, pstrString ) == 0 )
            return iCurrNode;
        pNode = pNode->pNext;
    }
    char * pstrStringNode = ( char * ) malloc ( strlen ( pstrString ) + 1 );
    strcpy ( pstrStringNode, pstrString );
    return AddNode ( pList, pstrStringNode );
}


char * GetStringByIndex ( LinkedList * pList, int iIndex )
{
    LinkedListNode * pNode = pList->pHead;
    for ( int iCurrNode = 0; iCurrNode < pList->iNodeCount; ++ iCurrNode )
    {

        if ( iIndex == iCurrNode )
            return ( char * ) pNode->pData;
        pNode = pNode->pNext;
    }
    return NULL;
}
