#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char* g_pstrSource ;
int main(int argc, char* arvc[])
{
    FILE *pSourceFile;
    if(argc < 2)
    {
        printf("need a file\n");
        exit(0);
    }
    
    if((pSourceFile = fopen(arvc[1], "rb")) == 0)
    {
        printf("open file error\n");
    }

    fseek(pSourceFile, 0, SEEK_END);
    int iSourceSize = ftell(pSourceFile);
    fseek(pSourceFile, 0, SEEK_SET);

    g_pstrSource = (char*)malloc(iSourceSize + 1);

    int i;
    for(i = 0 ; i < iSourceSize ; i ++)
    {
         
        g_pstrSource[i] =  fgetc(pSourceFile);
    }
    g_pstrSource[i] = '\0';
    fclose(pSourceFile);













    return 0;
}
