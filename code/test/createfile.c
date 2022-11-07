#include "syscall.h"
#define MAX_INPUT 256
int main()
{   
    char filename[MAX_INPUT];
    int id;
    PrintString("Input filename: ");
    ReadString(filename, MAX_INPUT);
    id = Create(filename);
    PrintNum(id);
    Halt();
}