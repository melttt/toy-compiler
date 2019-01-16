#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include "lex.h" 


Lexer lexer;
// ---- Delimiters ------------------------------------------------------------------------
char cDelims [ MAX_DELIM_COUNT ] = { ',', '(', ')', '[', ']', '{', '}', ';' };


void init(char* pstrSource ,int iSourceSize)
{
    lexer.iStartLexemeIndex = lexer.iEndLexemeIndex = 0;
    lexer.pstrCurrLexeme = (char*)malloc(MAX_LEXEME_SIZE);
    lexer.pstrCurrSource = pstrSource;
    lexer.iSourceSize = iSourceSize;
    LexerState = LEX_STATE_START;
    
}

int IsCharDelim ( char cChar )
{

    // Loop through each delimiter in the array and compare it to the specified character
    for ( int iCurrDelimIndex = 0; iCurrDelimIndex < MAX_DELIM_COUNT; ++ iCurrDelimIndex )
    {
        // Return TRUE if a match was found
        if ( cChar == cDelims [ iCurrDelimIndex ] )
            return TRUE;
    }
    // The character is not a delimiter, so return FALSE
    return FALSE;
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

int IsCharIdent ( char cChar )
{
    if ((cChar >= '0' && cChar <= '9' ) ||
            ( cChar >= 'A' && cChar <= 'Z' ) ||
            ( cChar >= 'a' && cChar <= 'z' ) ||
            cChar == '_' )
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
                }else if(IsCharIdent(cCurrChar)){
                    LexerState = LEX_STATE_IDENT; 
                }else if(IsCharDelim(cCurrChar)){
                    LexerState = LEX_STATE_DELIM; 
                }else if(cCurrChar == '"'){
                    iIsWrite = FALSE;
                    LexerState = LEX_STATE_STRING;
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
                }else if(IsCharWhitespace(cCurrChar) || IsCharDelim(cCurrChar)){
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
                    ;
                }else if(IsCharWhitespace(cCurrChar) || IsCharDelim(cCurrChar))
                {
                    if(iCurrLexemeIndex <= 1 || (iCurrLexemeIndex > 1 && lexer.pstrCurrLexeme[iCurrLexemeIndex - 1] == '.'))
                    {
                        ExitOnInvalidInfo(LEX_ERR_FLOATWRONG_ONLY_POINT_CHAR);
                    }
                    iIsWrite = FALSE;
                    iIsFinish = TRUE;
                }else{
                    // wrong 
                    ExitOnInvalidInputError(cCurrChar);
                }

                break;
            }

            case LEX_STATE_IDENT:
            {
                if(IsCharIdent(cCurrChar))
                {
                    ;
                }else if(IsCharWhitespace(cCurrChar) || IsCharDelim(cCurrChar))
                {

                    iIsWrite = FALSE;
                    iIsFinish = TRUE;
                }else{
                    // wrong 
                    ExitOnInvalidInputError(cCurrChar);
                }


                break;
            }

            case LEX_STATE_DELIM:
            {
                iIsWrite = FALSE;
                iIsFinish = TRUE;

                break;
            }

            case LEX_STATE_STRING:
            {
                if(cCurrChar == '"')                
                {
                    iIsWrite = FALSE;
                    LexerState = LEX_STATE_STRING_CLOSE_QUOTE;                    
                }else if(cCurrChar == '\\'){
                    iIsWrite = FALSE;
                    LexerState = LEX_STATE_STRING_ESCAPE;
                }else{
                    ;
                }
                break;
            }

            case LEX_STATE_STRING_CLOSE_QUOTE:
            {
                iIsWrite = FALSE;
                iIsFinish = TRUE;
                break;
            }

            case LEX_STATE_STRING_ESCAPE:
            {
                LexerState = LEX_STATE_STRING;
                break;
            }

            default:
                ExitOnInvalidInfo(LEX_ERR_WRONG_STATE);

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
        case LEX_STATE_IDENT:
            tToken = TOKEN_TYPE_IDENT;

            if (strcasecmp( GetCurrLexeme(), "var" ) == 0 )
                tToken = TOKEN_TYPE_RSRVD_VAR;
            if (strcasecmp( GetCurrLexeme(), "true" ) == 0 )
                tToken = TOKEN_TYPE_RSRVD_TRUE;
            if (strcasecmp( GetCurrLexeme(), "false" ) == 0 )
                tToken = TOKEN_TYPE_RSRVD_FALSE;
            if (strcasecmp(GetCurrLexeme(), "if" ) == 0 )
                tToken = TOKEN_TYPE_RSRVD_IF;
            if (strcasecmp(GetCurrLexeme() , "else" ) == 0 )
                tToken = TOKEN_TYPE_RSRVD_ELSE;
            if (strcasecmp (GetCurrLexeme(), "break" ) == 0 )
                tToken = TOKEN_TYPE_RSRVD_BREAK;
            if (strcasecmp (GetCurrLexeme(), "continue" ) == 0 )
                tToken = TOKEN_TYPE_RSRVD_CONTINUE;
            if (strcasecmp (GetCurrLexeme(), "for" ) == 0 )
                tToken = TOKEN_TYPE_RSRVD_FOR;
            if (strcasecmp (GetCurrLexeme(), "while" ) == 0 )
                tToken = TOKEN_TYPE_RSRVD_WHILE;
            if (strcasecmp (GetCurrLexeme(), "func" ) == 0 )
                tToken = TOKEN_TYPE_RSRVD_FUNC;
            if (strcasecmp (GetCurrLexeme(), "return" ) == 0 )
                tToken = TOKEN_TYPE_RSRVD_RETURN;
            break;

        case LEX_STATE_DELIM:

            switch ( GetCurrLexeme()[0])
            {
                case ',':
                    tToken = TOKEN_TYPE_DELIM_COMMA;
                    break;
                case '(':
                    tToken = TOKEN_TYPE_DELIM_OPEN_PAREN;
                    break;
                case ')':
                    tToken = TOKEN_TYPE_DELIM_CLOSE_PAREN;
                    break;
                case '[':
                    tToken = TOKEN_TYPE_DELIM_OPEN_BRACE;
                    break;
                case ']':
                    tToken = TOKEN_TYPE_DELIM_CLOSE_BRACE;
                    break;
                case '{':
                    tToken = TOKEN_TYPE_DELIM_OPEN_CURLY_BRACE;
                    break;
                case '}':
                    tToken = TOKEN_TYPE_DELIM_CLOSE_CURLY_BRACE;
                    break;
                case ';':
                    tToken = TOKEN_TYPE_DELIM_SEMICOLON;
                    break;
            }
            break;

        case LEX_STATE_STRING_CLOSE_QUOTE:
            tToken = TOKEN_TYPE_STRING;
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
