#ifndef TOKEN_H
#define TOKEN_H

// The following constants are used as flags into an operand type bit field, hence
// their values being increasing powers of 2.

#define OP_FLAG_TYPE_INT        1           // Integer literal value
#define OP_FLAG_TYPE_FLOAT      2           // Floating-point literal value
#define OP_FLAG_TYPE_STRING     4           // Integer literal value
#define OP_FLAG_TYPE_MEM_REF    8           // Memory reference (variable or array
// index, both absolute and relative)
#define OP_FLAG_TYPE_LINE_LABEL 16          // Line label (used for jumps)
#define OP_FLAG_TYPE_FUNC_NAME  32          // Function table index (used for Call)
#define OP_FLAG_TYPE_HOST_API_CALL  64      // Host API Call table index (used for
// CallHost)
#define OP_FLAG_TYPE_REG        128         // Register
// next state is LEXEME_STATE_NOSTRING
#define INSTRS_ARRAY         {    \
            "Mov",              \
            "Add", "Sub", "Mul", "Div", "Mod", "Exp", "Neg", "Inc", "Dec",\
            "And", "Or", "XOr", "Not", "ShL", "ShR",    \
            "Concat", "GetChar", "SetChar",     \
            "Jmp", "JE", "JNE", "JG", "JL", "JGE", "JLE",   \
            "Push", "Pop",  \
            "Call", "Ret", "CallHost",  \
            "Pause", "Exit" , "Sqrt"         \
        }

#define TOKEN_TYPE_INT              0           // An integer literal
#define TOKEN_TYPE_FLOAT            1           // An floating-point literal
#define TOKEN_TYPE_STRING           2           // An string literal
#define TOKEN_TYPE_QUOTE            3           // A double-quote
#define TOKEN_TYPE_IDENT            4           // An identifier
#define TOKEN_TYPE_COLON            5           // A colon
#define TOKEN_TYPE_OPEN_BRACKET     6           // An openening bracket
#define TOKEN_TYPE_CLOSE_BRACKET    7           // An closing bracket
#define TOKEN_TYPE_COMMA            8           // A comma
#define TOKEN_TYPE_OPEN_BRACE       9           // An openening curly brace
#define TOKEN_TYPE_CLOSE_BRACE      10          // An closing curly brace
#define TOKEN_TYPE_NEWLINE          11          // A newline

#define TOKEN_TYPE_INSTR			12			// An instruction

#define TOKEN_TYPE_SETSTACKSIZE     13          // The SetStackSize directive
#define TOKEN_TYPE_VAR              14          // The Var/Var [] directives
#define TOKEN_TYPE_FUNC             15          // The Func directives
#define TOKEN_TYPE_PARAM            16          // The Param directives
#define TOKEN_TYPE_REG_RETVAL       17          // The _RetVal directives

#define TOKEN_TYPE_INVALID          18          // Error code for invalid tokens
#define END_OF_TOKEN_STREAM         19          // The end of the stream has been
// ---- Instruction Opcodes -----------------------------------------------------------

#define INSTR_MOV               0

#define INSTR_ADD               1
#define INSTR_SUB               2
#define INSTR_MUL               3
#define INSTR_DIV               4
#define INSTR_MOD               5
#define INSTR_EXP               6
#define INSTR_NEG               7
#define INSTR_INC               8
#define INSTR_DEC               9

#define INSTR_AND               10
#define INSTR_OR                11
#define INSTR_XOR               12
#define INSTR_NOT               13
#define INSTR_SHL               14
#define INSTR_SHR               15

#define INSTR_CONCAT            16
#define INSTR_GETCHAR           17
#define INSTR_SETCHAR           18

#define INSTR_JMP               19
#define INSTR_JE                20
#define INSTR_JNE               21
#define INSTR_JG                22
#define INSTR_JL                23
#define INSTR_JGE               24
#define INSTR_JLE               25

#define INSTR_PUSH              26
#define INSTR_POP               27

#define INSTR_CALL              28
#define INSTR_RET               29
#define INSTR_CALLHOST          30

#define INSTR_PAUSE             31
#define INSTR_EXIT              32

//new 
#define INSTR_SQRT              33
#define INSTR_ROL               34
#define INSTR_ROR               35

#define INSTR_ATH (OP_FLAG_TYPE_MEM_REF | OP_FLAG_TYPE_REG)
#define INSTR_ATH2 (OP_FLAG_TYPE_INT | OP_FLAG_TYPE_FLOAT | OP_FLAG_TYPE_STRING | OP_FLAG_TYPE_MEM_REF | OP_FLAG_TYPE_REG)
#define INSTR_ATH3 (OP_FLAG_TYPE_MEM_REF | OP_FLAG_TYPE_REG | OP_FLAG_TYPE_STRING)
#define INSTR_ATH4 (OP_FLAG_TYPE_MEM_REF | OP_FLAG_TYPE_REG | OP_FLAG_TYPE_INT)


