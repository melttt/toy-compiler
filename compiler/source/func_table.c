#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#include "func_table.h"

FuncNode * GetFuncByIndex ( int iIndex )
{
    if ( ! g_FuncTable.iNodeCount )
        return NULL;
    LinkedListNode * pCurrNode = g_FuncTable.pHead;
    int iCurrNode;
    for (iCurrNode = 1; iCurrNode <= g_FuncTable.iNodeCount; ++ iCurrNode )
    {
        FuncNode * pCurrFunc = ( FuncNode * ) pCurrNode->pData;
        if ( iIndex == pCurrFunc->iIndex )
            return pCurrFunc;
        pCurrNode = pCurrNode->pNext;
    }
    return NULL;
}

FuncNode * GetFuncByName ( char * pstrName )
{
    FuncNode * pCurrFunc;
    int iCurrFuncIndex;
    for (iCurrFuncIndex = 1; iCurrFuncIndex <= g_FuncTable.iNodeCount; ++ iCurrFuncIndex )
    {
        pCurrFunc = GetFuncByIndex ( iCurrFuncIndex );
        if ( pCurrFunc && strcasecmp ( pCurrFunc->pstrName, pstrName ) == 0 )
            return pCurrFunc;
    }
    return NULL;
}


int AddFunc ( char * pstrName, int iIsHostAPI )
{
    if ( GetFuncByName ( pstrName ) )
        return -1;
    FuncNode * pNewFunc = ( FuncNode * ) malloc ( sizeof ( FuncNode ) );
    strcpy ( pNewFunc->pstrName, pstrName );
    int iIndex = AddNode ( & g_FuncTable, pNewFunc ) + 1;
    pNewFunc->iIndex = iIndex;
    pNewFunc->iIsHostAPI = iIsHostAPI;
    pNewFunc->iParamCount = 0;
    pNewFunc->ICodeStream.iNodeCount = 0;
    if ( strcasecmp ( pstrName, MAIN_FUNC_NAME ) == 0 )
    {
        g_ScriptHeader.iIsMainFuncPresent = TRUE;
        g_ScriptHeader.iMainFuncIndex = iIndex;
    }
    return iIndex;
}

void SetFuncParamCount ( int iIndex, int iParamCount )
{
    FuncNode * pFunc = GetFuncByIndex ( iIndex );
    pFunc->iParamCount = iParamCount;
}
