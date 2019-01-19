#ifndef XSC_SYMBOL_TABLE
#define XSC_SYMBOL_TABLE
#include "xsc.h"
#define SCOPE_GLOBAL                    0       // Global scope
#define SYMBOL_TYPE_VAR                 0       // Variable
#define SYMBOL_TYPE_PARAM               1       // Parameter
typedef struct _SymbolNode                          // A symbol table node
{
    int iIndex;									    // Index
    char pstrIdent [ MAX_IDENT_SIZE ];              // Identifier
    int iSize;                                      // Size (1 for variables, N for arrays)
    int iScope;                                     // Scope (0 for globals, N for locals'
    int iType;                                      // Symbol type (parameter or variable)
}SymbolNode;

SymbolNode* GetSymbolByIndex( int iIndex );
SymbolNode* GetSymbolByIdent( char * pstrIdent, int iScope );
int GetSizeByIdent( char * pstrIdent, int iScope );
int AddSymbol( char * pstrIdent, int iSize, int iScope, int iType );

#endif
