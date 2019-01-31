#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include "lex.h" 


Lexer lexer;
Lexer prevlexer;
// ---- Delimiters ------------------------------------------------------------------------
char cDelims [ MAX_DELIM_COUNT ] = { ',', '(', ')', '[', ']', '{', '}', ';' };


//Opstate Table
OpState g_OpChars0 [ MAX_OP_STATE_COUNT ] = { { '+', 0, 2, 0 },{ '-', 2, 2, 1 },{ '*', 4, 1, 2 },
                                              { '/', 5, 1, 3 },{ '%', 6, 1, 4 },{ '^', 7, 1, 5 },
                                              { '&', 8, 2, 6 },{ '|', 10, 2, 7 },{ '#', 12, 1, 8 },
                                              { '~', 0, 0, 9 },{ '!', 13, 1, 10 },{ '=', 14, 1, 11 },
                                              { '<', 15, 2, 12 },{ '>', 17, 2, 13 }
                                            };


OpState g_OpChars1 [ MAX_OP_STATE_COUNT ] = { { '=', 0, 0, 14 }, { '+', 0, 0, 15 },     // +=, ++
                                              { '=', 0, 0, 16 }, { '-', 0, 0, 17 },     // -=, --
                                              { '=', 0, 0, 18 },                        // *=
                                              { '=', 0, 0, 19 },                        // /=
                                              { '=', 0, 0, 20 },                        // %=
                                              { '=', 0, 0, 21 },                        // ^=
                                              { '=', 0, 0, 22 }, { '&', 0, 0, 23 },     // &=, &&
                                              { '=', 0, 0, 24 }, { '|', 0, 0, 25 },     // |=, ||
                                              { '=', 0, 0, 26 },                        // #=
                                              { '=', 0, 0, 27 },                        // !=
                                              { '=', 0, 0, 28 },                        // ==
                                              { '=', 0, 0, 29 }, { '<', 0, 1, 30 },     // <=, <<
                                              { '=', 0, 0, 31 }, { '>', 1, 1, 32 } };   // >=, >>

OpState g_OpChars2 [ MAX_OP_STATE_COUNT ] = { { '=', 0, 0, 33 }, { '=', 0, 0, 34 } }; // <<=, >>=




void ResetLexer()
{
    lexer.iCurrLineIndex = 0;
    lexer.pCurrLine = g_SourceCode.pHead;
    lexer.iStartLexemeIndex = lexer.iEndLexemeIndex = 0;
    LexerOpIndex = OP_TYPE_INVALID;
    LexerState = LEX_STATE_START;
}



