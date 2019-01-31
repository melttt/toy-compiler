#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "error.h"
#include "xsc.h"
#include "preprocessor.h"
#include "symbol_table.h"
#include "parser.h"
#include "code_emit.h"
char g_pstrOutputFilename[128] = "xsc.out";
LinkedList g_SourceCode;
ScriptHeader g_ScriptHeader;
LinkedList g_ICodeStream; 
LinkedList g_FuncTable;   
LinkedList g_SymbolTable;
LinkedList g_StringTable;
int g_iTempVar0SymbolIndex,                     // Temporary variable symbol indices
    g_iTempVar1SymbolIndex;
void LoadSourceFile (char* pstrSourceFileName)
{
    // ---- Open the input file
    FILE * pSourceFile;
    if ( ! ( pSourceFile = fopen ( pstrSourceFileName, "r" ) ) )
        ExitOnError ( "Could not open source file for input" );
    

    while ( ! feof ( pSourceFile ) )
    {
        char * pstrCurrLine = ( char * ) malloc ( MAX_SOURCE_LINE_SIZE + 1 );
        fgets ( pstrCurrLine, MAX_SOURCE_LINE_SIZE, pSourceFile );
        AddNode ( & g_SourceCode, pstrCurrLine );
    }

    fclose ( pSourceFile );

}
void CompileSourceFile ()
{
    // Add two temporary variables for evaluating expressions
    g_iTempVar0SymbolIndex = AddSymbol ( TEMP_VAR_0, 1, SCOPE_GLOBAL, SYMBOL_TYPE_VAR );
    g_iTempVar1SymbolIndex = AddSymbol ( TEMP_VAR_1, 1, SCOPE_GLOBAL, SYMBOL_TYPE_VAR );

    // Parse the source file to create an I-code representation
    ParseSourceCode ();
}

void Init ()
{
    // ---- Initialize the script header
    g_ScriptHeader.iIsMainFuncPresent = FALSE;
    g_ScriptHeader.iStackSize = 0;

    // Initialize the source code list
    InitLinkedList ( & g_SourceCode );
    // Initialize the tables

    InitLinkedList ( & g_FuncTable );

    InitLinkedList ( & g_SymbolTable );
    InitLinkedList ( & g_StringTable );
}


int main(int argc, char* arvc[])
{
    if(argc < 2)
    {
        printf("need a file\n");
        exit(0);
    }
    Init();
    LoadSourceFile(arvc[1]);
    PrintFile();
    StripComment(); 
    CompileSourceFile ();

    EmitCode();

    return 0;
}
