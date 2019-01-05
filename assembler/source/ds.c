#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ds.h"
#include "utils.h"
//tables
LinkedList g_StringTable;
LinkedList g_FuncTable;
LinkedList g_SymbolTable;
LinkedList g_LabelTable;
LinkedList g_HostAPICallTable;
InstrLookup g_InstrTable[MAX_INSTR_LOOKUP_COUNT];
int g_iInstrTableLength;

Instr* g_pInstrStream = NULL;
int g_iInstrStreamSize;
int g_iCurrInstrIndex;

ScriptHeader g_ScriptHeader;
int g_iIsSetStackSizeFound;

void InitLinkedList ( LinkedList * pList )
{
    // Set both the head and tail pointers to null
    pList->pHead = NULL;
    pList->pTail = NULL;

    // Set the node count to zero, since the list is currently empty

    pList->iNodeCount = 0;
}

int AddNode ( LinkedList * pList, void * pData )
{
    // Create a new node

    LinkedListNode * pNewNode = ( LinkedListNode * ) malloc ( sizeof ( LinkedListNode ) );

    // Set the node's data to the specified pointer

    pNewNode->pData = pData;

    // Set the next pointer to NULL, since nothing will lie beyond it

    pNewNode->pNext = NULL;

    // If the list is currently empty, set both the head and tail pointers to the new node

    if ( ! pList->iNodeCount )
    {
        // Point the head and tail of the list at the node

        pList->pHead = pNewNode;
        pList->pTail = pNewNode;
    }

    // Otherwise append it to the list and update the tail pointer

    else
    {
        // Alter the tail's next pointer to point to the new node

        pList->pTail->pNext = pNewNode;

        // Update the list's tail pointer

        pList->pTail = pNewNode;
    }

    // Increment the node count

    ++ pList->iNodeCount;

    // Return the new size of the linked list - 1, which is the node's index

    return pList->iNodeCount - 1;
}

void FreeLinkedList ( LinkedList * pList )
{
    // If the list is empty, exit

    if ( ! pList )
        return;

    // If the list is not empty, free each node

    if ( pList->iNodeCount )
    {
        // Create a pointer to hold each current node and the next node

        LinkedListNode * pCurrNode,
                       * pNextNode;

        // Set the current node to the head of the list

        pCurrNode = pList->pHead;

        // Traverse the list

        while ( TRUE )
        {
            // Save the pointer to the next node before freeing the current one

            pNextNode = pCurrNode->pNext;

            // Clear the current node's data

            if ( pCurrNode->pData )
                free ( pCurrNode->pData );

            // Clear the node itself

            if ( pCurrNode )
                free ( pCurrNode );

            // Move to the next node if it exists; otherwise, exit the loop

            if ( pNextNode )
                pCurrNode = pNextNode;
            else
                break;
        }
    }
}

//String Table

/******************************************************************************************
 *
 *	AddString ()
 *
 *	Adds a string to a linked list, blocking duplicate entries
 */

int AddString ( LinkedList * pList, char * pstrString )
{
    // ---- First check to see if the string is already in the list

    // Create a node to traverse the list

    LinkedListNode * pNode = pList->pHead;

    // Loop through each node in the list

    for ( int iCurrNode = 0; iCurrNode < pList->iNodeCount; ++ iCurrNode )
    {
        // If the current node's string equals the specified string, return its index

        if ( strcmp ( ( char * ) pNode->pData, pstrString ) == 0 )
            return iCurrNode;

        // Otherwise move along to the next node

        pNode = pNode->pNext;
    }

    // ---- Add the new string, since it wasn't added

    // Create space on the heap for the specified string

    char * pstrStringNode = ( char * ) malloc ( strlen ( pstrString ) + 1 );
    strcpy ( pstrStringNode, pstrString );

    // Add the string to the list and return its index

    return AddNode ( pList, pstrStringNode );
}


//Function Table
/******************************************************************************************
 *
 *   GetFuncByName ()
 *
 *   Returns a FuncNode structure pointer corresponding to the specified name.
 */
FuncNode * GetFuncByName ( char * pstrName )
{
    // If the table is empty, return a NULL pointer
    if ( ! g_FuncTable.iNodeCount )
        return NULL;

    // Create a pointer to traverse the list

    LinkedListNode * pCurrNode = g_FuncTable.pHead;

    // Traverse the list until the matching structure is found

    for ( int iCurrNode = 0; iCurrNode < g_FuncTable.iNodeCount; ++ iCurrNode )
    {
        // Create a pointer to the current function structure

        FuncNode * pCurrFunc = ( FuncNode * ) pCurrNode->pData;

        // If the names match, return the current pointer

        if ( strcmp ( pCurrFunc->pstrName, pstrName ) == 0 )
            return pCurrFunc;

        // Otherwise move to the next node

        pCurrNode = pCurrNode->pNext;
    }

    // The structure was not found, so return a NULL pointer

    return NULL;
}
/******************************************************************************************
 *
 *   SetFuncInfo ()
 *
 *   Fills in the remaining fields not initialized by AddFunc ().
 */

