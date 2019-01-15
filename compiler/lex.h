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

typedef int Token;                                      // Token type
 // ---- Token Types -----------------------------------------------------------------------

#define TOKEN_TYPE_INVALID             -1
#define TOKEN_TYPE_END_OF_STREAM        0           // End of the token stream
#define TOKEN_TYPE_INT                  1           // Integer
#define TOKEN_TYPE_FLOAT                2           // Float


//----- Wrong Infomation------------------------
#define LEX_ERR_FLOATWRONG_ONLY_POINT_CHAR "only one point"

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
