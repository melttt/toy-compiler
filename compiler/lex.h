#ifndef _LEX_H
#define _LEX_H

#ifndef TRUE
#define TRUE                        1           // True
#endif

#ifndef FALSE
#define FALSE                       0           // False
#endif

// ---- Lexemes ---------------------------------------------------------------------------
#define MAX_LEXEME_SIZE                 1024        // Maximum individual lexeme size
// ---- Lexer States ----------------------------------------------------------------------
#define LEX_STATE_START                 0           // Start state
#define LEX_STATE_INT                   1           // Integer
#define LEX_STATE_FLOAT                 2           // Float
#define LEX_STATE_IDENT                 5           // Identifier
#define LEX_STATE_DELIM                 7           // Delimiter

#define LEX_STATE_STRING                8           // String value
#define LEX_STATE_STRING_ESCAPE         9           // Escape sequence
#define LEX_STATE_STRING_CLOSE_QUOTE    10          // String closing quote

typedef int Token;                                      // Token type
 // ---- Token Types -----------------------------------------------------------------------

#define TOKEN_TYPE_INVALID             -1
#define TOKEN_TYPE_END_OF_STREAM        0           // End of the token stream
#define TOKEN_TYPE_INT                  1           // Integer
#define TOKEN_TYPE_FLOAT                2           // Float
#define TOKEN_TYPE_IDENT                3           // Identifier

#define TOKEN_TYPE_RSRVD_VAR            4           // var/var []
#define TOKEN_TYPE_RSRVD_TRUE           5           // true
#define TOKEN_TYPE_RSRVD_FALSE          6           // false
#define TOKEN_TYPE_RSRVD_IF             7           // if
#define TOKEN_TYPE_RSRVD_ELSE           8           // else
#define TOKEN_TYPE_RSRVD_BREAK          9           // break
#define TOKEN_TYPE_RSRVD_CONTINUE       10          // continue
#define TOKEN_TYPE_RSRVD_FOR            11          // for
#define TOKEN_TYPE_RSRVD_WHILE          12          // while
#define TOKEN_TYPE_RSRVD_FUNC           13          // func
#define TOKEN_TYPE_RSRVD_RETURN         14          // return


#define TOKEN_TYPE_DELIM_COMMA          16          // ,
#define TOKEN_TYPE_DELIM_OPEN_PAREN     17          // (
#define TOKEN_TYPE_DELIM_CLOSE_PAREN    18          // )
#define TOKEN_TYPE_DELIM_OPEN_BRACE     19          // [
#define TOKEN_TYPE_DELIM_CLOSE_BRACE    20          // ]
#define TOKEN_TYPE_DELIM_OPEN_CURLY_BRACE   21      // {
#define TOKEN_TYPE_DELIM_CLOSE_CURLY_BRACE  22      // }
#define TOKEN_TYPE_DELIM_SEMICOLON      23          // ;

#define TOKEN_TYPE_STRING               24          // String


//Delimiter
#define MAX_DELIM_COUNT                 24          // Maximum number of delimiters
//----- Wrong Infomation------------------------
#define LEX_ERR_FLOATWRONG_ONLY_POINT_CHAR "only one point"
#define LEX_ERR_WRONG_STATE "wrong lex state"

typedef struct{
    int iStartLexemeIndex;
    int iEndLexemeIndex; 
    char* pstrCurrLexeme;
    char* pstrCurrSource;
    int iSourceSize;
    int iCurrState;
}Lexer;

extern Lexer lexer;
#define LexerStartIndex (lexer.iStartLexemeIndex)
#define LexerEndIndex (lexer.iEndLexemeIndex)
#define LexerState (lexer.iCurrState)

void init();
Token GetNextToken ();
char* GetCurrLexeme();
void ExitOnInvalidInputError ( char cInput );
void ExitOnInvalidInfo(char* pstrErrorInfo);



























#endif
