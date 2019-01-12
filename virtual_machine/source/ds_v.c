#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ds_v.h"
#include "../../assembler/source/token.h"
Script g_Script;

void _strupr(char *str)
{
    while(*str != 0)
    {
        if(*str >= 'a' && *str <= 'z')
            *str -= 32;
        str ++;
    }
}
/******************************************************************************************
 *
 *	GetOpType ()
 *
 *	Returns the type of the specified operand in the current instruction.
*/
int GetOpType ( int iOpIndex )
{
    // Get the current instruction
    int iCurrInstr = g_Script.InstrStream.iCurrInstr;
    // Return the type
    return g_Script.InstrStream.pInstrs [ iCurrInstr ].pOpList [ iOpIndex ].iType;
}

int ResolveOpType ( int iOpIndex )
{
    // Resolve the operand's value
    Value OpValue = ResolveOpValue ( iOpIndex );
    // Return the value type
    return OpValue.iType;
}

Value GetStackValue ( int iIndex )
{
    // Use ResolveStackIndex () to return the element at the specified index
    return g_Script.Stack.pElmnts [ ResolveStackIndex ( iIndex ) ];
}

void SetStackValue ( int iIndex, Value Val )
{
    // Use ResolveStackIndex () to set the element at the specified index
    g_Script.Stack.pElmnts [ ResolveStackIndex ( iIndex ) ] = Val;
}

int ResolveOpStackIndex ( int iOpIndex )
{
    // Get the current instruction
    int iCurrInstr = g_Script.InstrStream.iCurrInstr;
    // Get the operand type type
    Value OpValue = g_Script.InstrStream.pInstrs [ iCurrInstr ].pOpList [ iOpIndex ];
    // Resolve the stack index based on its type
    switch ( OpValue.iType )
    {
        // It's an absolute index so return it as-is
        case OP_TYPE_ABS_STACK_INDEX:
            return OpValue.iStackIndex;
            // It's a relative index so resolve it
        case OP_TYPE_REL_STACK_INDEX:
        {
            // First get the base index
            int iBaseIndex = OpValue.iStackIndex;
            // Now get the index of the variable
            int iOffsetIndex = OpValue.iOffsetIndex;
            // Get the variable's value
            Value StackValue = GetStackValue ( iOffsetIndex );
            // Now add the variable's integer field to the base index to produce the
            // absolute index
            return iBaseIndex + StackValue.iIntLiteral;
        }
            // Return zero for everything else, but we shouldn't encounter this case
        default:
            return 0;
    }
}

Value * ResolveOpPntr ( int iOpIndex )
{
    // Get the method of indirection
    int iIndirMethod = GetOpType ( iOpIndex );
    // Return a pointer to wherever the operand lies
    switch ( iIndirMethod )
    {
        // It's _RetVal
        case OP_TYPE_REG:
            return & g_Script._RetVal;
            // It's on the stack
        case OP_TYPE_ABS_STACK_INDEX:
        case OP_TYPE_REL_STACK_INDEX:
            {
                int iStackIndex = ResolveOpStackIndex ( iOpIndex );
                return & g_Script.Stack.pElmnts [ ResolveStackIndex ( iStackIndex ) ];
            }
    }
    // Return NULL for anything else
    return NULL;
}
/******************************************************************************************
 *
 *	ResolveOpValue ()
 *
 *	Resolves an operand and returns it's associated Value structure.
*/

Value ResolveOpValue ( int iOpIndex )
{
    // Get the current instruction

    int iCurrInstr = g_Script.InstrStream.iCurrInstr;

    // Get the operand type type
    Value OpValue = g_Script.InstrStream.pInstrs [ iCurrInstr ].pOpList [ iOpIndex ];
    // Determine what to return based on the value's type
    switch ( OpValue.iType )
    {
        // It's a stack index so resolve it
        case OP_TYPE_ABS_STACK_INDEX:
        case OP_TYPE_REL_STACK_INDEX:
        {
                // Resolve the index and use it to return the corresponding stack element
            int iAbsIndex = ResolveOpStackIndex ( iOpIndex );
            return GetStackValue ( iAbsIndex );
        }
            // It's in _RetVal
        case OP_TYPE_REG:
            return g_Script._RetVal;
            // Anything else can be returned as-is
        default:
            return OpValue;
    }

}

/******************************************************************************************
 *
 *   CoereceValueToInt ()
 *
 *   Coerces a Value structure from it's current type to an integer value.
 */
