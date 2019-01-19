#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "error.h"
#include "xsc.h"
LinkedList g_SourceCode;
ScriptHeader g_ScriptHeader;
LinkedList g_ICodeStream; 
LinkedList g_FuncTable;   
LinkedList g_SymbolTable;
LinkedList g_StringTable;

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
int main(int argc, char* arvc[])
{
    if(argc < 2)
    {
        printf("need a file\n");
        exit(0);
    }
    LoadSourceFile(arvc[1]);






    return 0;
}
