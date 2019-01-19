#ifndef XSC_FUNC_TABLE
#define XSC_FUNC_TABLE
#include "xsc.h"
typedef struct _FuncNode                            // A function table node
{
    int iIndex;									    // Index
    char pstrName [MAX_IDENT_SIZE];                 // Name
    int iIsHostAPI;                                 // Is this a host API function?
    int iParamCount;                                // The number of accepted parameters
    LinkedList ICodeStream;                         // Local I-code stream
}FuncNode;


FuncNode* GetFuncByIndex ( int iIndex );
FuncNode* GetFuncByName ( char * pstrName );
int AddFunc(char * pstrName, int iIsHostAPI );
void SetFuncParamCount(int iIndex, int iParamCount );
#endif
