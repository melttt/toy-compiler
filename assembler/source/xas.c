#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "xas.h"
#include "utils.h"

char** g_ppstrSourceCode = NULL;
FILE* g_pSourceFile = NULL; 
int g_iSourceFileLine = 0;

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





int main(int argc, char * argv [])
{
    printf("start:\n------------------------\n");
    if(argc == 1)
    {
        printf("need a file!\n");
        exit(1);
    }
    g_iSourceFileLine = LoadSourceFile(argv[1]);

    int i;
    for(i = 0 ; i < g_iSourceFileLine; i ++)
    {
        printf("%s", g_ppstrSourceCode[i]);
    }
    printf("------------------------\n"); 


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
