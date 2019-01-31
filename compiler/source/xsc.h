#ifndef _XSC_H_
#define _XSC_H_
#include "list.h"

#ifndef TRUE
#define TRUE                    1           // True
#endif

#ifndef FALSE
#define FALSE                   0           // False
#endif



#define VERSION_MAJOR               0           // Major version number
#define VERSION_MINOR               8           // Minor version number

#define MAX_SOURCE_LINE_SIZE            512
#define MAX_IDENT_SIZE                  256        
#define MAIN_FUNC_NAME				"_Main"

#define REG_CODE_RETVAL             0           // _RetVal

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
extern int g_iTempVar0SymbolIndex;
extern int g_iTempVar1SymbolIndex;

#define TEMP_VAR_0                  "_T0"       // Temporary variable 0
#define TEMP_VAR_1                  "_T1"       // Temporary variable 1

extern char g_pstrOutputFilename[128];

#endif
