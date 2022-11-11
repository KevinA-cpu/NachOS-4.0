#include"syscall.h"
#define MAX_INPUT 256
int main()
{   
    char fileNameOriginal[MAX_INPUT];
    char fileNameCopy[MAX_INPUT];
    char fileContent[MAX_INPUT];
    int idOriginal;
    int idCopy;

    PrintString("Input original filename: ");
    ReadString(fileNameOriginal, MAX_INPUT);
    idOriginal = Open(fileNameOriginal);

    if (idOriginal == -1){
        PrintString("File doesn't exists, can't perform concatenate");
        Halt();
    }

    PrintString("Input copy filename: ");
    ReadString(fileNameCopy, MAX_INPUT);
    idCopy = Open(fileNameCopy);

    if(idCopy == -1){
        Create(fileNameCopy);
        idCopy = Open(fileNameCopy);
    }

    Read(fileContent, MAX_INPUT, idOriginal);
    Seek(-1, idCopy);
    Write(fileContent, MAX_INPUT, idCopy);

    Close(idOriginal);
    Close(idCopy);

    Halt();
}