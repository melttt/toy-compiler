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

// ---- Error Strings ---------------------------------------------------------------------

// The following macros are used to represent assembly-time error strings

#define ERROR_MSSG_INVALID_INPUT	\
    "Invalid input"

#define ERROR_MSSG_LOCAL_SETSTACKSIZE	\
    "SetStackSize can only appear in the global scope"

#define ERROR_MSSG_INVALID_STACK_SIZE	\
    "Invalid stack size"

#define ERROR_MSSG_MULTIPLE_SETSTACKSIZES	\
    "Multiple instances of SetStackSize illegal"

#define ERROR_MSSG_IDENT_EXPECTED	\
    "Identifier expected"

#define ERROR_MSSG_INVALID_ARRAY_SIZE	\
    "Invalid array size"

#define ERROR_MSSG_IDENT_REDEFINITION	\
    "Identifier redefinition"

#define ERROR_MSSG_UNDEFINED_IDENT	\
    "Undefined identifier"

#define ERROR_MSSG_NESTED_FUNC	\
    "Nested functions illegal"

#define ERROR_MSSG_FUNC_REDEFINITION	\
    "Function redefinition"

#define ERROR_MSSG_UNDEFINED_FUNC	\
    "Undefined function"

#define ERROR_MSSG_GLOBAL_PARAM	\
    "Parameters can only appear inside functions"

#define ERROR_MSSG_MAIN_PARAM	\
    "_Main () function cannot accept parameters"

#define ERROR_MSSG_GLOBAL_LINE_LABEL	\
    "Line labels can only appear inside functions"

#define ERROR_MSSG_LINE_LABEL_REDEFINITION	\
    "Line label redefinition"

#define ERROR_MSSG_UNDEFINED_LINE_LABEL	\
    "Undefined line label"

#define ERROR_MSSG_GLOBAL_INSTR	\
    "Instructions can only appear inside functions"

#define ERROR_MSSG_INVALID_INSTR	\
    "Invalid instruction"

#define ERROR_MSSG_INVALID_OP	\
    "Invalid operand"

#define ERROR_MSSG_INVALID_STRING	\
    "Invalid string"

#define ERROR_MSSG_INVALID_ARRAY_NOT_INDEXED	\
    "Arrays must be indexed"

#define ERROR_MSSG_INVALID_ARRAY	\
    "Invalid array"

#define ERROR_MSSG_INVALID_ARRAY_INDEX	\
    "Invalid array index"




void Exit ();
void ExitOnError ( char * pstrErrorMssg );
void ExitOnCodeError ( char * pstrErrorMssg );
void ExitOnCharExpectedError ( char cChar );

#endif
