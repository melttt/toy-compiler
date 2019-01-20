#ifndef _XSC_H_
#define _XSC_H_
#include "list.h"

#ifndef TRUE
#define TRUE                    1           // True
#endif

#ifndef FALSE
#define FALSE                   0           // False
#endif


#define MAX_SOURCE_LINE_SIZE            512
#define MAX_IDENT_SIZE                  256        
#define MAIN_FUNC_NAME				"_Main"


typedef struct _ScriptHeader                    // Script header data
{
    int iStackSize;                             // Requested stack size
    int iIsMainFuncPresent;                     // Is _Main () present?
    int iMainFuncIndex;							// _Main ()'s function index
}ScriptHeader;

extern LinkedList g_SourceCode;
extern ScriptHeader g_ScriptHeader;
extern LinkedList g_ICodeStream; 
extern LinkedList g_FuncTable;   
extern LinkedList g_SymbolTable;
extern LinkedList g_StringTable;

#endif
