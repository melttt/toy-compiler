#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "ds.h"


void _strupr(char *str)
{
    while(*str != 0)
    {
        if(*str >= 'a' && *str <= 'z')
            *str -= 32;
        str ++;
    }
}
void TrimWhiteSpace(char *str)
{
    int len = strlen(str);
    int i; 
    for(i = 0 ; i < len ; i ++)
    {
        if(str[i] != ' ' && str[i] != '\t')
            break;
    }

    if(i)
        strcpy(str, str + i); 

    len -= i; 
    for(i = len - 1 ; i >= 0 ; i --)
    {
        if(str[i] == '\n' && i == len - 1) continue;
        if(str[i] != ' ' && str[i] != '\t')
        {
            str[i + 1] = '\n';
            str[i + 2] = '\0';
            break;
        }
    }
    if(i == -1)
        str[0] = '\0';
}
void StripComments(char *str)
{
    int len = strlen(str);
    int i;
    int instr = 0;
    for(i = 0 ; i < len ; i ++)
    {
        if(str[i] == '"')
        {
            instr = !instr;
        }

        if(str[i] == '\\' && instr) 
        {
            i ++;
            continue;
        }

        if(str[i] == ';' && !instr)
        {
            str[i] = '\n';
            str[i + 1] = '\0';
            break;
        }
    }
    
}


//copy from a book
/******************************************************************************************
*
*	IsCharWhitespace ()
*
*	Returns a nonzero if the given character is whitespace, or zero otherwise.
*/

int IsCharWhitespace ( char cChar )
{
    // Return true if the character is a space or tab.

    if ( cChar == ' ' || cChar == '\t' )
        return TRUE;
    else
        return FALSE;
}

/******************************************************************************************
*
*	IsCharNumeric ()
*
*	Returns a nonzero if the given character is numeric, or zero otherwise.
*/

int IsCharNumeric ( char cChar )
{
    // Return true if the character is between 0 and 9 inclusive.

    if ( cChar >= '0' && cChar <= '9' )
        return TRUE;
    else
        return FALSE;
}

/******************************************************************************************
*
*	IsCharIdentifier ()
*
*	Returns a nonzero if the given character is part of a valid identifier, meaning it's an
*	alphanumeric or underscore. Zero is returned otherwise.
*/

int IsCharIdent ( char cChar )
{
    // Return true if the character is between 0 or 9 inclusive or is an uppercase or
    // lowercase letter or underscore

    if ( ( cChar >= '0' && cChar <= '9' ) ||
         ( cChar >= 'A' && cChar <= 'Z' ) ||
         ( cChar >= 'a' && cChar <= 'z' ) ||
         cChar == '_' )
        return TRUE;
    else
        return FALSE;
}

/******************************************************************************************
*
*	IsCharDelimiter ()
*
*	Return a nonzero if the given character is a token delimeter, and return zero otherwise
*/

int IsCharDelimiter ( char cChar )
{
    // Return true if the character is a delimiter

    if ( cChar == ':' || cChar == ',' || cChar == '"' ||
         cChar == '[' || cChar == ']' ||
         cChar == '{' || cChar == '}' ||
         IsCharWhitespace ( cChar ) || cChar == '\n' )
        return TRUE;
    else
        return FALSE;
}
/******************************************************************************************
*
*	IsStringWhitespace ()
*
*	Returns a nonzero if the given string is whitespace, or zero otherwise.
*/

int IsStringWhitespace ( char * pstrString )
{
    // If the string is NULL, return false

    if ( ! pstrString )
        return FALSE;

    // If the length is zero, it's technically whitespace

    if ( strlen ( pstrString ) == 0 )
        return TRUE;

    // Loop through each character and return false if a non-whitespace is found

    for ( unsigned int iCurrCharIndex = 0; iCurrCharIndex < strlen ( pstrString ); ++ iCurrCharIndex )
        if ( ! IsCharWhitespace ( pstrString [ iCurrCharIndex ] ) && pstrString [ iCurrCharIndex ] != '\n' )
            return FALSE;

    // Otherwise return true

    return TRUE;
}

