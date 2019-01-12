#ifndef DS_V_H
#define DS_V_H

#define MAX_COERCION_STRING_SIZE    65


#define OP_TYPE_NULL                -1
#define OP_TYPE_INT                 0           // Integer literal value
#define OP_TYPE_FLOAT               1           // Floating-point literal value
#define OP_TYPE_STRING_INDEX        2           // String literal value
#define OP_TYPE_STRING              2           // String literal value
#define OP_TYPE_ABS_STACK_INDEX     3           // Absolute array index
#define OP_TYPE_REL_STACK_INDEX     4           // Relative array index
#define OP_TYPE_INSTR_INDEX         5           // Instruction index
#define OP_TYPE_FUNC_INDEX          6           // Function index
#define OP_TYPE_HOST_API_CALL_INDEX 7           // Host API call index
#define OP_TYPE_REG                 8           // Register

typedef struct _Value							// A runtime value
{
    int iType;                                  // Type
    union                                       // The value
    {
        int iIntLiteral;                        // Integer literal
        float fFloatLiteral;                    // Float literal
        char * pstrStringLiteral;				// String literal
        int iStackIndex;                        // Stack Index
        int iInstrIndex;                        // Instruction index
        int iFuncIndex;                         // Function index
        int iHostAPICallIndex;                  // Host API Call index
        int iReg;                               // Register code
    };
    int iOffsetIndex;                           // Index of the offset
}Value;

// ---- Runtime Stack ---------------------------------------------------------------------


typedef struct _RuntimeStack					// A runtime stack
{
    Value* pElmnts;							// The stack elements
    int iSize;									// The number of elements in the stack
    int iTopIndex;								// The top index
    int iFrameIndex;                            // Index of the top of the current
    // stack frame.
}RuntimeStack;



// ---- Functions -------------------------------------------------------------------------
typedef struct _Func							// A function
{
    int iEntryPoint;							// The entry point
    int iParamCount;							// The parameter count
    int iLocalDataSize;							// Total size of all local data
    int iStackFrameSize;						// Total size of the stack frame
}Func;

// ---- Instructions ----------------------------------------------------------------------

typedef struct _Instr                           // An instruction
{
    int iOpcode;                                // The opcode
    int iOpCount;                               // The number of operands
    Value * pOpList;                            // The operand list
}Instr;

typedef struct _InstrStream                     // An instruction stream
{
    Instr * pInstrs;							// The instructions themselves
    int iSize;                                  // The number of instructions in the
    // stream
    int iCurrInstr;                             // The instruction pointer
}InstrStream;

// ---- Host API Call Table ---------------------------------------------------------------
typedef void (*func)(void);
typedef struct _HostFunc
{
    char* ptrFuncName;
    int iParamCount;
    func fHostfunc;
}HostFunc;
#define MAXHOSTFUNC 5
typedef struct _HostAPICallTable				// A host API call table
{
    char ** ppstrCalls;							// Pointer to the call array
    int iSize;									// The number of calls in the array
    HostFunc ptrHostFuncTable[MAXHOSTFUNC];
    int iFuncNum;
}HostAPICallTable;

// ---- Scripts ---------------------------------------------------------------------------


typedef struct _Script							// Encapsulates a full script
{

    // Header data
    int iGlobalDataSize;						// The size of the script's global data
    int iIsMainFuncPresent;                     // Is _Main () present?
    int iMainFuncIndex;							// _Main ()'s function index
    // Runtime tracking

    int iIsPaused;								// Is the script currently paused?
    int iPauseEndTime;			                // If so, when should it resume?
    // Register file
    Value _RetVal;								// The _RetVal register
    // Script data
    struct _InstrStream InstrStream;                    // The instruction stream
    RuntimeStack Stack;                         // The runtime stack
    Func * pFuncTable;                          // The function table
    struct _HostAPICallTable HostAPICallTable;			// The host API call table
}Script;


// ---- Macros --------------------------------------------------------------------------------
/******************************************************************************************
 *
 *	ResolveStackIndex ()
 *
 *	Resolves a stack index by translating negative indices relative to the top of the
 *	stack, to positive ones relative to the bottom.
 */
#define ResolveStackIndex( iIndex )	\
    ( iIndex < 0 ? iIndex += g_Script.Stack.iFrameIndex : iIndex )  


extern Script g_Script;


int CoerceValueToInt ( Value Val );
float CoerceValueToFloat ( Value Val );
char * CoerceValueToString ( Value Val );

void CopyValue ( Value * pDest, Value Source );

int GetOpType ( int iOpIndex );
int ResolveOpType ( int iOpIndex );

int ResolveOpStackIndex ( int iOpIndex );
Value ResolveOpValue ( int iOpIndex );
int ResolveOpAsInt ( int iOpIndex );
float ResolveOpAsFloat ( int iOpIndex );
char * ResolveOpAsString ( int iOpIndex );
int ResolveOpAsInstrIndex ( int iOpIndex );
int ResolveOpAsFuncIndex ( int iOpIndex );
char * ResolveOpAsHostAPICall ( int iOpIndex );
Value * ResolveOpPntr ( int iOpIndex );


// ---- Runtime Stack Interface -----------------------------------------------------------


Value GetStackValue ( int iIndex );
void SetStackValue ( int iIndex, Value Val );
void Push ( Value Val );
Value Pop ();
void PushFrame ( int iSize );
void PopFrame ( int iSize );

// ---- Function Table Interface ----------------------------------------------------------
Func GetFunc ( int iIndex );
int RegisterHostFunc(char *ptrName, int iCount,func ptrFunc );
// ---- Host API Call Table Interface -----------------------------------------------------
char * GetHostAPICall ( int iIndex );
int GetParamAsInt(int iParamIndex);
int GetParamAsFloat(int iParamIndex);
char*  GetParamAsString(int iParamIndex);
Value GetParamAsValue(int iParamIndex);
int ReturnStringFromHost(char* ch, int iParamCount);
int ReturnFloatFromHost(int iRet, int iParamCount);
int ReturnIntFromHost(int iRet, int iParamCount);


#endif


