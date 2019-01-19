#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "i_code.h"

int g_iCurrJumpTargetIndex = 0;                     // The current target index
ICodeNode * GetICodeNodeByImpIndex ( int iFuncIndex, int iInstrIndex )
{
    FuncNode * pFunc = GetFuncByIndex ( iFuncIndex );
    if ( ! pFunc->ICodeStream.iNodeCount )
        return NULL;
    LinkedListNode * pCurrNode = pFunc->ICodeStream.pHead;

    int iCurrNode;
    for ( iCurrNode = 0; iCurrNode < pFunc->ICodeStream.iNodeCount; ++ iCurrNode )
    {
        if ( iInstrIndex == iCurrNode )
            return ( ICodeNode * ) pCurrNode->pData;
        pCurrNode = pCurrNode->pNext;
    }
    return NULL;
}
void AddICodeSourceLine ( int iFuncIndex, char * pstrSourceLine )
{
    FuncNode * pFunc = GetFuncByIndex ( iFuncIndex );
    ICodeNode * pSourceLineNode = ( ICodeNode * ) malloc ( sizeof ( ICodeNode ) );

    pSourceLineNode->iType = ICODE_NODE_SOURCE_LINE;

    pSourceLineNode->pstrSourceLine = pstrSourceLine;

    AddNode ( & pFunc->ICodeStream, pSourceLineNode );
}


int AddICodeInstr ( int iFuncIndex, int iOpcode )
{
    FuncNode * pFunc = GetFuncByIndex ( iFuncIndex );


    ICodeNode * pInstrNode = ( ICodeNode * ) malloc ( sizeof ( ICodeNode ) );


    pInstrNode->iType = ICODE_NODE_INSTR;


    pInstrNode->Instr.iOpcode = iOpcode;

    pInstrNode->Instr.OpList.iNodeCount = 0;

    int iIndex = AddNode ( & pFunc->ICodeStream, pInstrNode );

    return iIndex;
}


Op * GetICodeOpByIndex ( ICodeNode * pInstr, int iOpIndex )
{


    if ( ! pInstr->Instr.OpList.iNodeCount )
        return NULL;


    LinkedListNode * pCurrNode = pInstr->Instr.OpList.pHead;


    int iCurrNode;
    for ( iCurrNode = 0; iCurrNode < pInstr->Instr.OpList.iNodeCount; ++ iCurrNode )
    {
        if ( iOpIndex == iCurrNode )
            return ( Op * ) pCurrNode->pData;

        pCurrNode = pCurrNode->pNext;
    }

    return NULL;
}
void AddICodeOp ( int iFuncIndex, int iInstrIndex, Op Value )
{

    ICodeNode * pInstr = GetICodeNodeByImpIndex ( iFuncIndex, iInstrIndex );
    Op * pValue = ( Op * ) malloc ( sizeof ( Op ) );
    memcpy ( pValue, & Value, sizeof ( Op ) );


    AddNode ( & pInstr->Instr.OpList, pValue );

}


void AddIntICodeOp ( int iFuncIndex, int iInstrIndex, int iValue )
{
    Op Value;

    Value.iType = OP_TYPE_INT;
    Value.iIntLiteral = iValue;

    AddICodeOp ( iFuncIndex, iInstrIndex, Value );
}


void AddFloatICodeOp ( int iFuncIndex, int iInstrIndex, float fValue )
{
    Op Value;


    Value.iType = OP_TYPE_FLOAT;
    Value.fFloatLiteral = fValue;

    AddICodeOp ( iFuncIndex, iInstrIndex, Value );
}

void AddStringICodeOp ( int iFuncIndex, int iInstrIndex, int iStringIndex )
{

    Op Value;

    Value.iType = OP_TYPE_STRING_INDEX;
    Value.iStringIndex = iStringIndex;
    AddICodeOp ( iFuncIndex, iInstrIndex, Value );
}
void AddVarICodeOp ( int iFuncIndex, int iInstrIndex, int iSymbolIndex )
{

    Op Value;


    Value.iType = OP_TYPE_VAR;
    Value.iSymbolIndex = iSymbolIndex;

    AddICodeOp ( iFuncIndex, iInstrIndex, Value );
}

void AddArrayIndexAbsICodeOp ( int iFuncIndex, int iInstrIndex, int iArraySymbolIndex, int iOffset )
{

    Op Value;
    Value.iType = OP_TYPE_ARRAY_INDEX_ABS;
    Value.iSymbolIndex = iArraySymbolIndex;
    Value.iOffset = iOffset;
    AddICodeOp ( iFuncIndex, iInstrIndex, Value );
}

void AddArrayIndexVarICodeOp ( int iFuncIndex, int iInstrIndex, int iArraySymbolIndex, int iOffsetSymbolIndex )
{

    Op Value;

    Value.iType = OP_TYPE_ARRAY_INDEX_VAR;
    Value.iSymbolIndex = iArraySymbolIndex;
    Value.iOffsetSymbolIndex = iOffsetSymbolIndex;

    AddICodeOp ( iFuncIndex, iInstrIndex, Value );
}


void AddFuncICodeOp ( int iFuncIndex, int iInstrIndex, int iOpFuncIndex )
{

    Op Value;

    Value.iType = OP_TYPE_FUNC_INDEX;
    Value.iFuncIndex = iOpFuncIndex;
    AddICodeOp ( iFuncIndex, iInstrIndex, Value );
}


void AddRegICodeOp ( int iFuncIndex, int iInstrIndex, int iRegCode )
{

    Op Value;


    Value.iType = OP_TYPE_REG;
    Value.iRegCode = iRegCode;

    AddICodeOp ( iFuncIndex, iInstrIndex, Value );
}


void AddJumpTargetICodeOp ( int iFuncIndex, int iInstrIndex, int iTargetIndex )
{

    Op Value;
    Value.iType = OP_TYPE_JUMP_TARGET_INDEX;
    Value.iJumpTargetIndex = iTargetIndex;
    AddICodeOp ( iFuncIndex, iInstrIndex, Value );
}

int GetNextJumpTargetIndex ()
{
    return g_iCurrJumpTargetIndex ++;
}

void AddICodeJumpTarget ( int iFuncIndex, int iTargetIndex )
{

    FuncNode * pFunc = GetFuncByIndex ( iFuncIndex );
    ICodeNode * pSourceLineNode = ( ICodeNode * ) malloc ( sizeof ( ICodeNode ) );

    pSourceLineNode->iType = ICODE_NODE_JUMP_TARGET;
    pSourceLineNode->iJumpTargetIndex = iTargetIndex;

    AddNode ( & pFunc->ICodeStream, pSourceLineNode );

}