/******************************************************************************************
*
*	IsStringIdentifier ()
*
*	Returns a nonzero if the given string is composed entirely of valid identifier
*	characters and begins with a letter or underscore. Zero is returned otherwise.
*/

int IsStringIdent ( char * pstrString )
{
    // If the string is NULL return false

    if ( ! pstrString )
        return FALSE;

    // If the length of the string is zero, it's not a valid identifier

    if ( strlen ( pstrString ) == 0 )
        return FALSE;

    // If the first character is a number, it's not a valid identifier

    if ( pstrString [ 0 ] >= '0' && pstrString [ 0 ] <= '9' )
        return FALSE;

    // Loop through each character and return zero upon encountering the first invalid identifier
    // character

    for ( unsigned int iCurrCharIndex = 0; iCurrCharIndex < strlen ( pstrString ); ++ iCurrCharIndex )
        if ( ! IsCharIdent ( pstrString [ iCurrCharIndex ] ) )
            return FALSE;

    // Otherwise return true

    return TRUE;
}

/******************************************************************************************
*
*	IsStringInteger ()
*
*	Returns a nonzero if the given string is composed entire of integer characters, or zero
*	otherwise.
*/

int IsStringInteger ( char * pstrString )
{
    // If the string is NULL, it's not an integer

    if ( ! pstrString )
        return FALSE;

    // If the string's length is zero, it's not an integer

    if ( strlen ( pstrString ) == 0 )
        return FALSE;

    unsigned int iCurrCharIndex;

    // Loop through the string and make sure each character is a valid number or minus sign

    for ( iCurrCharIndex = 0; iCurrCharIndex < strlen ( pstrString ); ++ iCurrCharIndex )
        if ( ! IsCharNumeric ( pstrString [ iCurrCharIndex ] ) && ! ( pstrString [ iCurrCharIndex ] == '-' ) )
            return FALSE;

    // Make sure the minus sign only occured at the first character

    for ( iCurrCharIndex = 1; iCurrCharIndex < strlen ( pstrString ); ++ iCurrCharIndex )
        if ( pstrString [ iCurrCharIndex ] == '-' )
            return FALSE;

    return TRUE;
}

/******************************************************************************************
*
*	IsStringFloat ()
*
*	Returns a nonzero if the given string is composed entire of float characters, or zero
*	otherwise.
*/

int IsStringFloat( char * pstrString )
{
    if ( ! pstrString )
        return FALSE;

    if ( strlen ( pstrString ) == 0 )
        return FALSE;

    // First make sure we've got only numbers and radix points

    unsigned int iCurrCharIndex;

    for ( iCurrCharIndex = 0; iCurrCharIndex < strlen ( pstrString ); ++ iCurrCharIndex )
        if ( ! IsCharNumeric ( pstrString [ iCurrCharIndex ] ) && ! ( pstrString [ iCurrCharIndex ] == '.' ) && ! ( pstrString [ iCurrCharIndex ] == '-' ) )
            return FALSE;

    // Make sure only one radix point is present

    int iRadixPointFound = 0;

    for ( iCurrCharIndex = 0; iCurrCharIndex < strlen ( pstrString ); ++ iCurrCharIndex )
        if ( pstrString [ iCurrCharIndex ] == '.' )
            if ( iRadixPointFound )
                return FALSE;
            else
                iRadixPointFound = 1;

    // Make sure the minus sign only appears in the first character

    for ( iCurrCharIndex = 1; iCurrCharIndex < strlen ( pstrString ); ++ iCurrCharIndex )
        if ( pstrString [ iCurrCharIndex ] == '-' )
            return FALSE;

    // If a radix point was found, return true; otherwise, it must be an integer so return false

    if ( iRadixPointFound )
        return TRUE;
    else
        return FALSE;
}
