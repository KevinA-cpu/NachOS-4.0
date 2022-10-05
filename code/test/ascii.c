#include "syscall.h"

//Printing printable characters in the ASCII table code
int main()
{   int i;
    PrintString("ASCII TABLE CODE (not including the extended version): [character] [decimal]\n");
    for(i = 32; i <= 126;i ++){
        PrintChar((char)i);
        PrintChar((char)32);
        PrintNum(i);
        PrintChar('\n');
    }
    Halt();
}