void SetFuncInfo ( char * pstrName, int iParamCount, int iLocalDataSize )
{
    // Based on the function's name, find its node in the list

    FuncNode * pFunc = GetFuncByName ( pstrName );
    // Set the remaining fields
    pFunc->iParamCount = iParamCount;
    pFunc->iLocalDataSize = iLocalDataSize;
}
/******************************************************************************************
 *
 *   AddFunc ()
 *
 *   Adds a function to the function table.
 */

int AddFunc ( char * pstrName, int iEntryPoint )
{
    // If a function already exists with the specified name, exit and return an invalid
    // index

    if ( GetFuncByName ( pstrName ) )
        return -1;

    // Create a new function node

    FuncNode * pNewFunc = ( FuncNode * ) malloc ( sizeof ( FuncNode ) );

    // Initialize the new function

    strcpy ( pNewFunc->pstrName, pstrName );
    pNewFunc->iEntryPoint = iEntryPoint;

    // Add the function to the list and get its index

    int iIndex = AddNode ( & g_FuncTable, pNewFunc );

    // Set the function node's index

    pNewFunc->iIndex = iIndex;

    // Return the new function's index

    return iIndex;
}



//Symbol Table
/******************************************************************************************
 *
 *   GetSymbolByIdent ()
 *
 *   Returns a pointer to the symbol structure corresponding to the identifier and function
 *   index.
 */
SymbolNode * GetSymbolByIdent ( char * pstrIdent, int iFuncIndex )
{
    // If the table is empty, return a NULL pointer

    if ( ! g_SymbolTable.iNodeCount )
        return NULL;

    // Create a pointer to traverse the list

    LinkedListNode * pCurrNode = g_SymbolTable.pHead;

    // Traverse the list until the matching structure is found

    for ( int iCurrNode = 0; iCurrNode < g_SymbolTable.iNodeCount; ++ iCurrNode )
    {
        // Create a pointer to the current symbol structure

        SymbolNode * pCurrSymbol = ( SymbolNode * ) pCurrNode->pData;

        // See if the names match

        if ( strcmp ( pCurrSymbol->pstrIdent, pstrIdent ) == 0 )

            // If the functions match, or if the existing symbol is global, they match.
            // Return the symbol.

            if ( pCurrSymbol->iFuncIndex == iFuncIndex || pCurrSymbol->iStackIndex >= 0 )
                return pCurrSymbol;

        // Otherwise move to the next node

        pCurrNode = pCurrNode->pNext;
    }

    // The structure was not found, so return a NULL pointer

    return NULL;
}
/******************************************************************************************
 *
 *	GetStackIndexByIdent ()
 *
 *	Returns a symbol's stack index based on its identifier and function index.
 */
int GetStackIndexByIdent ( char * pstrIdent, int iFuncIndex )
{
    // Get the symbol's information

    SymbolNode * pSymbol = GetSymbolByIdent ( pstrIdent, iFuncIndex );

    // Return its stack index

    return pSymbol->iStackIndex;
}

/******************************************************************************************
 *
 *	GetSizeByIndent ()
 *
 *	Returns a variable's size based on its identifier.
 */

int GetSizeByIdent ( char * pstrIdent, int iFuncIndex )
{
    // Get the symbol's information

    SymbolNode * pSymbol = GetSymbolByIdent ( pstrIdent, iFuncIndex );

    // Return its size

    return pSymbol->iSize;
}
/******************************************************************************************
 *
 *   AddSymbol ()
 *
 *   Adds a symbol to the symbol table.
 */

int AddSymbol ( char * pstrIdent, int iSize, int iStackIndex, int iFuncIndex )
{
    // If a label already exists

    if ( GetSymbolByIdent ( pstrIdent, iFuncIndex ) )
        return -1;

    // Create a new symbol node

    SymbolNode * pNewSymbol = ( SymbolNode * ) malloc ( sizeof ( SymbolNode ) );

    // Initialize the new label

    strcpy ( pNewSymbol->pstrIdent, pstrIdent );
    pNewSymbol->iSize = iSize;
    pNewSymbol->iStackIndex = iStackIndex;
    pNewSymbol->iFuncIndex = iFuncIndex;

    // Add the symbol to the list and get its index

    int iIndex = AddNode ( & g_SymbolTable, pNewSymbol );

    // Set the symbol node's index

    pNewSymbol->iIndex = iIndex;

    // Return the new symbol's index

    return iIndex;
}



