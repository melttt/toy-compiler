#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "xas.h"
#include "utils.h"
#include "ds.h"
#include "token.h"

char** g_ppstrSourceCode = NULL;
FILE* g_pSourceFile = NULL; 
int g_iSourceFileLine = 0;
Lexer g_Lexer;

void ShutDown()
{
    int i;
    for(i = 0 ; i < g_iSourceFileLine ; i ++)
    {
        free(g_ppstrSourceCode[i]);
    }
    free(g_ppstrSourceCode);
    fclose(g_pSourceFile);
}

int LoadSourceFile(char* FileName)
{
    int line = 0;
    char tmparray[MAX_SOURCE_LINE_SIZE];
    int len;
    
    if(!(g_pSourceFile = fopen(FileName, "rb")))    
    {
        ExitOnError("Open Wrong!\n");
    }

    if(NULL == (g_ppstrSourceCode = (char**) malloc(sizeof(char*) * MAX_SOURCE_LIEN_NUM)))  
    {
        ExitOnError("Malloc Wrong!\n");
    }

    while(fgets(tmparray, MAX_SOURCE_LINE_SIZE, g_pSourceFile))
    {
        StripComments(tmparray);
        TrimWhiteSpace(tmparray);
        if((len = strlen(tmparray)) <= 1)
            continue;

        if(tmparray[len - 2] != '\n')
        {
            tmparray[len - 1] = '\n';
            tmparray[len] = '\0';
            len ++;
        }

        if(len >= MAX_SOURCE_LINE_SIZE)
        {
            ExitOnError("Too Long Line!\n");
        }

        g_ppstrSourceCode[line] = (char*)malloc(sizeof(char) * len);
        strcpy(g_ppstrSourceCode[line], tmparray);
        line ++;
    }

    
    return line;
}
void InitInstrTable (){
    int iInstrIndex;

#define ENTRY(InstrName, InstrCode ) \
    iInstrIndex = AddInstrLookup (#InstrName, InstrCode, 1 ); \

    ZeroOperatorInstrState
#undef ENTRY

#define ENTRY(InstrName, InstrCode, InstrFlag ) \
    iInstrIndex = AddInstrLookup (#InstrName, InstrCode, 1 ); \
    SetOpType ( iInstrIndex, 0, InstrFlag);

    OneOperatorInstrState
#undef ENTRY

#define ENTRY(InstrName, InstrCode, InstrFlag1, InstrFlag2) \
    iInstrIndex = AddInstrLookup (#InstrName, InstrCode, 2 ); \
    SetOpType ( iInstrIndex, 0, InstrFlag1);  \
    SetOpType ( iInstrIndex, 1, InstrFlag2);

    TwoOperatorInstrState
#undef ENTRY

#define ENTRY(InstrName, InstrCode, InstrFlag1, InstrFlag2, InstrFlag3) \
    iInstrIndex = AddInstrLookup (#InstrName, InstrCode, 2 ); \
    SetOpType ( iInstrIndex, 0, InstrFlag1);  \
    SetOpType ( iInstrIndex, 1, InstrFlag2);    \
    SetOpType ( iInstrIndex, 1, InstrFlag3);

    ThreeOperatorInstrState
#undef ENTRY
    g_iInstrTableLength = iInstrIndex + 1;
}

//Lexer
void ResetLexer ()
{
    // Set the current line to the start of the file
    g_Lexer.iCurrSourceLine = 0;

    // Set both indices to point to the start of the string

    g_Lexer.iIndex0 = 0;
    g_Lexer.iIndex1 = 0;

    // Set the token type to invalid, since a token hasn't been read yet
    g_Lexer.CurrToken = TOKEN_TYPE_INVALID;
    // Set the lexing state to no strings
    g_Lexer.iCurrLexState = LEX_STATE_NO_STRING;
}

char * GetCurrLexeme ()
{
    // Simply return the pointer rather than making a copy
    return g_Lexer.pstrCurrLexeme;
}

int SkipToNextLine ()
{
    // Increment the current line

    ++ g_Lexer.iCurrSourceLine;

    // Return FALSE if we've gone past the end of the source code

    if ( g_Lexer.iCurrSourceLine >= g_iSourceFileLine )
        return FALSE;

    // Set both indices to point to the start of the string
    g_Lexer.iIndex0 = 0;
    g_Lexer.iIndex1 = 0;

    // Turn off string lexeme mode, since strings can't span multiple lines
    g_Lexer.iCurrLexState = LEX_STATE_NO_STRING;

    // Return TRUE to indicate success

    return TRUE;
}
Token GetNextToken ()
{
    g_Lexer.iIndex0 = g_Lexer.iIndex1;    
    if ( g_Lexer.iIndex0 >= strlen ( g_ppstrSourceCode [ g_Lexer.iCurrSourceLine ] ) )
    {
        if ( ! SkipToNextLine () )
            return END_OF_TOKEN_STREAM;
    }

    if ( g_Lexer.iCurrLexState == LEX_STATE_END_STRING )
        g_Lexer.iCurrLexState = LEX_STATE_NO_STRING;


    if ( g_Lexer.iCurrLexState != LEX_STATE_IN_STRING && ! IsCharWhitespace ( g_ppstrSourceCode [ g_Lexer.iCurrSourceLine ][ g_Lexer.iIndex0 ]))
    {
        ++ g_Lexer.iIndex0;
    }
    g_Lexer.iIndex1 =g_Lexer.iIndex0;


    if(g_Lexer.iCurrLexState == LEX_STATE_IN_STRING)
    {
        for(;;g_Lexer.iIndex1 ++)
        {
            if(g_Lexer.iIndex1 >= strlen ( g_ppstrSourceCode [ g_Lexer.iCurrSourceLine ]))
            {
                g_Lexer.CurrToken = TOKEN_TYPE_INVALID;
                return g_Lexer.CurrToken;
            }
            if(g_ppstrSourceCode [ g_Lexer.iCurrSourceLine ][ g_Lexer.iIndex1 ] == '"' )
            {
                g_Lexer.iCurrLexState = LEX_STATE_END_STRING;
                break;
            }
            if(g_ppstrSourceCode [ g_Lexer.iCurrSourceLine ][ g_Lexer.iIndex1 ] == '\\')
            {
                g_Lexer.iIndex1 ++; 
            }
        }
        
    }else{
        for(;;g_Lexer.iIndex1 ++)
        {
            if(g_Lexer.iIndex1 >= strlen ( g_ppstrSourceCode [ g_Lexer.iCurrSourceLine ]))
            {
                break;
            }
            if(IsCharDelimiter(g_ppstrSourceCode [ g_Lexer.iCurrSourceLine ][ g_Lexer.iIndex1 ]))
            {
                break;
            }
        }
        
    }






}



int main(int argc, char * argv [])
{
    printf("start:\n------------------------\n");
    if(argc == 1)
    {
        printf("need a file!\n");
        exit(1);
    }
    g_iSourceFileLine = LoadSourceFile(argv[1]);


    InitInstrTable();
    ResetLexer ();
    int i;
    for(i = 0 ; i < g_iSourceFileLine; i ++)
    {
        printf("%s", g_ppstrSourceCode[i]);
    }
    printf("------------------------\n"); 

    for(i = 0 ; i < g_iInstrTableLength ; i ++)
    {
        printf("%d %d :%s\n",g_InstrTable[i].iOpcode, g_InstrTable[i].iOpCount, g_InstrTable[i].pstrMnemonic);
    }

    ShutDown();
    return 0;
}







//copy from a book
void Exit ()
{
    // Give allocated resources a chance to be freed

    //ShutDown ();

    // Exit the program
    ShutDown();
    exit ( 0 );
}

/******************************************************************************************
*
*   ExitOnError ()
*
*   Prints an error message and exits.
*/

void ExitOnError ( char * pstrErrorMssg )
{
    // Print the message

    printf ( "Fatal Error: %s.\n", pstrErrorMssg );

    // Exit the program

    Exit ();
}

/******************************************************************************************
*
*   ExitOnCodeError ()
*
*   Prints an error message relating to the source code and exits.
*/
void ExitOnCodeError ( char * pstrErrorMssg )
{
    printf("has not been impelemented!\n");
/*
    // Print the message

    printf ( "Error: %s.\n\n", pstrErrorMssg );
    printf ( "Line %d\n", g_Lexer.iCurrSourceLine );

    // Reduce all of the source line's spaces to tabs so it takes less space and so the
    // karet lines up with the current token properly

    char pstrSourceLine [ MAX_SOURCE_LINE_SIZE ];
    strcpy ( pstrSourceLine, g_ppstrSourceCode [ g_Lexer.iCurrSourceLine ] );

    // Loop through each character and replace tabs with spaces

    for ( unsigned int iCurrCharIndex = 0; iCurrCharIndex < strlen ( pstrSourceLine ); ++ iCurrCharIndex )
        if ( pstrSourceLine [ iCurrCharIndex ] == '\t' )
            pstrSourceLine [ iCurrCharIndex ] = ' ';

    // Print the offending source line

    printf ( "%s", pstrSourceLine );

    // Print a karet at the start of the (presumably) offending lexeme

    for ( unsigned int iCurrSpace = 0; iCurrSpace < g_Lexer.iIndex0; ++ iCurrSpace )
        printf ( " " );
    printf ( "^\n" );

    // Print message indicating that the script could not be assembled

    printf ( "Could not assemble %s.\n", g_pstrExecFilename );

    // Exit the program

    Exit ();
*/
}
