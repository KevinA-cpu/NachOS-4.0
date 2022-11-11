#include "syscall.h"
#define MAX_INPUT 256
int main()
{   
    char fileName[MAX_INPUT];
    char fileContent[MAX_INPUT];
    int id;

    PrintString("Input filename: ");
    ReadString(fileName, MAX_INPUT);
    id = Open(fileName);

    PrintNum(Read(fileContent, MAX_INPUT, id));
    PrintString(fileContent);

    Close(id);
    Halt();
}