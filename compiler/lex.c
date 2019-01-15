#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lex.h" 


Lexer lexer;


void init(char* pstrSource ,int iSourceSize)
{
    lexer.iStartLexemeIndex = lexer.iEndLexemeIndex = 0;
    lexer.pstrCurrLexeme = (char*)malloc(MAX_LEXEME_SIZE);
    lexer.pstrCurrSource = pstrSource;
    lexer.iSourceSize = iSourceSize;
    LexerState = LEX_STATE_START;
    
}

int IsCharWhitespace ( char cChar )
{
    if ( cChar == ' ' || cChar == '\t' || cChar == '\n' )
        return TRUE;
    else
        return FALSE;
}

int IsCharNumeric ( char cChar )
{
    // Return true if the character is between 0 and 9 inclusive.
    if ( cChar >= '0' && cChar <= '9' )
        return TRUE;
    else
        return FALSE;
}


char GetNextChar()
{
    return lexer.pstrCurrSource[LexerEndIndex ++];
}

Token GetNextToken ()
{
    int iIsWrite = FALSE;
    int iCurrLexemeIndex = 0;
    int iIsFinish = FALSE;
    char cCurrChar;
    Token tToken = TOKEN_TYPE_INVALID;

    iIsWrite = FALSE;
    LexerStartIndex = LexerEndIndex;
    LexerState = LEX_STATE_START;


    while(TRUE)
    {
        if(LexerEndIndex >= lexer.iSourceSize)
        {
            return TOKEN_TYPE_END_OF_STREAM;
        }

        iIsWrite = TRUE;
        cCurrChar = GetNextChar();

        switch(LexerState)
        {
            case LEX_STATE_START:
            {
                if(IsCharWhitespace(cCurrChar))
                {
                    iIsWrite = FALSE;
                }else if(IsCharNumeric(cCurrChar))
                {
                    LexerState = LEX_STATE_INT;
                }else if(cCurrChar == '.'){
                    LexerState = LEX_STATE_FLOAT;
                }else{
                    // wrong 
                    ExitOnInvalidInputError(cCurrChar);
                }

                break;
            }

            case LEX_STATE_INT:
            {
                if(IsCharNumeric(cCurrChar))
                {
                    ;
                }else if(cCurrChar == '.')
                {
                    LexerState = LEX_STATE_FLOAT;
                }else if(IsCharWhitespace(cCurrChar)){
                    iIsWrite = FALSE;
                    iIsFinish = TRUE; 
                }else{
                    // wrong 
                    ExitOnInvalidInputError(cCurrChar);
                }
                break;
            }
            case LEX_STATE_FLOAT:
            {
                if(IsCharNumeric(cCurrChar)) 
                {

                }else if(IsCharWhitespace(cCurrChar))
                {
                    if(iCurrLexemeIndex <= 1 || (iCurrLexemeIndex > 1 && lexer.pstrCurrLexeme[iCurrLexemeIndex - 1] == '.'))
                    {
                        ExitOnInvalidInfo(LEX_ERR_FLOATWRONG_ONLY_POINT_CHAR);
                    }
                    iIsWrite = FALSE;
                    iIsFinish = TRUE;
                }

                break;
            }
      
        }

        if(iIsWrite)
        {
            lexer.pstrCurrLexeme[iCurrLexemeIndex ++] = cCurrChar;
        }


        if(iIsFinish)
        {

            lexer.pstrCurrLexeme[iCurrLexemeIndex] = '\0';
            LexerEndIndex --;
            break;
        }
    }




    switch(LexerState)
    {
        case LEX_STATE_INT:
            tToken = TOKEN_TYPE_INT;
            break;
        case LEX_STATE_FLOAT:
            tToken = TOKEN_TYPE_FLOAT;
            break;
        default:
            tToken = TOKEN_TYPE_INVALID;
    }

    return tToken;
}

char* GetCurrLexeme()
{
    return lexer.pstrCurrLexeme;
}

void ExitOnInvalidInfo(char* pstrErrorInfo)
{
    printf("Error:%s\n", pstrErrorInfo);
    exit( 0 );
    
}
void ExitOnInvalidInputError ( char cInput )
{
    printf ( "Error: '%c' unexpected.\n", cInput );
    exit ( 0 );
}
