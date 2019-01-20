#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lex.h"
#include "preprocessor.h"
char* g_pstrSource ;
LinkedList g_SourceCode;

void LoadSourceFile (char* pstrSourceFileName)
{
    // ---- Open the input file
    FILE * pSourceFile;
    if ( ! ( pSourceFile = fopen ( pstrSourceFileName, "r" ) ) )
        printf ( "Could not open source file for input" );


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



    PrintFile();
    printf("\n------------------\n");
    StripComment(); 
    PrintFile();
//    PrintFile();

    /*
    init(g_SourceCode.pHead);

    Token tCurrToken;
    int iTokenCount = 0;
    char pstrToken[MAX_LEXEME_SIZE];




    while(TRUE)
    {
        tCurrToken = GetNextToken();

        if(tCurrToken == TOKEN_TYPE_END_OF_STREAM)
            break;

        switch(tCurrToken)
        {
            case TOKEN_TYPE_INT:
                strcpy ( pstrToken, "Integer" );
                break;
            case TOKEN_TYPE_FLOAT:
                strcpy ( pstrToken, "Float" );
                break;
            case TOKEN_TYPE_IDENT:
                strcpy ( pstrToken, "Identifier" );
                break;
            case TOKEN_TYPE_RSRVD_VAR:
                strcpy ( pstrToken, "var" );
                break;
            case TOKEN_TYPE_RSRVD_TRUE:
                strcpy ( pstrToken, "true" );
                break;
            case TOKEN_TYPE_RSRVD_FALSE:
                strcpy ( pstrToken, "false" );
                break;
            case TOKEN_TYPE_RSRVD_IF:
                strcpy ( pstrToken, "if" );
                break;
            case TOKEN_TYPE_RSRVD_ELSE:
                strcpy ( pstrToken, "else" );
                break;
            case TOKEN_TYPE_RSRVD_BREAK:
                strcpy ( pstrToken, "break" );
                break;
            case TOKEN_TYPE_RSRVD_CONTINUE:
                strcpy ( pstrToken, "continue" );
                break;
            case TOKEN_TYPE_RSRVD_FOR:
                strcpy ( pstrToken, "for" );
                break;
            case TOKEN_TYPE_RSRVD_WHILE:
                strcpy ( pstrToken, "while" );
                break;
            case TOKEN_TYPE_RSRVD_FUNC:
                strcpy ( pstrToken, "func" );
                break;
            case TOKEN_TYPE_RSRVD_RETURN:
                strcpy ( pstrToken, "return" );
                break;
            case TOKEN_TYPE_DELIM_COMMA:
                strcpy ( pstrToken, "Comma" );
                break;
            case TOKEN_TYPE_DELIM_OPEN_PAREN:
                strcpy ( pstrToken, "Opening Parenthesis" );
                break;
            case TOKEN_TYPE_DELIM_CLOSE_PAREN:
                strcpy ( pstrToken, "Closing Parenthesis" );
                break;
            case TOKEN_TYPE_DELIM_OPEN_BRACE:
                strcpy ( pstrToken, "Opening Brace" );
                break;
            case TOKEN_TYPE_DELIM_CLOSE_BRACE:
                strcpy ( pstrToken, "Closing Brace" );
                break;
            case TOKEN_TYPE_DELIM_OPEN_CURLY_BRACE:
                strcpy ( pstrToken, "Opening Curly Brace" );
                break;
            case TOKEN_TYPE_DELIM_CLOSE_CURLY_BRACE:
                strcpy ( pstrToken, "Closing Curly Brace" );
                break;
            case TOKEN_TYPE_DELIM_SEMICOLON:
                strcpy ( pstrToken, "Semicolon" );
                break;
            case TOKEN_TYPE_STRING:
                strcpy ( pstrToken, "String" );
                break;
            case TOKEN_TYPE_OP:
                sprintf ( pstrToken, "Operator %d", LexerOpIndex );
                break;

            default:
                strcpy ( pstrToken, "unknow" );

        }
        printf ( "%d: Token: %s, Lexeme: \"%s\"\n", iTokenCount, pstrToken, GetCurrLexeme() );
        ++ iTokenCount;
    }
*/

    return 0;
}
