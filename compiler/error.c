#include <stdio.h>
#include "error.h"


void ExitOnError ( char * pstrErrorMssg )
{



    printf ( "Fatal Error: %s.\n", pstrErrorMssg );

    //Exit ();
}

void ExitOnCodeError ( char * pstrErrorMssg )
{
    /*
    printf ( "Error: %s.\n\n", pstrErrorMssg );
    printf ( "Line %d\n", GetCurrSourceLineIndex () );
    char pstrSourceLine [ MAX_SOURCE_LINE_SIZE ];
    char * pstrCurrSourceLine = GetCurrSourceLine ();
    if ( pstrCurrSourceLine )
        strcpy ( pstrSourceLine, pstrCurrSourceLine );
    else
        pstrSourceLine [ 0 ] = '\0';
    int iLastCharIndex = strlen ( pstrSourceLine ) - 1;
    if ( pstrSourceLine [ iLastCharIndex ] == '\n' )
        pstrSourceLine [ iLastCharIndex ] = '\0';
    for ( unsigned int iCurrCharIndex = 0; iCurrCharIndex < strlen ( pstrSourceLine ); ++ iCurrCharIndex )
        if ( pstrSourceLine [ iCurrCharIndex ] == '\t' )
            pstrSourceLine [ iCurrCharIndex ] = ' ';
    printf ( "%s\n", pstrSourceLine );
    for ( int iCurrSpace = 0; iCurrSpace < GetLexemeStartIndex (); ++ iCurrSpace )
        printf ( " " );
    printf ( "^\n" );
    printf ( "Could not compile %s.", g_pstrSourceFilename );
    Exit ();
    */

}