void CopyLexerState (Lexer *pDestLexer, Lexer* pSourceLexer )
{
    pDestLexer->pCurrLine = pSourceLexer->pCurrLine;
    pDestLexer->tCurrToken = pSourceLexer->tCurrToken;

    pDestLexer->iCurrLineIndex = pSourceLexer->iCurrLineIndex;
    strcpy ( pDestLexer->pstrCurrLexeme, pSourceLexer->pstrCurrLexeme );
    pDestLexer->iStartLexemeIndex = pSourceLexer->iStartLexemeIndex;
    pDestLexer->iEndLexemeIndex = pSourceLexer->iEndLexemeIndex;

    pDestLexer->iCurrOpIndex = pSourceLexer->iCurrOpIndex;
    pDestLexer->iCurrState = pSourceLexer->iCurrState;
}
int IsCharDelim ( char cChar )
{

    // Loop through each delimiter in the array and compare it to the specified character
    int iCurrDelimIndex;
    for ( iCurrDelimIndex = 0; iCurrDelimIndex < MAX_DELIM_COUNT; ++ iCurrDelimIndex )
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

int IsCharOpChar ( char cChar, int iCharIndex )
{
    int iCurrOpStateIndex;
    for ( iCurrOpStateIndex = 0; iCurrOpStateIndex < MAX_OP_STATE_COUNT; ++ iCurrOpStateIndex )
    {

        char cOpChar;
        switch ( iCharIndex )
        {
            case 0:
                cOpChar = g_OpChars0 [ iCurrOpStateIndex ].cChar;
                break;
            case 1:
                cOpChar = g_OpChars1 [ iCurrOpStateIndex ].cChar;
                break;
            case 2:
                cOpChar = g_OpChars2 [ iCurrOpStateIndex ].cChar;
                break;
        }
        if ( cChar == cOpChar )
            return TRUE;
    }
    return FALSE;    

}


int GetOpStateIndex(char ch,int col, int row, int len)
{
    int i;
    for (i = 0; i < len; ++ i )
    {

        char cOpChar;
        switch ( col )
        {
            case 0:
                cOpChar = g_OpChars0 [ row + i ].cChar;
                break;
            case 1:
                cOpChar = g_OpChars1 [ row + i ].cChar;
                break;
            case 2:
                cOpChar = g_OpChars2 [ row + i ].cChar;
                break;
        }
        if ( ch == cOpChar )
        {
            return row + i;
        }
    }
    return -1;    
}


char GetNextChar()
{
    char * pstrCurrLine;
    if ( lexer.pCurrLine )
        pstrCurrLine = ( char * ) lexer.pCurrLine->pData;
    else
        return '\0';

    if (lexer.iEndLexemeIndex >= ( int ) strlen ( pstrCurrLine ) )
    {
        lexer.pCurrLine = lexer.pCurrLine->pNext;
        if ( lexer.pCurrLine )
        {
            pstrCurrLine = ( char * ) lexer.pCurrLine->pData;
            ++ lexer.iCurrLineIndex;
            lexer.iStartLexemeIndex = 0;
            lexer.iEndLexemeIndex = 0;
        }
        else
        {
            return '\0';
        }
    }
    return pstrCurrLine[lexer.iEndLexemeIndex++];
}

int GetCurrOpIndex()
{
    return LexerOpIndex;
}

int GetCurrOp ()
{
    return lexer.iCurrOpIndex;
}


Token GetNextToken ()
{
    CopyLexerState (&prevlexer,&lexer);

    int iIsWrite = FALSE;
    int iCurrLexemeIndex = 0;
    int iIsFinish = FALSE;
    char cCurrChar;

    //Op
    int iCurrOpCol = 0;
    int iCurrOpRow = 0;
    int iCurrOpCount = MAX_OP_STATE_COUNT;
    int iCurrOpIndex = -1;
    OpState oCurrOpState;

    Token tToken = TOKEN_TYPE_INVALID;

    iIsWrite = FALSE;

    LexerStartIndex = LexerEndIndex;
    LexerState = LEX_STATE_START;


    while(TRUE)
    {
        iIsWrite = TRUE;
        cCurrChar = GetNextChar();
        if(cCurrChar == '\0')
        {
            LexerState = LEX_STATE_END;
            break;
        }

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
                }else if(IsCharOpChar(cCurrChar, iCurrOpCol)){
                    if((iCurrOpIndex = GetOpStateIndex(cCurrChar, iCurrOpCol ,iCurrOpRow, iCurrOpCount)) != -1) 
                    {
                        oCurrOpState = g_OpChars0[iCurrOpIndex];
                        iCurrOpCol ++;
                        iCurrOpRow = oCurrOpState.iSubStateIndex;
                        iCurrOpCount = oCurrOpState.iSubStateCount;
                        LexerState = LEX_STATE_OP;
                    }else{

                        LexerState = LEX_STATE_UNKNOW;
                        iIsFinish = TRUE;
                    }
                }else{
                    LexerState = LEX_STATE_UNKNOW;
                    iIsFinish = TRUE;
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
                    LexerState = LEX_STATE_UNKNOW;
                    iIsFinish = TRUE;
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
                        LexerState = LEX_STATE_UNKNOW;
                        iIsFinish = TRUE;
                    }else{
                        iIsWrite = FALSE;
                        iIsFinish = TRUE;
                    }
                }else{
                    LexerState = LEX_STATE_UNKNOW;
                    iIsFinish = TRUE;
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
                    LexerState = LEX_STATE_UNKNOW;
                    iIsFinish = TRUE;
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


            case LEX_STATE_OP:
            {
                if(iCurrOpCol <= 3 && iCurrOpCount != 0 && IsCharOpChar(cCurrChar ,iCurrOpCol))
                {

                    if((iCurrOpIndex = GetOpStateIndex(cCurrChar, iCurrOpCol ,iCurrOpRow, iCurrOpCount)) != -1) 
                    {
                        if(iCurrOpCol == 1)
                            oCurrOpState = g_OpChars1[iCurrOpIndex];
                        else
                            oCurrOpState = g_OpChars2[iCurrOpIndex];

                        iCurrOpCol ++;
                        iCurrOpRow = oCurrOpState.iSubStateIndex;
                        iCurrOpCount = oCurrOpState.iSubStateCount;
                    }else{
                        LexerState = LEX_STATE_UNKNOW;
                        iIsFinish = TRUE;
                    }
                    
                }else if(IsCharWhitespace(cCurrChar) || IsCharDelim(cCurrChar) || IsCharIdent(cCurrChar) ||
                        cCurrChar == '.' || cCurrChar == '"'){

                    LexerOpIndex = oCurrOpState.iIndex;
                    iIsWrite = FALSE;
                    iIsFinish = TRUE;
                }else{
                    LexerState = LEX_STATE_UNKNOW;
                    iIsFinish = TRUE;
                }


                break;
            }

            default:
                LexerState = LEX_STATE_UNKNOW;
                iIsFinish = TRUE;

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
            if ( strcasecmp (GetCurrLexeme(), "host" ) == 0 )
                tToken = TOKEN_TYPE_RSRVD_HOST;
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

        case LEX_STATE_OP:
            tToken = TOKEN_TYPE_OP;
            break;

        case LEX_STATE_UNKNOW:
            tToken = TOKEN_TYPE_INVALID;
            break;

        case LEX_STATE_END:
            tToken = TOKEN_TYPE_END_OF_STREAM;
            break;

        default:
            tToken = TOKEN_TYPE_INVALID;
    }

    lexer.tCurrToken = tToken;
    return tToken;
}

char* GetCurrLexeme()
{
    return lexer.pstrCurrLexeme;
}

char* GetCurrSourceLine ()
{
    if (lexer.pCurrLine)
        return (char*)lexer.pCurrLine->pData;
    else
        return NULL;
}

int GetCurrSourceLineIndex ()
{
    return lexer.iCurrLineIndex;
}

Token GetCurrToken ()
{
    return lexer.tCurrToken;
}

void CopyCurrLexeme ( char * pstrBuffer )
{
    strcpy ( pstrBuffer, lexer.pstrCurrLexeme );
}


char GetLookAheadChar ()
{
    Lexer PrevLexer;
    CopyLexerState (&PrevLexer, &lexer);
    char cCurrChar;
    while ( TRUE )
    {
        cCurrChar = GetNextChar ();
        if ( ! IsCharWhitespace ( cCurrChar ) )
            break;
    }
    CopyLexerState ( &lexer, &PrevLexer );
    return cCurrChar;

}

void RewindTokenStream ()
{
    CopyLexerState(&lexer,&prevlexer);
}

