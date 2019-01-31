#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preprocessor.h"
#include "lex.h"

int HasNextChar(LinkedListNode* pCurrNode ,int iCurrLineIndex)
{
    if((pCurrNode && iCurrLineIndex < strlen(pCurrNode->pData)) || (pCurrNode->pNext)) 
        return TRUE;

    return FALSE;

}

void AddIndex(LinkedListNode** pCurrNode ,int* iCurrLineIndex)
{
    if(pCurrNode == NULL) return;

    if((*pCurrNode) && (*iCurrLineIndex) + 1 < strlen((*pCurrNode)->pData) )
    {
        *iCurrLineIndex = (*iCurrLineIndex) + 1;
        return; 
    }

    if((*pCurrNode) && (*pCurrNode)->pNext)
    {
        *iCurrLineIndex = 0;
        *pCurrNode =  (*pCurrNode)->pNext;

        return;
    }
    return;
}
//remove // /*
int StripComment()
{
    LinkedListNode* pCurrNode = g_SourceCode.pHead;   
    char *pstrCurrSourceLine;
    int iCurrLineIndex = 0;
    int iState = PREP_STATE_NORMAL;
    char cCurrChar, cNextChar;
    int iIsFinish = FALSE;

    for(iCurrLineIndex = 0 ;HasNextChar(pCurrNode, iCurrLineIndex) ; AddIndex(&pCurrNode, &iCurrLineIndex))
    {
        pstrCurrSourceLine = pCurrNode->pData;
        cCurrChar = pstrCurrSourceLine[iCurrLineIndex];
        //printf("%c", cCurrChar);

        //jmp null string
        if(cCurrChar == '\0')
        {
            continue;
        }
        switch(iState)
        {
            case PREP_STATE_NORMAL:
            {
                //maybe bug,there are a '\n' behind the '\\'
                if(cCurrChar == '/' && HasNextChar(pCurrNode, iCurrLineIndex + 1))
                {
                    cNextChar = pstrCurrSourceLine[iCurrLineIndex + 1];
                    if(cNextChar == '*')
                    {
                        iState = PREP_STATE_COMMENTS;
                        pstrCurrSourceLine[iCurrLineIndex] = PREP_SPACE;
                        pstrCurrSourceLine[iCurrLineIndex + 1] = PREP_SPACE;
                        AddIndex(&pCurrNode, &iCurrLineIndex);
                    }else if(cNextChar == '/')
                    {
                        iState = PREP_STATE_ONELINE_COMMENT;
                        pstrCurrSourceLine[iCurrLineIndex] = '\n';
                    }
                    
                }
                break;
            }
            case PREP_STATE_ONELINE_COMMENT:
            {
                if(cCurrChar == '/')
                {
                     pstrCurrSourceLine[iCurrLineIndex] = '\0';
                     iState = PREP_STATE_NORMAL;
                }else{
                     iState = PREP_STATE_WRONG;
                     iIsFinish = TRUE;
                }
                break;
            }
            case PREP_STATE_COMMENTS:
            {
                if(cCurrChar == '*' && HasNextChar(pCurrNode, iCurrLineIndex + 1))
                {
                    cNextChar = pstrCurrSourceLine[iCurrLineIndex + 1];
                    if(cNextChar == '/')
                    {
                        iState = PREP_STATE_NORMAL;
                        pstrCurrSourceLine[iCurrLineIndex] = PREP_SPACE;
                        pstrCurrSourceLine[iCurrLineIndex + 1] = PREP_SPACE;
                        AddIndex(&pCurrNode, &iCurrLineIndex);
                    }
                }else{

                    pstrCurrSourceLine[iCurrLineIndex] = PREP_SPACE;
                }
                break;
            }
            default:
                iState = PREP_STATE_WRONG;
                iIsFinish = TRUE;
        }
        if(iIsFinish)
        {
            break;
        }
    }
    
    return iState == PREP_STATE_WRONG ? FALSE : TRUE;

}




void PrintFile()
{
    LinkedListNode* pCurrNode = g_SourceCode.pHead;   
    while(pCurrNode->pNext)
    {
        printf("%s", (char*)pCurrNode->pData);
        pCurrNode = pCurrNode->pNext;
    }

}