#define ZeroOperatorInstrState \
    ENTRY(Ret ,INSTR_RET)  

#define OneOperatorInstrState \
    ENTRY(Neg ,INSTR_NEG ,INSTR_ATH)  \
    ENTRY(Inc ,INSTR_INC ,INSTR_ATH)  \
    ENTRY(Dec ,INSTR_DEC ,INSTR_ATH)  \
    ENTRY(Not ,INSTR_NOT ,INSTR_ATH)  \
    ENTRY(Push ,INSTR_PUSH , INSTR_ATH2)  \
    ENTRY(Pop ,INSTR_POP , INSTR_ATH)  \
    ENTRY(Call ,INSTR_CALL , OP_FLAG_TYPE_FUNC_NAME)  \
    ENTRY(CallHost ,INSTR_CALLHOST , OP_FLAG_TYPE_HOST_API_CALL)  \
    ENTRY(Pause ,INSTR_PAUSE , INSTR_ATH2)  \
    ENTRY(Jmp ,INSTR_JMP, OP_FLAG_TYPE_LINE_LABEL)  \
    ENTRY(Exit ,INSTR_EXIT , INSTR_ATH2)            \
    ENTRY(Sqrt ,INSTR_SQRT , INSTR_ATH)            



//two op
#define TwoOperatorInstrState \
    ENTRY(Mov ,INSTR_MOV ,INSTR_ATH ,INSTR_ATH2)  \
    ENTRY(Add ,INSTR_ADD ,INSTR_ATH ,INSTR_ATH2)  \
    ENTRY(Sub ,INSTR_SUB ,INSTR_ATH ,INSTR_ATH2)  \
    ENTRY(Mul ,INSTR_MUL ,INSTR_ATH ,INSTR_ATH2)  \
    ENTRY(Div ,INSTR_DIV ,INSTR_ATH ,INSTR_ATH2)  \
    ENTRY(Mod ,INSTR_MOD ,INSTR_ATH ,INSTR_ATH2)  \
    ENTRY(Exp ,INSTR_EXP ,INSTR_ATH ,INSTR_ATH2)  \
    ENTRY(And ,INSTR_AND ,INSTR_ATH ,INSTR_ATH2)  \
    ENTRY(Or ,INSTR_OR ,INSTR_ATH ,INSTR_ATH2)  \
    ENTRY(Xor ,INSTR_XOR ,INSTR_ATH ,INSTR_ATH2)  \
    ENTRY(Shl ,INSTR_SHL ,INSTR_ATH ,INSTR_ATH2)  \
    ENTRY(Shr ,INSTR_SHR ,INSTR_ATH ,INSTR_ATH2)  \
    ENTRY(Concat ,INSTR_CONCAT ,INSTR_ATH ,INSTR_ATH3 |OP_FLAG_TYPE_INT )  

#define ThreeOperatorInstrState \
    ENTRY(GetChar ,INSTR_GETCHAR ,INSTR_ATH ,INSTR_ATH3 ,INSTR_ATH4 )  \
    ENTRY(SetChar ,INSTR_SETCHAR ,INSTR_ATH ,INSTR_ATH3 ,INSTR_ATH4 )  \
    ENTRY(Je ,INSTR_JE ,INSTR_ATH2 ,INSTR_ATH2 ,OP_FLAG_TYPE_LINE_LABEL )  \
    ENTRY(Jne ,INSTR_JNE ,INSTR_ATH2 ,INSTR_ATH2 ,OP_FLAG_TYPE_LINE_LABEL )  \
    ENTRY(Jg ,INSTR_JG ,INSTR_ATH2 ,INSTR_ATH2 ,OP_FLAG_TYPE_LINE_LABEL )  \
    ENTRY(Jl ,INSTR_JL ,INSTR_ATH2 ,INSTR_ATH2 ,OP_FLAG_TYPE_LINE_LABEL )  \
    ENTRY(Jle ,INSTR_JLE ,INSTR_ATH2 ,INSTR_ATH2 ,OP_FLAG_TYPE_LINE_LABEL )  \
    ENTRY(Jge ,INSTR_JGE ,INSTR_ATH2 ,INSTR_ATH2 ,OP_FLAG_TYPE_LINE_LABEL )  



#endif
