#ifndef DS_H
#define DS_H

#ifndef TRUE
    #define TRUE                    1           // True
#endif

#ifndef FALSE
    #define FALSE                   0           // False
#endif

#define MAIN_FUNC_NAME				"_MAIN"		// _Main ()'s name
typedef struct _ScriptHeader                    // Script header data
{
    int iStackSize;                             // Requested stack size
    int iGlobalDataSize;						// The size of the script's global data
    int iIsMainFuncPresent;                     // Is _Main () present?
    int iMainFuncIndex;							// _Main ()'s function index
}ScriptHeader;


typedef struct _LinkedListNode                  // A linked list node
{
    void* pData;                               // Pointer to the node's data
    struct _LinkedListNode* pNext;                    // Pointer to the next node in the list
}LinkedListNode;

typedef struct _LinkedList                      // A linked list
{
     LinkedListNode* pHead,                     // Pointer to head node
                   * pTail;                     // Pointer to tail nail node
    int iNodeCount;                             // The number of nodes in the list
}LinkedList;

#define MAX_INSTR_LOOKUP_COUNT      256         // The maximum number of instructions
// the lookup table will hold
#define MAX_INSTR_MNEMONIC_SIZE     16          // Maximum size of an instruction
// mnemonic's string
 #define MAX_IDENT_SIZE              256        // Maximum identifier size

// ---- Instruction Lookup Table ----------------------------------------------------------

typedef int OpTypes;                            // Operand type bitfield alias type
typedef struct _InstrLookup                     // An instruction lookup
{
    char pstrMnemonic[MAX_INSTR_MNEMONIC_SIZE];  // Mnemonic string
    int iOpcode;                                // Opcode
    OpTypes* OpList;                           // Pointer to operand list
    int iOpCount;                               // Number of operands
}InstrLookup;

// ---- Assembled Instruction Stream ------------------------------------------------------
// ---- Operand Type Bitfield Flags ---------------------------------------------------
#define OP_TYPE_INT                 0           // Integer literal value
#define OP_TYPE_FLOAT               1           // Floating-point literal value
#define OP_TYPE_STRING_INDEX        2           // String literal value
#define OP_TYPE_ABS_STACK_INDEX     3           // Absolute array index
#define OP_TYPE_REL_STACK_INDEX     4           // Relative array index
#define OP_TYPE_INSTR_INDEX         5           // Instruction index
#define OP_TYPE_FUNC_INDEX          6           // Function index
#define OP_TYPE_HOST_API_CALL_INDEX 7           // Host API call index
#define OP_TYPE_REG                 8           // Register

typedef struct _Op                              // An assembled operand
{
    int iType;                                  // Type
    union                                       // The value
    {
        int iIntLiteral;                        // Integer literal
        float fFloatLiteral;                    // Float literal
        int iStringTableIndex;                  // String table index
        int iStackIndex;                        // Stack index
        int iInstrIndex;                        // Instruction index
        int iFuncIndex;                         // Function index
        int iHostAPICallIndex;                  // Host API Call index
        int iReg;                               // Register code
    };
    int iOffsetIndex;                           // Index of the offset
}Op;

typedef struct _Instr                           // An instruction
{
    int iOpcode;                                // Opcode
    int iOpCount;                               // Number of operands
    Op * pOpList;                               // Pointer to operand list
}Instr;

// ---- Function Table --------------------------------------------------------------------

typedef struct _FuncNode                        // A function table node
{
    int iIndex;									// Index
    char pstrName [ MAX_IDENT_SIZE ];           // Name
    int iEntryPoint;                            // Entry point
    int iParamCount;                            // Param count
    int iLocalDataSize;                         // Local data size
}FuncNode;


// ---- Label Table -----------------------------------------------------------------------

typedef struct _LabelNode                       // A label table node
{
    int iIndex;									// Index
    char pstrIdent [ MAX_IDENT_SIZE ];          // Identifier
    int iTargetIndex;                           // Index of the target instruction
    int iFuncIndex;                             // Function in which the label resides
}LabelNode;


// ---- Symbol Table ----------------------------------------------------------------------

typedef struct _SymbolNode                      // A symbol table node
{
    int iIndex;									// Index
    char pstrIdent [ MAX_IDENT_SIZE ];          // Identifier
    int iSize;                                  // Size (1 for variables, N for arrays)
    int iStackIndex;                            // The stack index to which the symbol
    // points
    int iFuncIndex;                             // Function in which the symbol resides
}SymbolNode;




extern LinkedList g_StringTable;
extern LinkedList g_FuncTable;
extern LinkedList g_SymbolTable;
extern LinkedList g_LabelTable;
extern LinkedList g_HostAPICallTable;
extern InstrLookup g_InstrTable[MAX_INSTR_LOOKUP_COUNT];
extern int g_iInstrTableLength;

extern Instr* g_pInstrStream;
extern int g_iInstrStreamSize;

extern ScriptHeader g_ScriptHeader;
extern int g_iIsSetStackSizeFound;
extern int g_iCurrInstrIndex;

void InitLinkedList ( LinkedList * pList );
int AddNode ( LinkedList * pList, void * pData );
void FreeLinkedList ( LinkedList * pList );
int AddString ( LinkedList * pList, char * pstrString );
FuncNode * GetFuncByName ( char * pstrName );
void SetFuncInfo ( char * pstrName, int iParamCount, int iLocalDataSize );
int AddFunc ( char * pstrName, int iEntryPoint );
SymbolNode * GetSymbolByIdent ( char * pstrIdent, int iFuncIndex );
int GetStackIndexByIdent ( char * pstrIdent, int iFuncIndex );
int GetSizeByIdent ( char * pstrIdent, int iFuncIndex );
int AddSymbol ( char * pstrIdent, int iSize, int iStackIndex, int iFuncIndex );
LabelNode * GetLabelByIdent ( char * pstrIdent, int iFuncIndex );
int AddLabel ( char * pstrIdent, int iTargetIndex, int iFuncIndex );
int AddInstrLookup ( char * pstrMnemonic, int iOpcode, int iOpCount );
void SetOpType ( int iInstrIndex, int iOpIndex, OpTypes iOpType );
int GetInstrByMnemonic ( char * pstrMnemonic, InstrLookup * pInstr );
#endif
