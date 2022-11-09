#include"syscall.h"
#define MAX_INPUT 256
int main()
{   
    // char fileNameOriginal[MAX_INPUT];
    // char fileNameCopy[MAX_INPUT];
    // char fileContent[MAX_INPUT];
    // OpenFileId idOriginal;
    // OpenFileId idCopy;

    // PrintString("Input original filename: ");
    // ReadString(fileNameOriginal, MAX_INPUT);
    // idOriginal = Open(fileNameOriginal);

    // if (idOriginal == -1){
    //     PrintString("File doesn't exists, can't perform copy");
    //     Halt();
    // }

    // PrintString("Input copy filename: ");
    // ReadString(fileNameCopy, MAX_INPUT);
    // idCopy = Open(fileNameCopy);

    // if(idCopy == -1){
    //     Create(fileNameCopy);
    //     idCopy = Open(fileNameCopy);
    // }

    // Read(fileContent, MAX_INPUT, idOriginal);
    // Write(fileContent, MAX_INPUT, idCopy);

    // Close(fileNameOriginal);
    // Close(fileNameCopy);

    Halt();
}