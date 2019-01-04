#ifndef XAS_H
#define XAS_H

#define MAX_SOURCE_CODE_SIZE        65536       // Maximum number of lines in source
#define MAX_SOURCE_LINE_SIZE        1024        // Maximum source line length
#define MAX_SOURCE_LIEN_NUM         1024

#define MAX_LEXEME_SIZE             256         // Maximum lexeme size

#define LEX_STATE_NO_STRING         0           // Lexemes are scanned as normal
#define LEX_STATE_IN_STRING         1           // Lexemes are scanned as strings
#define LEX_STATE_END_STRING        2           // Lexemes are scanned as normal, and the
// next state is LEXEME_STATE_NOSTRING
typedef int Token;                              // Tokenizer alias type

typedef struct _Lexer                           // The lexical analyzer/tokenizer
{
    int iCurrSourceLine;                        // Current line in the source

    unsigned int iIndex0,                       // Indices into the string
                 iIndex1;

    Token CurrToken;                            // Current token
    char pstrCurrLexeme [ MAX_LEXEME_SIZE ];    // Current lexeme

    int iCurrLexState;                          // The current lex state
}Lexer;

void Exit ();
void ExitOnError ( char * pstrErrorMssg );
void ExitOnCodeError ( char * pstrErrorMssg );

#endif
