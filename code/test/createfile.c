#include "syscall.h"
#define MAX_INPUT 256
int main()
{   
    char fileName[MAX_INPUT];
    int id;
    PrintString("Input filename: ");
    ReadString(fileName, MAX_INPUT);
    id = Create(fileName);
    PrintNum(id);
    Halt();
}