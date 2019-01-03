#ifndef XAS_H
#define XAS_H

#define MAX_SOURCE_CODE_SIZE        65536       // Maximum number of lines in source
#define MAX_SOURCE_LINE_SIZE        1024        // Maximum source line length
#define MAX_SOURCE_LIEN_NUM         1024


void Exit ();
void ExitOnError ( char * pstrErrorMssg );
void ExitOnCodeError ( char * pstrErrorMssg );

#endif
