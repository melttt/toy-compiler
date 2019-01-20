#ifndef XSC_PREPROCESSOR
#define XSC_PREPROCESSOR
#include "xsc.h"
#define PREP_STATE_NORMAL 0
#define PREP_STATE_ONELINE_COMMENT 1
#define PREP_STATE_COMMENTS 2
#define PREP_STATE_WRONG 3

#define PREP_SPACE ' '

int StripComment();
void PrintFile();
#endif