//LabelTable
/******************************************************************************************
 *
 *   GetLabelByIdent ()
 *
 *   Returns a pointer to the label structure corresponding to the identifier and function
 *   index.
 */
LabelNode * GetLabelByIdent ( char * pstrIdent, int iFuncIndex )
{
    // If the table is empty, return a NULL pointer

    if ( ! g_LabelTable.iNodeCount )
        return NULL;

    // Create a pointer to traverse the list

    LinkedListNode * pCurrNode = g_LabelTable.pHead;

    // Traverse the list until the matching structure is found

    for ( int iCurrNode = 0; iCurrNode < g_LabelTable.iNodeCount; ++ iCurrNode )
    {
        // Create a pointer to the current label structure

        LabelNode * pCurrLabel = ( LabelNode * ) pCurrNode->pData;

        // If the names and scopes match, return the current pointer

        if ( strcmp ( pCurrLabel->pstrIdent, pstrIdent ) == 0 && pCurrLabel->iFuncIndex == iFuncIndex )
            return pCurrLabel;

        // Otherwise move to the next node

        pCurrNode = pCurrNode->pNext;
    }

    // The structure was not found, so return a NULL pointer

    return NULL;
}
/******************************************************************************************
 *
 *   AddLabel ()
 *
 *   Adds a label to the label table.
 */

int AddLabel ( char * pstrIdent, int iTargetIndex, int iFuncIndex )
{
    // If a label already exists, return -1

    if ( GetLabelByIdent ( pstrIdent, iFuncIndex ) )
        return -1;

    // Create a new label node

    LabelNode * pNewLabel = ( LabelNode * ) malloc ( sizeof ( LabelNode ) );

    // Initialize the new label

    strcpy ( pNewLabel->pstrIdent, pstrIdent );
    pNewLabel->iTargetIndex = iTargetIndex;
    pNewLabel->iFuncIndex = iFuncIndex;

    // Add the label to the list and get its index

    int iIndex = AddNode ( & g_LabelTable, pNewLabel );

    // Set the index of the label node

    pNewLabel->iIndex = iIndex;

    // Return the new label's index

    return iIndex;
}


//InstructmentTable
/******************************************************************************************
 *
 *   AddInstrLookup ()
 *
 *   Adds an instruction to the instruction lookup table.
 */

int AddInstrLookup ( char * pstrMnemonic, int iOpcode, int iOpCount )
{
    // Just use a simple static int to keep track of the next instruction index in the
    // table.

    static int iInstrIndex = 0;

    // Make sure we haven't run out of instruction indices

    if ( iInstrIndex >= MAX_INSTR_LOOKUP_COUNT )
        return -1;

    // Set the mnemonic, opcode and operand count fields

    strcpy ( g_InstrTable [ iInstrIndex ].pstrMnemonic, pstrMnemonic );
    _strupr ( g_InstrTable [ iInstrIndex ].pstrMnemonic );
    g_InstrTable [ iInstrIndex ].iOpcode = iOpcode;
    g_InstrTable [ iInstrIndex ].iOpCount = iOpCount;

    // Allocate space for the operand list

    g_InstrTable [ iInstrIndex ].OpList = ( OpTypes * ) malloc ( iOpCount * sizeof ( OpTypes ) );

    // Copy the instruction index into another variable so it can be returned to the caller

    int iReturnInstrIndex = iInstrIndex;

    // Increment the index for the next instruction

    ++ iInstrIndex;

    // Return the used index to the caller

    return iReturnInstrIndex;
}

/******************************************************************************************
 *
 *   SetOpType ()
 *
 *   Sets the operand type for the specified operand in the specified instruction.
 */
void SetOpType ( int iInstrIndex, int iOpIndex, OpTypes iOpType )
{
    g_InstrTable [ iInstrIndex ].OpList [ iOpIndex ] = iOpType;
}

/******************************************************************************************
 *
 *   GetInstrByMnemonic ()
 *
 *   Returns a pointer to the instruction definition corresponding to the specified mnemonic.
 */

int GetInstrByMnemonic ( char * pstrMnemonic, InstrLookup * pInstr )
{
    // Loop through each instruction in the lookup table

    for ( int iCurrInstrIndex = 0; iCurrInstrIndex < g_iInstrTableLength; ++ iCurrInstrIndex )

        // Compare the instruction's mnemonic to the specified one

        if ( strcmp ( g_InstrTable [ iCurrInstrIndex ].pstrMnemonic, pstrMnemonic ) == 0 )
        {
            // Set the instruction definition to the user-specified pointer

            * pInstr = g_InstrTable [ iCurrInstrIndex ];

            // Return TRUE to signify success

            return TRUE;
        }

    // A match was not found, so return FALSE

    return FALSE;
}