int CoerceValueToInt ( Value Val )
{
    // Determine which type the Value currently is
    switch ( Val.iType )
    {
        // It's an integer, so return it as-is
        case OP_TYPE_INT:
            return Val.iIntLiteral;
            // It's a float, so cast it to an integer
        case OP_TYPE_FLOAT:
            return ( int ) Val.fFloatLiteral;
            // It's a string, so convert it to an integer
        case OP_TYPE_STRING:
            return atoi ( Val.pstrStringLiteral );
            // Anything else is invalid
        default:
            return 0;
    }
}


/******************************************************************************************
 *
 *   CoereceValueToFloat ()
 *
 *   Coerces a Value structure from it's current type to an float value.
 */
float CoerceValueToFloat ( Value Val )
{
    // Determine which type the Value currently is
    switch ( Val.iType )
    {
        // It's an integer, so cast it to a float
        case OP_TYPE_INT:
            return ( float ) Val.iIntLiteral;
            // It's a float, so return it as-is
        case OP_TYPE_FLOAT:
            return Val.fFloatLiteral;
            // It's a string, so convert it to an float
        case OP_TYPE_STRING:
            return ( float ) atof ( Val.pstrStringLiteral );
            // Anything else is invalid
        default:
            return 0;
    }
}

/******************************************************************************************
 *
 *   CoereceValueToString ()
 *
 *   Coerces a Value structure from it's current type to a string value.
 */
char * CoerceValueToString ( Value Val )
{
    char * pstrCoercion;
    if ( Val.iType != OP_TYPE_STRING )
    {
        pstrCoercion = ( char * ) malloc ( MAX_COERCION_STRING_SIZE + 1 );
    }
    // Determine which type the Value currently is
    switch ( Val.iType )
    {
        // It's an integer, so convert it to a string
        case OP_TYPE_INT:
            sprintf ( pstrCoercion, "%d", Val.iIntLiteral );
            return pstrCoercion;
            // It's a float, so use sprintf () to convert it since there's no built-in function
            // for converting floats to strings
        case OP_TYPE_FLOAT:
            sprintf ( pstrCoercion, "%f", Val.fFloatLiteral );
            return pstrCoercion;
            // It's a string, so return it as-is
        case OP_TYPE_STRING:
            return Val.pstrStringLiteral;
            // Anything else is invalid
        default:
            return NULL;
    }
}


int ResolveOpAsInstrIndex ( int iOpIndex )
{
    // Resolve the operand's value
    Value OpValue = ResolveOpValue ( iOpIndex );
    // Return it's instruction index
    return OpValue.iInstrIndex;
}

int ResolveOpAsInt ( int iOpIndex )
{
    // Resolve the operand's value
    Value OpValue = ResolveOpValue ( iOpIndex );
    // Coerce it to an int and return it
    int iInt = CoerceValueToInt ( OpValue );
    return iInt;
}

float ResolveOpAsFloat ( int iOpIndex )
{
    // Resolve the operand's value
    Value OpValue = ResolveOpValue ( iOpIndex );
    // Coerce it to a float and return it
    float fFloat = CoerceValueToFloat ( OpValue );
    return fFloat;
}

Func GetFunc ( int iIndex )
{
    return g_Script.pFuncTable [ iIndex ];
}
char * ResolveOpAsString ( int iOpIndex )
{
    // Resolve the operand's value
    Value OpValue = ResolveOpValue ( iOpIndex );
    // Coerce it to a string and return it
    char * pstrString = CoerceValueToString ( OpValue );
    return pstrString;
}

int ResolveOpAsFuncIndex ( int iOpIndex )
{
    // Resolve the operand's value
    Value OpValue = ResolveOpValue ( iOpIndex );
    // Return the function index
    return OpValue.iFuncIndex;
}


char * GetHostAPICall ( int iIndex )
{
    return g_Script.HostAPICallTable.ppstrCalls [ iIndex ];
}
char * ResolveOpAsHostAPICall ( int iOpIndex )
{
    // Resolve the operand's value
    Value OpValue = ResolveOpValue ( iOpIndex );
    // Get the value's host API call index
    int iHostAPICallIndex = OpValue.iHostAPICallIndex;
    // Return the host API call
    return GetHostAPICall ( iHostAPICallIndex );
}

