#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "symbol_table.h"

SymbolNode * GetSymbolByIndex ( int iIndex )
{
    if ( ! g_SymbolTable.iNodeCount )
        return NULL;
    LinkedListNode * pCurrNode = g_SymbolTable.pHead;
    int iCurrNode;
    for (iCurrNode = 0; iCurrNode < g_SymbolTable.iNodeCount; ++ iCurrNode )
    {
        SymbolNode * pCurrSymbol = ( SymbolNode * ) pCurrNode->pData;
        if ( iIndex == pCurrSymbol->iIndex )
            return pCurrSymbol;
        pCurrNode = pCurrNode->pNext;
    }
    return NULL;
}
SymbolNode * GetSymbolByIdent ( char * pstrIdent, int iScope )
{
    SymbolNode * pCurrSymbol;
    int iCurrSymbolIndex;
    for(iCurrSymbolIndex = 0; iCurrSymbolIndex < g_SymbolTable.iNodeCount; ++ iCurrSymbolIndex )
    {
        pCurrSymbol = GetSymbolByIndex ( iCurrSymbolIndex );
        if ( pCurrSymbol && strcasecmp ( pCurrSymbol->pstrIdent, pstrIdent ) == 0 &&
                ( pCurrSymbol->iScope == iScope || pCurrSymbol->iScope == 0 ) )
            return pCurrSymbol;
    }
    return NULL;
}

int GetSizeByIdent ( char * pstrIdent, int iScope )
{
    SymbolNode * pSymbol = GetSymbolByIdent ( pstrIdent, iScope );
    return pSymbol->iSize;
}

int AddSymbol ( char * pstrIdent, int iSize, int iScope, int iType )
{
    if ( GetSymbolByIdent ( pstrIdent, iScope ) )
        return -1;
    SymbolNode * pNewSymbol = ( SymbolNode * ) malloc ( sizeof ( SymbolNode ) );
    strcpy ( pNewSymbol->pstrIdent, pstrIdent );
    pNewSymbol->iSize = iSize;
    pNewSymbol->iScope = iScope;
    pNewSymbol->iType = iType;
    int iIndex = AddNode ( & g_SymbolTable, pNewSymbol );
    pNewSymbol->iIndex = iIndex;
    return iIndex;

}
