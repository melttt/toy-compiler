#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "xas.h"
#include "utils.h"
#include "ds.h"
#include "token.h"

char** g_ppstrSourceCode = NULL;
FILE* g_pSourceFile = NULL; 
int g_iSourceFileLine = 0;
char g_pstrExecFilename[MAX_SOURCE_LINE_SIZE] = "xas.out";
Lexer g_Lexer;

void ShutDown()
{
    int i;
    for(i = 0 ; i < g_iSourceFileLine ; i ++)
    {
        free(g_ppstrSourceCode[i]);
    }
    free(g_ppstrSourceCode);

    for(i = 0 ; i < g_iInstrStreamSize ; i ++)
    {
        if(g_pInstrStream[i].iOpCount)
        {
            free(g_pInstrStream[i].pOpList);
        }
    }
    free(g_pInstrStream);

    FreeLinkedList(&g_StringTable);
    FreeLinkedList(&g_FuncTable);
    FreeLinkedList(&g_SymbolTable);
    FreeLinkedList(&g_LabelTable);
    FreeLinkedList(&g_HostAPICallTable);

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

    
    fclose(g_pSourceFile);
    return line;
}
void InitInstrTable (){
    int iInstrIndex;

#define ENTRY(InstrName, InstrCode ) \
    iInstrIndex = AddInstrLookup (#InstrName, InstrCode, 1 ); \

    ZeroOperatorInstrState
#undef ENTRY

#define ENTRY(InstrName, InstrCode, InstrFlag ) \
    iInstrIndex = AddInstrLookup (#InstrName, InstrCode, 1 ); \
    SetOpType ( iInstrIndex, 0, InstrFlag);

    OneOperatorInstrState
#undef ENTRY

#define ENTRY(InstrName, InstrCode, InstrFlag1, InstrFlag2) \
    iInstrIndex = AddInstrLookup (#InstrName, InstrCode, 2 ); \
    SetOpType ( iInstrIndex, 0, InstrFlag1);  \
    SetOpType ( iInstrIndex, 1, InstrFlag2);

    TwoOperatorInstrState
#undef ENTRY

#define ENTRY(InstrName, InstrCode, InstrFlag1, InstrFlag2, InstrFlag3) \
    iInstrIndex = AddInstrLookup (#InstrName, InstrCode, 3 ); \
    SetOpType ( iInstrIndex, 0, InstrFlag1);  \
    SetOpType ( iInstrIndex, 1, InstrFlag2);    \
    SetOpType ( iInstrIndex, 2, InstrFlag3);

    ThreeOperatorInstrState
#undef ENTRY
    g_iInstrTableLength = iInstrIndex + 1;
}

//Lexer
void ResetLexer ()
{
    // Set the current line to the start of the file
    g_Lexer.iCurrSourceLine = 0;

    // Set both indices to point to the start of the string

    g_Lexer.iIndex0 = 0;
    g_Lexer.iIndex1 = 0;

    // Set the token type to invalid, since a token hasn't been read yet
    g_Lexer.CurrToken = TOKEN_TYPE_INVALID;
    // Set the lexing state to no strings
    g_Lexer.iCurrLexState = LEX_STATE_NO_STRING;
}

char * GetCurrLexeme ()
{
    // Simply return the pointer rather than making a copy
    return g_Lexer.pstrCurrLexeme;
}

int SkipToNextLine ()
{
    // Increment the current line

    ++ g_Lexer.iCurrSourceLine;

    // Return FALSE if we've gone past the end of the source code

    if ( g_Lexer.iCurrSourceLine >= g_iSourceFileLine )
        return FALSE;

    // Set both indices to point to the start of the string
    g_Lexer.iIndex0 = 0;
    g_Lexer.iIndex1 = 0;

    // Turn off string lexeme mode, since strings can't span multiple lines
    g_Lexer.iCurrLexState = LEX_STATE_NO_STRING;

    // Return TRUE to indicate success

    return TRUE;
}

Token GetNextToken ()
{
    //init
    g_Lexer.iIndex0 = g_Lexer.iIndex1;    
    if ( g_Lexer.iIndex0 >= strlen ( g_ppstrSourceCode [ g_Lexer.iCurrSourceLine ] ) )
    {
        if ( ! SkipToNextLine () )
            return END_OF_TOKEN_STREAM;
    }

    if ( g_Lexer.iCurrLexState == LEX_STATE_END_STRING )
        g_Lexer.iCurrLexState = LEX_STATE_NO_STRING;

    //remove space
    while ( g_Lexer.iCurrLexState != LEX_STATE_IN_STRING && IsCharWhitespace ( g_ppstrSourceCode [ g_Lexer.iCurrSourceLine ][ g_Lexer.iIndex0 ]))
    {
        ++ g_Lexer.iIndex0;
    }
    g_Lexer.iIndex1 = g_Lexer.iIndex0;

    //get string
    if(g_Lexer.iCurrLexState == LEX_STATE_IN_STRING)
    {
        for(;;g_Lexer.iIndex1 ++)
        {
            if(g_Lexer.iIndex1 >= strlen ( g_ppstrSourceCode [ g_Lexer.iCurrSourceLine ]))
            {
                g_Lexer.CurrToken = TOKEN_TYPE_INVALID;
                return g_Lexer.CurrToken;
            }
            if(g_ppstrSourceCode [ g_Lexer.iCurrSourceLine ][ g_Lexer.iIndex1 ] == '"' )
            {
                break;
            }
            if(g_ppstrSourceCode [ g_Lexer.iCurrSourceLine ][ g_Lexer.iIndex1 ] == '\\')
            {
                g_Lexer.iIndex1 ++; 
            }
        }
        
    }else{
        for(;g_Lexer.iIndex1 < strlen ( g_ppstrSourceCode [ g_Lexer.iCurrSourceLine ]);g_Lexer.iIndex1 ++)
        {
            if(IsCharDelimiter(g_ppstrSourceCode [ g_Lexer.iCurrSourceLine ][ g_Lexer.iIndex1 ]))
            {
                break;
            }
        }
        
    }

    if ( g_Lexer.iIndex1 == g_Lexer.iIndex0 )
        ++ g_Lexer.iIndex1;

    
    //copy string
    int i,j;
    int IsStr = 0;

    for(i = g_Lexer.iIndex0 ,j = 0 ; i < g_Lexer.iIndex1 ; i ++ ,j ++ )
    {
        if(g_Lexer.iCurrLexState == LEX_STATE_IN_STRING && g_ppstrSourceCode [ g_Lexer.iCurrSourceLine ][i] == '\\')
        {
            i ++;
            IsStr = 1;
        }
        g_Lexer.pstrCurrLexeme [j] = g_ppstrSourceCode [ g_Lexer.iCurrSourceLine ][i];        
    }
    g_Lexer.pstrCurrLexeme [j] = '\0';

    //convert to uppercase
    if ( g_Lexer.iCurrLexState != LEX_STATE_IN_STRING )
        _strupr ( g_Lexer.pstrCurrLexeme );


    // ---- Token Identification
    g_Lexer.CurrToken = TOKEN_TYPE_INVALID;


    if ( strlen ( g_Lexer.pstrCurrLexeme ) > 1 || g_Lexer.pstrCurrLexeme [ 0 ] != '"' || (g_Lexer.pstrCurrLexeme [ 0 ] == '"' && IsStr))
    {
        if ( g_Lexer.iCurrLexState == LEX_STATE_IN_STRING )
        {
            g_Lexer.CurrToken = TOKEN_TYPE_STRING;
            return TOKEN_TYPE_STRING;
        }
    }

    if ( strlen ( g_Lexer.pstrCurrLexeme ) == 1 )
    {

        switch ( g_Lexer.pstrCurrLexeme [ 0 ] )
        {
            // Double-Quote

            case '"':
                // If a quote is read, advance the lexing state so that strings are lexed
                // properly

                switch ( g_Lexer.iCurrLexState )
                {
                    // If we're not lexing strings, tell the lexer we're now
                    // in a string

                    case LEX_STATE_NO_STRING:
                        g_Lexer.iCurrLexState = LEX_STATE_IN_STRING;
                        break;

                        // If we're in a string, tell the lexer we just ended a string

                    case LEX_STATE_IN_STRING:
                        g_Lexer.iCurrLexState = LEX_STATE_END_STRING;
                        break;
                }

                g_Lexer.CurrToken = TOKEN_TYPE_QUOTE;
                break;

                // Comma

            case ',':
                g_Lexer.CurrToken = TOKEN_TYPE_COMMA;
                break;

                // Colon

            case ':':
                g_Lexer.CurrToken = TOKEN_TYPE_COLON;
                break;

                // Opening Bracket

            case '[':
                g_Lexer.CurrToken = TOKEN_TYPE_OPEN_BRACKET;
                break;

                // Closing Bracket

            case ']':
                g_Lexer.CurrToken = TOKEN_TYPE_CLOSE_BRACKET;
                break;

                // Opening Brace

            case '{':
                g_Lexer.CurrToken = TOKEN_TYPE_OPEN_BRACE;
                break;

                // Closing Brace

            case '}':
                g_Lexer.CurrToken = TOKEN_TYPE_CLOSE_BRACE;
                break;

                // Newline

            case '\n':
                g_Lexer.CurrToken = TOKEN_TYPE_NEWLINE;
                break;
        }
    }

    InstrLookup Instr;
    if ( IsStringInteger ( g_Lexer.pstrCurrLexeme ) )
        g_Lexer.CurrToken = TOKEN_TYPE_INT;
    if ( IsStringFloat ( g_Lexer.pstrCurrLexeme ) )
        g_Lexer.CurrToken = TOKEN_TYPE_FLOAT;
    if ( IsStringIdent ( g_Lexer.pstrCurrLexeme ) )
        g_Lexer.CurrToken = TOKEN_TYPE_IDENT;
    if ( strcmp ( g_Lexer.pstrCurrLexeme, "SETSTACKSIZE" ) == 0 )
        g_Lexer.CurrToken = TOKEN_TYPE_SETSTACKSIZE;
    if ( strcmp ( g_Lexer.pstrCurrLexeme, "VAR" ) == 0 )
        g_Lexer.CurrToken = TOKEN_TYPE_VAR;
    if ( strcmp ( g_Lexer.pstrCurrLexeme, "FUNC" ) == 0 )
        g_Lexer.CurrToken = TOKEN_TYPE_FUNC;
    if ( strcmp ( g_Lexer.pstrCurrLexeme, "PARAM" ) == 0 )
        g_Lexer.CurrToken =TOKEN_TYPE_PARAM;
    if ( strcmp ( g_Lexer.pstrCurrLexeme, "_RETVAL" ) == 0 )
        g_Lexer.CurrToken = TOKEN_TYPE_REG_RETVAL;
    if ( GetInstrByMnemonic ( g_Lexer.pstrCurrLexeme, & Instr ) )
        g_Lexer.CurrToken = TOKEN_TYPE_INSTR;

    return g_Lexer.CurrToken;
}

char GetLookAheadChar ()
{
    // We don't actually want to move the lexer's indices, so we'll make a copy of them
    int iCurrSourceLine = g_Lexer.iCurrSourceLine;
    unsigned int iIndex = g_Lexer.iIndex1;
    // If the next lexeme is not a string, scan past any potential leading whitespace
    if ( g_Lexer.iCurrLexState != LEX_STATE_IN_STRING )
    {
        // Scan through the whitespace and check for the end of the line
        while ( TRUE )
        {
            // If we've passed the end of the line, skip to the next line and reset the
            // index to zero

            if ( iIndex >= strlen ( g_ppstrSourceCode [ iCurrSourceLine ] ) )
            {
                // Increment the source code index

                iCurrSourceLine += 1;

                // If we've passed the end of the source file, just return a null character

                if ( iCurrSourceLine >= g_iSourceFileLine )
                    return 0;

                // Otherwise, reset the index to the first character on the new line

                iIndex = 0;
            }

            // If the current character is not whitespace, return it, since it's the first
            // character of the next lexeme and is thus the look-ahead

            if ( ! IsCharWhitespace ( g_ppstrSourceCode [ iCurrSourceLine ][ iIndex ] ) )
                break;

            // It is whitespace, however, so move to the next character and continue scanning

            ++ iIndex;
        }
    }

    // Return whatever character the loop left iIndex at

    return g_ppstrSourceCode [ iCurrSourceLine ][ iIndex ];
}

void Init ()
{
    // Initialize the master instruction lookup table

    InitInstrTable ();

    // Initialize tables

    InitLinkedList ( & g_SymbolTable );
    InitLinkedList ( & g_LabelTable );
    InitLinkedList ( & g_StringTable );
    InitLinkedList ( & g_FuncTable );
    InitLinkedList ( & g_HostAPICallTable );
}
//parser
void AssmblSourceFile()
{
        //global status
        g_ScriptHeader.iStackSize = 0;
        g_ScriptHeader.iIsMainFuncPresent = FALSE;

        g_iInstrStreamSize = 0;
        g_iIsSetStackSizeFound = FALSE;
        g_ScriptHeader.iGlobalDataSize = 0; 
        
        //current status
        int iIsFuncActive = FALSE;
		FuncNode * pCurrFunc;
		int iCurrFuncIndex;
		char pstrCurrFuncName [ MAX_IDENT_SIZE ];
        int iCurrFuncParamCount = 0;
        int iCurrFuncLocalDataSize = 0;

        InstrLookup CurrInstr;
        
        ResetLexer ();


        //first parse
        while(TRUE)
        {
            if (GetNextToken() == END_OF_TOKEN_STREAM)
                break;
            
            switch(g_Lexer.CurrToken)
            {
                case TOKEN_TYPE_SETSTACKSIZE:
                    if ( iIsFuncActive )
                        ExitOnCodeError ( ERROR_MSSG_LOCAL_SETSTACKSIZE );

                    if ( g_iIsSetStackSizeFound )
                        ExitOnCodeError ( ERROR_MSSG_MULTIPLE_SETSTACKSIZES );

                    if ( GetNextToken () != TOKEN_TYPE_INT )
                        ExitOnCodeError ( ERROR_MSSG_INVALID_STACK_SIZE ); 

                    g_ScriptHeader.iStackSize = atoi ( GetCurrLexeme () );

                    g_iIsSetStackSizeFound = TRUE;

                    break;
                
                case TOKEN_TYPE_VAR:
                {
                    if ( GetNextToken () != TOKEN_TYPE_IDENT )
                        ExitOnCodeError ( ERROR_MSSG_IDENT_EXPECTED );

                    char pstrIdent [ MAX_IDENT_SIZE ];
                    strcpy ( pstrIdent, GetCurrLexeme () );
                    
                    int iSize = 1;
                    if(GetLookAheadChar() == '[')
                    {
                        if ( GetNextToken () != TOKEN_TYPE_OPEN_BRACKET )
                            ExitOnCharExpectedError ( '[' );
                        
                        if ( GetNextToken () != TOKEN_TYPE_INT )
                            ExitOnCodeError ( ERROR_MSSG_INVALID_STACK_SIZE ); 

                        iSize = atoi(GetCurrLexeme());

                        if(iSize < 0)
                            ExitOnCodeError ( ERROR_MSSG_INVALID_ARRAY_SIZE );

                        if ( GetNextToken () != TOKEN_TYPE_CLOSE_BRACKET )
                            ExitOnCharExpectedError ( ']' );
                    }
                
                    // Determine the variable's index into the stack

                    // If the variable is local, then its stack index is always the local data
                    // size + 2 subtracted from zero
                    int iStackIndex;

                    if ( iIsFuncActive )
                        iStackIndex = -( iCurrFuncLocalDataSize + 2 );

                    // Otherwise it's global, so it's equal to the current global data size
                    else
                        iStackIndex = g_ScriptHeader.iGlobalDataSize;
                
                    if ( AddSymbol ( pstrIdent, iSize, iStackIndex, iCurrFuncIndex ) == -1 )
                        ExitOnCodeError ( ERROR_MSSG_IDENT_REDEFINITION );
                
                    if ( iIsFuncActive )
                        iCurrFuncLocalDataSize += iSize;
                    else
                        g_ScriptHeader.iGlobalDataSize += iSize;

                    break;
                }
                case TOKEN_TYPE_FUNC:
                {
                    if ( iIsFuncActive )
                        ExitOnCodeError ( ERROR_MSSG_NESTED_FUNC );

                    if ( GetNextToken () != TOKEN_TYPE_IDENT )
                        ExitOnCodeError ( ERROR_MSSG_IDENT_EXPECTED );

                    char* pstrFuncName = GetCurrLexeme ();

                    // Calculate the function's entry point, which is the instruction immediately
                    // following the current one, which is in turn equal to the instruction stream
                    // size
                    int iEntryPoint = g_iInstrStreamSize;

                    int iFuncIndex = AddFunc(pstrFuncName, iEntryPoint );
                    if ( iFuncIndex == -1 )
                        ExitOnCodeError ( ERROR_MSSG_FUNC_REDEFINITION );
                    
                    // Is this the _Main () function?

                    if ( strcmp ( pstrFuncName, MAIN_FUNC_NAME ) == 0 )
					{
                        g_ScriptHeader.iIsMainFuncPresent = TRUE;
						g_ScriptHeader.iMainFuncIndex = iFuncIndex;
					}

                    iIsFuncActive = TRUE;
                    strcpy ( pstrCurrFuncName, pstrFuncName );
                    iCurrFuncIndex = iFuncIndex;
                    iCurrFuncParamCount = 0;
                    iCurrFuncLocalDataSize = 0;

                    while ( GetNextToken () == TOKEN_TYPE_NEWLINE );

                    if ( g_Lexer.CurrToken != TOKEN_TYPE_OPEN_BRACE )
                        ExitOnCharExpectedError ( '{' );

                    // All functions are automatically appended with Ret, so increment the
                    // required size of the instruction stream
                    ++ g_iInstrStreamSize;
                    break;
                }

                case TOKEN_TYPE_CLOSE_BRACE:
                {
                    if ( ! iIsFuncActive )
                        ExitOnCharExpectedError ( '}' );

                    // Set the fields we've collected
                    SetFuncInfo ( pstrCurrFuncName, iCurrFuncParamCount, iCurrFuncLocalDataSize ); 
                    iIsFuncActive = FALSE;
                    break;
                }
                case TOKEN_TYPE_PARAM:
                {
                    if ( ! iIsFuncActive )
                        ExitOnCodeError ( ERROR_MSSG_GLOBAL_PARAM );

                    if ( strcmp ( pstrCurrFuncName, MAIN_FUNC_NAME ) == 0 )
						ExitOnCodeError ( ERROR_MSSG_MAIN_PARAM );


                    if(GetNextToken() != TOKEN_TYPE_IDENT)
                        ExitOnCodeError ( ERROR_MSSG_IDENT_EXPECTED );

                    iCurrFuncParamCount ++;
                    break;
                }


                //line lable
                case TOKEN_TYPE_IDENT:
                {
                    if ( GetLookAheadChar () != ':' )
                        ExitOnCodeError ( ERROR_MSSG_INVALID_INSTR );

                    // Make sure we're in a function, since labels can only appear there

                    if ( ! iIsFuncActive )
                        ExitOnCodeError ( ERROR_MSSG_GLOBAL_LINE_LABEL );

                    char * pstrIdent = GetCurrLexeme ();

                    // The target instruction is always the value of the current
                    // instruction count, which is the current size - 1

                    int iTargetIndex = g_iInstrStreamSize - 1;

                    // Save the label's function index as well

                    int iFuncIndex = iCurrFuncIndex;

                    // Try adding the label to the label table, and print an error if it
                    // already exists
                    if ( AddLabel ( pstrIdent, iTargetIndex, iFuncIndex ) == -1 )
                        ExitOnCodeError ( ERROR_MSSG_LINE_LABEL_REDEFINITION );


                    break;
                }

                case TOKEN_TYPE_INSTR:
                {
                    // Make sure we aren't in the global scope, since instructions
                    // can only appear in functions

                    if ( ! iIsFuncActive )
                        ExitOnCodeError ( ERROR_MSSG_GLOBAL_INSTR );

                    // Increment the instruction stream size

                    ++ g_iInstrStreamSize;

                    break;
                }

                default:
                    if ( g_Lexer.CurrToken != TOKEN_TYPE_NEWLINE )
                        ExitOnCodeError ( ERROR_MSSG_INVALID_INPUT );

            }
             if ( ! SkipToNextLine () )
                break;
        }
        


         // We counted the instructions, so allocate the assembled instruction stream array
        // so the next phase can begin

        g_pInstrStream = ( Instr * ) malloc ( g_iInstrStreamSize * sizeof ( Instr ) );

        // Initialize every operand list pointer to NULL
        int iCurrInstrIndex;
        for (iCurrInstrIndex = 0; iCurrInstrIndex < g_iInstrStreamSize; ++ iCurrInstrIndex )
            g_pInstrStream [ iCurrInstrIndex ].pOpList = NULL;

        // Set the current instruction index to zero
        g_iCurrInstrIndex = 0;

        // ---- Perform the second pass over the source
        // Reset the lexer so we begin at the top of the source again
        ResetLexer ();

        //second parse

        while(TRUE)
        {
            if(GetNextToken() == END_OF_TOKEN_STREAM)
                break;
            switch(g_Lexer.CurrToken)
            {
                case TOKEN_TYPE_FUNC:
                {

                    GetNextToken ();

                    // Use the identifier (the current lexeme) to get it's corresponding function
                    // from the table

                    pCurrFunc = GetFuncByName ( GetCurrLexeme () );

                    // Set the active function flag

                    iIsFuncActive = TRUE;

					// Set the parameter count to zero, since we'll need to count parameters as
					// we parse Param directives

					iCurrFuncParamCount = 0;

					// Save the function's index

					iCurrFuncIndex = pCurrFunc->iIndex;

                    // Read any number of line breaks until the opening brace is found

                    while ( GetNextToken () == TOKEN_TYPE_NEWLINE );

                    break;
                }

                case TOKEN_TYPE_CLOSE_BRACE:
                {
                    iIsFuncActive = FALSE;

                    //main func
                    if(strcmp(pCurrFunc->pstrName, MAIN_FUNC_NAME) == 0)
                    {
                        //exit 0 
                        g_pInstrStream [ g_iCurrInstrIndex ].iOpcode = INSTR_EXIT;
                        g_pInstrStream [ g_iCurrInstrIndex ].iOpCount = 1;
                        
                        g_pInstrStream [ g_iCurrInstrIndex ].pOpList = ( Op * ) malloc ( sizeof ( Op ) );
                        g_pInstrStream [ g_iCurrInstrIndex ].pOpList [ 0 ].iType = OP_TYPE_INT;
                        g_pInstrStream [ g_iCurrInstrIndex ].pOpList [ 0 ].iIntLiteral = 0;

                    }else{
                    //general func    
                        g_pInstrStream [ g_iCurrInstrIndex ].iOpcode = INSTR_RET;
                        g_pInstrStream [ g_iCurrInstrIndex ].iOpCount = 0;
                        g_pInstrStream [ g_iCurrInstrIndex ].pOpList = NULL;

                    }
                    
                    g_iCurrInstrIndex ++;
                    break;
                }


                case TOKEN_TYPE_PARAM:
                {
                    if ( GetNextToken () != TOKEN_TYPE_IDENT )
						ExitOnCodeError ( ERROR_MSSG_IDENT_EXPECTED ); 
                    char * pstrIdent = GetCurrLexeme ();

					// Calculate the parameter's stack index

					int iStackIndex = -( pCurrFunc->iLocalDataSize + 2 + ( iCurrFuncParamCount + 1 ) );

					// Add the parameter to the symbol table

					if ( AddSymbol ( pstrIdent, 1, iStackIndex, iCurrFuncIndex ) == -1 )
						ExitOnCodeError ( ERROR_MSSG_IDENT_REDEFINITION );

				    // Increment the current parameter count
                    ++ iCurrFuncParamCount;


                    break;
                } 

                case TOKEN_TYPE_INSTR:
                {
                    // Get the instruction's info using the current lexeme (the mnemonic )
                    GetInstrByMnemonic ( GetCurrLexeme (), & CurrInstr );

                    // Write the opcode to the stream
                    g_pInstrStream [ g_iCurrInstrIndex ].iOpcode = CurrInstr.iOpcode;

                    // Write the operand count to the stream
                    g_pInstrStream [ g_iCurrInstrIndex ].iOpCount = CurrInstr.iOpCount;

                    // Allocate space to hold the operand list
                    Op * pOpList = ( Op * ) malloc ( CurrInstr.iOpCount * sizeof ( Op ) ); 

                    int iCurrOpIndex;
                    for ( iCurrOpIndex = 0; iCurrOpIndex < CurrInstr.iOpCount; ++ iCurrOpIndex ) 
                    {
                        OpTypes CurrOpTypes = CurrInstr.OpList [ iCurrOpIndex ];
                        Token InitOpToken = GetNextToken ();

                        switch ( InitOpToken )
                        {

                            case TOKEN_TYPE_INT:
                                if ( CurrOpTypes & OP_FLAG_TYPE_INT )
                                {
                                    pOpList [ iCurrOpIndex ].iType = OP_TYPE_INT;
                                    pOpList [ iCurrOpIndex ].iIntLiteral = atoi ( GetCurrLexeme () );
                                }else{
                                    ExitOnCodeError ( ERROR_MSSG_INVALID_OP );
                                }
                                break;

                            case TOKEN_TYPE_FLOAT:
                                if ( CurrOpTypes & OP_FLAG_TYPE_FLOAT )
                                {
                                    pOpList [ iCurrOpIndex ].iType = OP_TYPE_FLOAT;
                                    pOpList [ iCurrOpIndex ].fFloatLiteral = ( float ) atof ( GetCurrLexeme () );
                                }
                                else{
                                    ExitOnCodeError ( ERROR_MSSG_INVALID_OP );
                                }
                                break;

                            case TOKEN_TYPE_QUOTE:
                            {
                                if ( CurrOpTypes & OP_FLAG_TYPE_STRING )
                                {

                                    GetNextToken ();
                                    
                                    if(g_Lexer.CurrToken == TOKEN_TYPE_QUOTE)
                                    {
                                        //empty string
                                        pOpList [ iCurrOpIndex ].iType = OP_TYPE_INT;
									    pOpList [ iCurrOpIndex ].iIntLiteral = 0;
                                    }else if(g_Lexer.CurrToken == TOKEN_TYPE_STRING)
                                    {
                                        char * pstrString = GetCurrLexeme ();
                                        // Add the string to the table, or get the index of
                                        // the existing copy
                                        int iStringIndex = AddString ( & g_StringTable, pstrString );
                                        pOpList [ iCurrOpIndex ].iType = OP_TYPE_STRING_INDEX;
                                        pOpList [ iCurrOpIndex ].iStringTableIndex = iStringIndex;
                                        if ( GetNextToken () != TOKEN_TYPE_QUOTE )
									            ExitOnCharExpectedError ( '\\' );
                                    }else{
                                        ExitOnCodeError ( ERROR_MSSG_INVALID_STRING );
                                    }

                                }else{
                                    ExitOnCodeError ( ERROR_MSSG_INVALID_OP );

                                }
                                break;
                            }

                            case TOKEN_TYPE_REG_RETVAL:
                                if ( CurrOpTypes & OP_FLAG_TYPE_REG )
                                {
                                    // Set a register type

                                    pOpList [ iCurrOpIndex ].iType = OP_TYPE_REG;
                                    pOpList [ iCurrOpIndex ].iReg = 0;
                                }
                                else
                                    ExitOnCodeError ( ERROR_MSSG_INVALID_OP );

                                break;
                            case TOKEN_TYPE_IDENT:
                            {
                                if ( CurrOpTypes & OP_FLAG_TYPE_MEM_REF )
								{ 
                                    char pstrIdent [ MAX_IDENT_SIZE ];
									strcpy ( pstrIdent, GetCurrLexeme () );

                                    if ( ! GetSymbolByIdent ( pstrIdent, iCurrFuncIndex ) )
										ExitOnCodeError ( ERROR_MSSG_UNDEFINED_IDENT );
                                    int iBaseIndex = GetStackIndexByIdent ( pstrIdent, iCurrFuncIndex );

                                    if ( GetLookAheadChar () != '[' ){

                                        if ( GetSizeByIdent ( pstrIdent, iCurrFuncIndex ) > 1 )
											ExitOnCodeError ( ERROR_MSSG_INVALID_ARRAY_NOT_INDEXED );

                                        pOpList [ iCurrOpIndex ].iType = OP_TYPE_ABS_STACK_INDEX;
										pOpList [ iCurrOpIndex ].iIntLiteral = iBaseIndex;
                                    }else{

                                        if ( GetNextToken () != TOKEN_TYPE_OPEN_BRACKET )
											ExitOnCharExpectedError ( '[' );
                                        Token IndexToken = GetNextToken ();

                                        if ( IndexToken == TOKEN_TYPE_INT )
                                        {
                                            int iOffsetIndex = atoi ( GetCurrLexeme () );

											pOpList [ iCurrOpIndex ].iType = OP_TYPE_ABS_STACK_INDEX;
											pOpList [ iCurrOpIndex ].iStackIndex = iBaseIndex + iOffsetIndex;

                                        }else if(IndexToken == TOKEN_TYPE_IDENT )
                                        {
                                            char * pstrIndexIdent = GetCurrLexeme (); 
                                            if ( ! GetSymbolByIdent ( pstrIndexIdent, iCurrFuncIndex ) )
												ExitOnCodeError ( ERROR_MSSG_UNDEFINED_IDENT );

											if ( GetSizeByIdent ( pstrIndexIdent, iCurrFuncIndex ) > 1 )
												ExitOnCodeError ( ERROR_MSSG_INVALID_ARRAY_INDEX );                                            
                                            int iOffsetIndex = GetStackIndexByIdent ( pstrIndexIdent, iCurrFuncIndex );
											pOpList [ iCurrOpIndex ].iType = OP_TYPE_REL_STACK_INDEX;
											pOpList [ iCurrOpIndex ].iStackIndex = iBaseIndex;
											pOpList [ iCurrOpIndex ].iOffsetIndex = iOffsetIndex;
                                        }else{
                                            ExitOnCodeError ( ERROR_MSSG_INVALID_ARRAY_INDEX );
                                        }

                                        if ( GetNextToken () != TOKEN_TYPE_CLOSE_BRACKET )
											ExitOnCharExpectedError ( '[' );
                                    }
                                }
                                if ( CurrOpTypes & OP_FLAG_TYPE_LINE_LABEL )
                                {
                                    char * pstrLabelIdent = GetCurrLexeme ();
                                    LabelNode * pLabel = GetLabelByIdent ( pstrLabelIdent, iCurrFuncIndex );

                                    if ( ! pLabel )
                                        ExitOnCodeError ( ERROR_MSSG_UNDEFINED_LINE_LABEL );

                                    pOpList [ iCurrOpIndex ].iType = OP_TYPE_INSTR_INDEX;
                                    pOpList [ iCurrOpIndex ].iInstrIndex = pLabel->iTargetIndex;
                                }

                                if ( CurrOpTypes & OP_FLAG_TYPE_FUNC_NAME )
                                {
                                    // Get the current lexeme, which is the function name

                                    char * pstrFuncName = GetCurrLexeme ();

                                    // Use the function name to get the function's information

                                    FuncNode * pFunc = GetFuncByName ( pstrFuncName );

                                    // Make sure the function exists

                                    if ( ! pFunc )
                                        ExitOnCodeError ( ERROR_MSSG_UNDEFINED_FUNC );

                                    // Set the operand type to function index and set its data
                                    // field

                                    pOpList [ iCurrOpIndex ].iType = OP_TYPE_FUNC_INDEX;
                                    pOpList [ iCurrOpIndex ].iFuncIndex = pFunc->iIndex;
                                }

                                if ( CurrOpTypes & OP_FLAG_TYPE_HOST_API_CALL )
								{
									// Get the current lexeme, which is the host API call

									char * pstrHostAPICall = GetCurrLexeme ();

									// Add the call to the table, or get the index of the
									// existing copy

									int iIndex = AddString ( & g_HostAPICallTable, pstrHostAPICall );

									// Set the operand type to host API call index and set its
									// data field

									pOpList [ iCurrOpIndex ].iType = OP_TYPE_HOST_API_CALL_INDEX;
									pOpList [ iCurrOpIndex ].iHostAPICallIndex = iIndex;
								}


                                break;
                            }
                            default:
                                ExitOnCodeError ( ERROR_MSSG_INVALID_OP );
                        
                        }
                        if ( iCurrOpIndex < CurrInstr.iOpCount - 1 )
                            if ( GetNextToken () != TOKEN_TYPE_COMMA )
                                ExitOnCharExpectedError ( ',' );

                    }
                    //if ( GetNextToken () != TOKEN_TYPE_NEWLINE )
                    //    ExitOnCodeError ( ERROR_MSSG_INVALID_INPUT );

                    g_pInstrStream [ g_iCurrInstrIndex ].pOpList = pOpList;
                    ++ g_iCurrInstrIndex;

                    break;
                }


            }
            if ( ! SkipToNextLine () )
                break;
        }    

}

void PrintText()
{
    printf("\n------------------------\n");
    int i;
    for(i = 0 ; i < g_iSourceFileLine; i ++)
    {
        printf("%s", g_ppstrSourceCode[i]);
    }
    printf("------------------------\n");
}
void PrintTableInfo()
{
    int i;
    LinkedListNode* tmp = g_StringTable.pHead;
    printf("String Table:\n");
    while(tmp)
    {
        printf("%s\n", (char*)tmp->pData);
        tmp = tmp->pNext;
    }

    tmp = g_FuncTable.pHead;
    printf("Function Table:\n");
    while(tmp)
    {
        printf("name:%s EP:%d paraC:%d lsize:%d\n", ((FuncNode*)tmp->pData)->pstrName, ((FuncNode*)tmp->pData)->iEntryPoint, ((FuncNode*)tmp->pData)->iParamCount, ((FuncNode*)tmp->pData)->iLocalDataSize);
        tmp = tmp->pNext;
    }

    tmp = g_SymbolTable.pHead;
    printf("Symble Table:\n");
    while(tmp)
    {
        printf("name:%s S:%d SI:%d FI:%d\n", ((SymbolNode*)tmp->pData)->pstrIdent, ((SymbolNode*)tmp->pData)->iSize, ((SymbolNode*)tmp->pData)->iStackIndex, ((SymbolNode*)tmp->pData)->iFuncIndex);
        tmp = tmp->pNext;
    }

    tmp = g_LabelTable.pHead;
    printf("Label Table:\n");
    while(tmp)
    {
        printf("name:%s TI:%d FI:%d\n", ((LabelNode*)tmp->pData)->pstrIdent, ((LabelNode*)tmp->pData)->iTargetIndex,((LabelNode*)tmp->pData)->iFuncIndex);
        tmp = tmp->pNext;
    }

    tmp = g_HostAPICallTable.pHead;
    printf("HostAPICallTable:\n");
    while(tmp)
    {
        printf("%s\n", (char*)tmp->pData);
        tmp = tmp->pNext;
    }

    printf("Instruction Stream\n");
    for(i = 0 ; i < g_iInstrStreamSize ; i ++)
    {
        printf("OP:%d COUNT:%d\n",g_pInstrStream[i].iOpcode, g_pInstrStream[i].iOpCount);
    }
}
void BuildXSE ()
{

    // ---- Open the output file
    FILE * pExecFile;
    if ( ! ( pExecFile = fopen ( g_pstrExecFilename, "wb" ) ) )
        ExitOnError ( "Could not open executable file for output" );

    // ---- Write the header
    // Write the ID string (4 bytes)

    fwrite ( XSE_ID_STRING, 4, 1, pExecFile );
    // Write the version (1 byte for each component, 2 total)

    char cVersionMajor = VERSION_MAJOR,
         cVersionMinor = VERSION_MINOR;
    fwrite ( & cVersionMajor, 1, 1, pExecFile );
    fwrite ( & cVersionMinor, 1, 1, pExecFile );

    // Write the stack size (4 bytes)

    fwrite ( & g_ScriptHeader.iStackSize, 4, 1, pExecFile );

    // Write the global data size (4 bytes )

    fwrite ( & g_ScriptHeader.iGlobalDataSize, 4, 1, pExecFile );

    // Write the _Main () flag (1 byte)

    char cIsMainPresent = 0;
    if ( g_ScriptHeader.iIsMainFuncPresent )
        cIsMainPresent = 1;
    fwrite ( & cIsMainPresent, 1, 1, pExecFile );

    // Write the _Main () function index (4 bytes)

    fwrite ( & g_ScriptHeader.iMainFuncIndex, 4, 1, pExecFile );

    // ---- Write the instruction stream

    // Output the instruction count (4 bytes)

    fwrite ( & g_iInstrStreamSize, 4, 1, pExecFile );

    // Loop through each instruction and write its data out

    for ( int iCurrInstrIndex = 0; iCurrInstrIndex < g_iInstrStreamSize; ++ iCurrInstrIndex )
    {
        // Write the opcode (2 bytes)

        short sOpcode = g_pInstrStream [ iCurrInstrIndex ].iOpcode;
        fwrite ( & sOpcode, 2, 1, pExecFile );

        // Write the operand count (1 byte)

        char iOpCount = g_pInstrStream [ iCurrInstrIndex ].iOpCount;
        fwrite ( & iOpCount, 1, 1, pExecFile );

        // Loop through the operand list and print each one out

        for ( int iCurrOpIndex = 0; iCurrOpIndex < iOpCount; ++ iCurrOpIndex )
        {
            // Make a copy of the operand pointer for convinience

            Op CurrOp = g_pInstrStream [ iCurrInstrIndex ].pOpList [ iCurrOpIndex ];

            // Create a character for holding operand types (1 byte)

            char cOpType = CurrOp.iType;
            fwrite ( & cOpType, 1, 1, pExecFile );

            // Write the operand depending on its type

            switch ( CurrOp.iType )
            {
                // Integer literal

                case OP_TYPE_INT:
                    fwrite ( & CurrOp.iIntLiteral, sizeof ( int ), 1, pExecFile );
                    break;

                    // Floating-point literal

                case OP_TYPE_FLOAT:
                    fwrite ( & CurrOp.fFloatLiteral, sizeof ( float ), 1, pExecFile );
                    break;

                    // String index

                case OP_TYPE_STRING_INDEX:
                    fwrite ( & CurrOp.iStringTableIndex, sizeof ( int ), 1, pExecFile );
                    break;

                    // Instruction index

                case OP_TYPE_INSTR_INDEX:
                    fwrite ( & CurrOp.iInstrIndex, sizeof ( int ), 1, pExecFile );
                    break;

                    // Absolute stack index

                case OP_TYPE_ABS_STACK_INDEX:
                    fwrite ( & CurrOp.iStackIndex, sizeof ( int ), 1, pExecFile );
                    break;

                    // Relative stack index

                case OP_TYPE_REL_STACK_INDEX:
                    fwrite ( & CurrOp.iStackIndex, sizeof ( int ), 1, pExecFile );
                    fwrite ( & CurrOp.iOffsetIndex, sizeof ( int ), 1, pExecFile );
                    break;

                    // Function index

                case OP_TYPE_FUNC_INDEX:
                    fwrite ( & CurrOp.iFuncIndex, sizeof ( int ), 1, pExecFile );
                    break;

                    // Host API call index

                case OP_TYPE_HOST_API_CALL_INDEX:
                    fwrite ( & CurrOp.iHostAPICallIndex, sizeof ( int ), 1, pExecFile );
                    break;

                    // Register

                case OP_TYPE_REG:
                    fwrite ( & CurrOp.iReg, sizeof ( int ), 1, pExecFile );
                    break;
            }
        }
    }

    // Create a node pointer for traversing the lists

    int iCurrNode;
    LinkedListNode * pNode;

    // ---- Write the string table

    // Write out the string count (4 bytes)

    fwrite ( & g_StringTable.iNodeCount, 4, 1, pExecFile );

    // Set the pointer to the head of the list

    pNode = g_StringTable.pHead;

    // Create a character for writing parameter counts

    char cParamCount;

    // Loop through each node in the list and write out its string

    for ( iCurrNode = 0; iCurrNode < g_StringTable.iNodeCount; ++ iCurrNode )
    {
        // Copy the string and calculate its length

        char * pstrCurrString = ( char * ) pNode->pData;
        int iCurrStringLength = strlen ( pstrCurrString );

        // Write the length (4 bytes), followed by the string data (N bytes)

        fwrite ( & iCurrStringLength, 4, 1, pExecFile );
        fwrite ( pstrCurrString, strlen ( pstrCurrString ), 1, pExecFile );

        // Move to the next node

        pNode = pNode->pNext;
    }

    // ---- Write the function table

    // Write out the function count (4 bytes)

    fwrite ( & g_FuncTable.iNodeCount, 4, 1, pExecFile );

    // Set the pointer to the head of the list

    pNode = g_FuncTable.pHead;

    // Loop through each node in the list and write out its function info

    for ( iCurrNode = 0; iCurrNode < g_FuncTable.iNodeCount; ++ iCurrNode )
    {
        // Create a local copy of the function

        FuncNode * pFunc = ( FuncNode * ) pNode->pData;

        // Write the entry point (4 bytes)

        fwrite ( & pFunc->iEntryPoint, sizeof ( int ), 1, pExecFile );

        // Write the parameter count (1 byte)

        cParamCount = pFunc->iParamCount;
        fwrite ( & cParamCount, 1, 1, pExecFile );

        // Write the local data size (4 bytes)

        fwrite ( & pFunc->iLocalDataSize, sizeof ( int ), 1, pExecFile );

        // Move to the next node

        pNode = pNode->pNext;
    }

    // ---- Write the host API call table

    // Write out the call count (4 bytes)

    fwrite ( & g_HostAPICallTable.iNodeCount, 4, 1, pExecFile );

    // Set the pointer to the head of the list

    pNode = g_HostAPICallTable.pHead;

    // Loop through each node in the list and write out its string

    for ( iCurrNode = 0; iCurrNode < g_HostAPICallTable.iNodeCount; ++ iCurrNode )
    {
        // Copy the string pointer and calculate its length

        char * pstrCurrHostAPICall = ( char * ) pNode->pData;
        char cCurrHostAPICallLength = strlen ( pstrCurrHostAPICall );

        // Write the length (1 byte), followed by the string data (N bytes)

        fwrite ( & cCurrHostAPICallLength, 1, 1, pExecFile );
        fwrite ( pstrCurrHostAPICall, strlen ( pstrCurrHostAPICall ), 1, pExecFile );

        // Move to the next node

        pNode = pNode->pNext;
    }

    // ---- Close the output file

    fclose ( pExecFile );
}



int main(int argc, char * argv [])
{
    if(argc == 1)
    {
        printf("need a file!\n");
        exit(1);
    }

    if(argc == 3)
    {
        strcpy(g_pstrExecFilename , argv[2]); 
    }

    Init();
    g_iSourceFileLine = LoadSourceFile(argv[1]);
    AssmblSourceFile();
    PrintTableInfo();
    BuildXSE();

    ShutDown();
    return 0;
}


//copy from a book
void Exit ()
{

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
}

void ExitOnCharExpectedError ( char cChar )
{
    // Create an error message based on the character

    char * pstrErrorMssg = ( char * ) malloc ( strlen ( "' ' expected" ) );
    sprintf ( pstrErrorMssg, "'%c' expected", cChar );

    // Exit on the code error

    ExitOnCodeError ( pstrErrorMssg );
}
