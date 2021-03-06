#ifndef XSC_PARSER
#define XSC_PARSER
#include "xsc.h"
#include "lex.h"
#define MAX_FUNC_DECLARE_PARAM_COUNT        32      // The maximum number of parameters
typedef struct _Expr                                // Expression instance
{
    int iStackOffset;                               // The current offset of the stack
}Expr;
typedef struct Loop                                 // Loop instance
{
    int iStartTargetIndex;                          // The starting jump target
    int iEndTargetIndex;                            // The ending jump target
}Loop;
void ReadToken ( Token ReqToken );
int IsOpRelational ( int iOpType );
int IsOpLogical ( int iOpType );
void ParseSourceCode ();
void ParseStatement ();
void ParseBlock ();
void ParseVar ();
void ParseHost ();
void ParseFunc ();
void ParseExpr ();
void ParseSubExpr ();
void ParseTerm ();
void ParseFactor ();
void ParseIf ();
void ParseWhile ();
void ParseFor ();
void ParseBreak ();
void ParseContinue ();
void ParseReturn ();
void ParseAssign ();
void ParseFuncCall ();


#endif
