#ifndef UTILS_H
#define UTILS_H

void _strupr(char *str);

void TrimWhiteSpace(char *str);
void StripComments(char *str);



int IsCharWhitespace ( char cChar );
int IsCharNumeric ( char cChar );
int IsCharIdent ( char cChar );
int IsCharDelimiter ( char cChar );
int IsStringWhitespace ( char * pstrString );
int IsStringIdent ( char * pstrString );
int IsStringInteger ( char * pstrString );
int IsStringFloat( char * pstrString );
#endif