int RegisterHostFunc(char *ptrName, int iCount,func ptrFunc )
{
    HostFunc *table = g_Script.HostAPICallTable.ptrHostFuncTable;
    if(g_Script.HostAPICallTable.iFuncNum >= MAXHOSTFUNC)
        return -1;
    printf("table index;%d\n", g_Script.HostAPICallTable.iFuncNum);
    printf("p;%p\n",table[g_Script.HostAPICallTable.iFuncNum].ptrFuncName );
    table[g_Script.HostAPICallTable.iFuncNum].fHostfunc = ptrFunc;
    table[g_Script.HostAPICallTable.iFuncNum].iParamCount = iCount;
    table[g_Script.HostAPICallTable.iFuncNum].ptrFuncName = (char*)malloc(strlen(ptrName) + 1);
    strcpy(table[g_Script.HostAPICallTable.iFuncNum].ptrFuncName, ptrName);
    _strupr(table[g_Script.HostAPICallTable.iFuncNum].ptrFuncName);
    return g_Script.HostAPICallTable.iFuncNum ++;
}

int GetParamAsInt(int iParamIndex)
{
    int iTopIndex = g_Script.Stack.iTopIndex;
    Value Param = g_Script.Stack.pElmnts[iTopIndex - (iParamIndex + 1)];
    return CoerceValueToInt(Param);
}

int GetParamAsFloat(int iParamIndex)
{
    int iTopIndex = g_Script.Stack.iTopIndex;
    Value Param = g_Script.Stack.pElmnts[iTopIndex - (iParamIndex + 1)];
    return CoerceValueToFloat(Param);
}

char*  GetParamAsString(int iParamIndex)
{
    int iTopIndex = g_Script.Stack.iTopIndex;
    Value Param = g_Script.Stack.pElmnts[iTopIndex - (iParamIndex + 1)];
    return CoerceValueToString(Param);
}

Value GetParamAsValue(int iParamIndex)
{
    int iTopIndex = g_Script.Stack.iTopIndex;
    return  g_Script.Stack.pElmnts[iTopIndex - (iParamIndex + 1)];
}


int ReturnIntFromHost(int iRet, int iParamCount)
{
    g_Script.Stack.iTopIndex -= iParamCount;
    g_Script._RetVal.iType = OP_TYPE_INT;
    g_Script._RetVal.iIntLiteral = iRet;
}

int ReturnFloatFromHost(int iRet, int iParamCount)
{
    g_Script.Stack.iTopIndex -= iParamCount;
    g_Script._RetVal.iType = OP_TYPE_FLOAT;
    g_Script._RetVal.fFloatLiteral = iRet;
}
int ReturnStringFromHost(char* ch, int iParamCount)
{
    g_Script.Stack.iTopIndex -= iParamCount;
    Value val;
    val.iType = OP_TYPE_STRING;
    val.pstrStringLiteral = ch;
    CopyValue(&g_Script._RetVal ,val);
}


//stack
void Push ( Value Val )
{
    // Get the current top element
    int iTopIndex = g_Script.Stack.iTopIndex;
    // Put the value into the current top index
    g_Script.Stack.pElmnts [ iTopIndex ] = Val;
    // Increment the top index
    ++ g_Script.Stack.iTopIndex;
}

void PushFrame ( int iSize )
{
    // Increment the top index by the size of the frame
    g_Script.Stack.iTopIndex += iSize;
    // Move the frame index to the new top of the stack
    g_Script.Stack.iFrameIndex = g_Script.Stack.iTopIndex;
}

void CopyValue ( Value * pDest, Value Source )
{
    // If the destination already contains a string, make sure to free it first
    if ( pDest->iType == OP_TYPE_STRING )
    {
        printf("YES");
        free(pDest->pstrStringLiteral );
        pDest->pstrStringLiteral = NULL;
    }
    // Copy the object
    *pDest = Source;
    // Make a physical copy of the source string, if necessary
    if ( Source.iType == OP_TYPE_STRING )
    {
        pDest->pstrStringLiteral = ( char * ) malloc ( strlen ( Source.pstrStringLiteral ) + 1 );
        strcpy ( pDest->pstrStringLiteral, Source.pstrStringLiteral );

    }

}

Value Pop ()
{
    // Decrement the top index to clear the old element for overwriting
    -- g_Script.Stack.iTopIndex;
    // Get the current top element
    int iTopIndex = g_Script.Stack.iTopIndex;
    // Use this index to read the top element
    Value Val;
    Val.iType = OP_TYPE_NULL;

    CopyValue ( & Val, g_Script.Stack.pElmnts [ iTopIndex ] );
    // Return the value to the caller
    return Val;
}

void PopFrame ( int iSize )
{
    // Decrement the top index by the size of the frame
    g_Script.Stack.iTopIndex -= iSize;
}


